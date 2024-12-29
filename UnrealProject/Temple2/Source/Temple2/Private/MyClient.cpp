// Fill out your copyright notice in the Description page of Project Settings.


#include "MyClient.h"

#include <winsock2.h>

#include "Common/UdpSocketBuilder.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include <winsock2.h>


class DataParser {
public:
	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) = 0;
	virtual ~DataParser() {}

protected:
	static float ntohf(uint32 netfloat) {
		uint32 hostfloat = ntohl(netfloat);
		return *(float*)&hostfloat;
	}
};

class IntegerParser : public DataParser {
public:
	int32 Value;

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		if (DataPtr + sizeof(int32) > DataEnd) return false;
		FMemory::Memcpy(&Value, DataPtr, sizeof(int32));
		Value = ntohl(Value);
		DataPtr += sizeof(int32);
		return true;
	}
};

class FloatParser : public DataParser {
public:
	float Value;

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		if (DataPtr + sizeof(float) > DataEnd) return false;
		uint32 NetFloat;
		FMemory::Memcpy(&NetFloat, DataPtr, sizeof(uint32));
		Value = ntohf(NetFloat);
		DataPtr += sizeof(uint32);
		return true;
	}
};
class StringParser : public DataParser {
public:
	FString Value;

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		uint32 StringLength;
		if (DataPtr + sizeof(uint32) > DataEnd) return false;
		FMemory::Memcpy(&StringLength, DataPtr, sizeof(uint32));
		StringLength = ntohl(StringLength);
		DataPtr += sizeof(uint32);

		if (StringLength > 0 && DataPtr + StringLength <= DataEnd) {
			Value = FString(FUTF8ToTCHAR((const ANSICHAR*)DataPtr, StringLength));
			DataPtr += StringLength;
			return true;
		}
		return false;
	}
};

class BooleanParser : public DataParser {
public:
	bool Value;

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		if (DataPtr + sizeof(bool) > DataEnd) return false;
		FMemory::Memcpy(&Value, DataPtr, sizeof(bool));
		DataPtr += sizeof(bool);
		return true;
	}
};

class ByteArrayParser : public DataParser {
public:
	uint8 ByteArray[4]; // �̶���С���ֽ�����

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		// ����Ƿ����㹻�������������ֽ�����
		if (DataPtr + sizeof(ByteArray) <= DataEnd) {
			FMemory::Memcpy(ByteArray, DataPtr, sizeof(ByteArray)); // ��������
			DataPtr += sizeof(ByteArray); // �ƶ�ָ��
			return true; // ���سɹ�
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to parse ByteArray: Not enough data"));
			return false; // ����ʧ��
		}
	}
};



PRAGMA_DISABLE_OPTIMIZATION
// Sets default values
AMyClient::AMyClient()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyClient::BeginPlay()
{
	Super::BeginPlay();
	ConnectToPython();

}

void AMyClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ȷ���ͷ� Socket
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}

	// ���ø���� EndPlay
	Super::EndPlay(EndPlayReason);
}
// Called every frame
void AMyClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ReceiveData();
}

void AMyClient::TriggerBlueprintEvent()
{
	// ������ͼ�¼�
	OnDataReceived.Broadcast();
}

void AMyClient::ConnectToPython()
{
	FIPv4Address IP;
	FIPv4Address::Parse(TEXT("127.0.0.1"), IP);
	RemoteEndpoint = FIPv4Endpoint(IP, 5555);

	Socket = FUdpSocketBuilder(TEXT("PythonSocket"))
		.AsReusable()
		.WithBroadcast()
		.BoundToEndpoint(RemoteEndpoint)
		.WithReceiveBufferSize(2 * 1024 * 1024);

	int32 NewSize = 0;
	Socket->SetReceiveBufferSize(2 * 1024 * 1024, NewSize);

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("Socket creation failed"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Socket successfully created and bound to endpoint"));
	}
}


void AMyClient::ReceiveData()
{
	if (!Socket) return;

	TArray<uint8> ReceivedData;
	uint32 Size;
	while (Socket->HasPendingData(Size))
	{
		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));

		int32 Read = 0;
		Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		if (Read > 0)
		{
			const uint8* DataPtr = ReceivedData.GetData();
			const uint8* DataEnd = DataPtr + Read;

			IntegerParser IntParser;
			FloatParser FloatParser;
			StringParser StrParser;
			BooleanParser BoolParser;
			ByteArrayParser BytesParser;

			// ��������
			if (IntParser.Parse(DataPtr, DataEnd) &&
				FloatParser.Parse(DataPtr, DataEnd) &&
				StrParser.Parse(DataPtr, DataEnd) &&
				BoolParser.Parse(DataPtr, DataEnd) &&
				BytesParser.Parse(DataPtr, DataEnd)
				)
			{
				// ��ӡ���յ�������
				UE_LOG(LogTemp, Log, TEXT("Received data: %d, %f, %s, %s, %02x%02x%02x%02x"),
					IntParser.Value, FloatParser.Value, *StrParser.Value,
					BoolParser.Value ? TEXT("true") : TEXT("false"),
					BytesParser.ByteArray[0],
					BytesParser.ByteArray[1],
					BytesParser.ByteArray[2],
					BytesParser.ByteArray[3]);


				// ʾ������ÿ֡�����д����¼�
				TriggerBlueprintEvent();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse data"));
			}
		}
	}
}


PRAGMA_ENABLE_OPTIMIZATION
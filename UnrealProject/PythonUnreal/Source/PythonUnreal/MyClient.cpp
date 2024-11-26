// Fill out your copyright notice in the Description page of Project Settings.


#include "MyClient.h"

#include <winsock2.h>

#include "Common/UdpSocketBuilder.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
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

// Called every frame
void AMyClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ReceiveData();
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
}

float AMyClient::ntohf(uint32 netfloat)
{
	uint32 hostfloat = ntohl(netfloat);
	return *(float*)&hostfloat;
}

// void AMyClient::ReceiveData()
// {
// 	if (!Socket) return;
//
// 	TArray<uint8> ReceivedData;
// 	uint32 Size;
// 	while (Socket->HasPendingData(Size))
// 	{
// 		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
//
// 		int32 Read = 0;
// 		Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
//
// 		if (Read > 0)
// 		{
// 			// 确保接收到的数据大小正确
// 			if (ReceivedData.Num() == sizeof(float) * 3)
// 			{
// 				float X, Y, Z;
// 				FMemory::Memcpy(&X, ReceivedData.GetData(), sizeof(float));
// 				FMemory::Memcpy(&Y, ReceivedData.GetData() + sizeof(float), sizeof(float));
// 				FMemory::Memcpy(&Z, ReceivedData.GetData() + 2 * sizeof(float), sizeof(float));
//
// 				// 将网络字节序转换为主机字节序
// 				X = ntohf(*(uint32*)&X);
// 				Y = ntohf(*(uint32*)&Y);
// 				Z = ntohf(*(uint32*)&Z);
//
// 				FVector Position(X, Y, Z);
// 				UE_LOG(LogTemp, Log, TEXT("Received position: %s %f %f %f"), *Position.ToString(),X, Y, Z);
//
// 				// 移动角色
// 				SetActorLocation(Position);
// 			}
// 			else
// 			{
// 				UE_LOG(LogTemp, Error, TEXT("Received data size mismatch"));
// 			}
// 		}
// 	}
// }

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

            int32 Integer;
            float Floating;
            uint32 StringLength;
            bool Boolean;
            uint8 ByteArray[4];

            // 解析整数
            if (DataPtr + sizeof(int32) <= DataEnd)
            {
                FMemory::Memcpy(&Integer, DataPtr, sizeof(int32));
                Integer = ntohl(Integer); // 转换为主机字节序
                DataPtr += sizeof(int32);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse Integer"));
                continue;
            }

            // 解析浮点数
            if (DataPtr + sizeof(float) <= DataEnd)
            {
                uint32 NetFloat;
                FMemory::Memcpy(&NetFloat, DataPtr, sizeof(uint32));
                Floating = ntohf(NetFloat); // 转换为主机字节序
                DataPtr += sizeof(uint32);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse Floating"));
                continue;
            }

            // 解析字符串长度
            if (DataPtr + sizeof(uint32) <= DataEnd)
            {
                FMemory::Memcpy(&StringLength, DataPtr, sizeof(uint32));
                StringLength = ntohl(StringLength); // 转换为主机字节序
                DataPtr += sizeof(uint32);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse StringLength"));
                continue;
            }

            // 解析字符串
            FString String;
            if (StringLength > 0 && DataPtr + StringLength <= DataEnd)
            {
                // 使用 FUTF8ToTCHAR 将 UTF-8 编码的字节数组转换为 FString
                String = FString(FUTF8ToTCHAR((const ANSICHAR*)DataPtr, StringLength));
                DataPtr += StringLength;
            }
            else if (StringLength > 0)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse String"));
                continue;
            }

            // 解析布尔值
            if (DataPtr + sizeof(bool) <= DataEnd)
            {
                FMemory::Memcpy(&Boolean, DataPtr, sizeof(bool));
                DataPtr += sizeof(bool);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse Boolean"));
                continue;
            }

            // 解析字节数组
            if (DataPtr + sizeof(ByteArray) <= DataEnd)
            {
                FMemory::Memcpy(ByteArray, DataPtr, sizeof(ByteArray));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse ByteArray"));
                continue;
            }

            // 打印接收到的数据
            UE_LOG(LogTemp, Log, TEXT("Received data: %d, %f, %s, %s, %02x%02x%02x%02x"),
                Integer, Floating, *String, Boolean ? TEXT("true") : TEXT("false"),
                ByteArray[0], ByteArray[1], ByteArray[2], ByteArray[3]);
        }
    }
}

PRAGMA_ENABLE_OPTIMIZATION
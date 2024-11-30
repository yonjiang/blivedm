#pragma once

#include "CoreMinimal.h"
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
	uint8 ByteArray[4]; // 固定大小的字节数组

	virtual bool Parse(const uint8*& DataPtr, const uint8* DataEnd) override {
		// 检查是否有足够的数据来解析字节数组
		if (DataPtr + sizeof(ByteArray) <= DataEnd) {
			FMemory::Memcpy(ByteArray, DataPtr, sizeof(ByteArray)); // 复制数据
			DataPtr += sizeof(ByteArray); // 移动指针
			return true; // 返回成功
		} else {
			UE_LOG(LogTemp, Error, TEXT("Failed to parse ByteArray: Not enough data"));
			return false; // 返回失败
		}
	}
};

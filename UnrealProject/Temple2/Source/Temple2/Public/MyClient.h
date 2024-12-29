// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "GameFramework/Actor.h"
#include "MyClient.generated.h"

// ����һ����̬�ಥί��
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataReceived);

UCLASS()
class TEMPLE2_API AMyClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyClient();

	// BlueprintAssignable ί��
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDataReceived OnDataReceived;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �����¼��ĺ���
	void TriggerBlueprintEvent();

private:
	void ConnectToPython();
	void ReceiveData();

	FSocket* Socket;
	FIPv4Endpoint RemoteEndpoint;
};

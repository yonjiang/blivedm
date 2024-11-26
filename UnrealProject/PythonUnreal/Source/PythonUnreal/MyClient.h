// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "MyClient.generated.h"

UCLASS()
class PYTHONUNREAL_API AMyClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void ConnectToPython();
	float ntohf(uint32 netfloat);
	void ReceiveData();

	FSocket* Socket;
	FIPv4Endpoint RemoteEndpoint;

};

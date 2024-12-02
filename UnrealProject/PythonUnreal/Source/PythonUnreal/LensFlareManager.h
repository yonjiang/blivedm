// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LensFlareManager.generated.h"

UCLASS()
class PYTHONUNREAL_API ALensFlareManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALensFlareManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVisibility(bool bVisible);
	void CheckVisibility();
	bool IsObstructed(FVector Start, FVector End);

private:
	UPROPERTY(EditAnywhere)
	float VisibilityDuration = 1.0f;

	float CurrentVisibility;
	float TargetVisibility;
	float VisibilityChangeRate;

	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;

};

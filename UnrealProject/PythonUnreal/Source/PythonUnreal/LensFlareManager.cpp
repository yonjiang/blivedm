// Fill out your copyright notice in the Description page of Project Settings.


#include "LensFlareManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALensFlareManager::ALensFlareManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CurrentVisibility = 1.0f;
	TargetVisibility = 1.0f;
	VisibilityChangeRate = 1.0f / VisibilityDuration;
}

// Called when the game starts or when spawned
void ALensFlareManager::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // 获取第一个玩家控制器
}

// Called every frame
void ALensFlareManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckVisibility(); // 检查可见性

	// 更新当前可见性
	if (CurrentVisibility != TargetVisibility)
	{
		CurrentVisibility = FMath::FInterpTo(CurrentVisibility, TargetVisibility, DeltaTime, VisibilityChangeRate);
		// 在这里更新光斑的可见性（例如，更新材质的透明度）
	}
}

void ALensFlareManager::SetVisibility(bool bVisible)
{
	TargetVisibility = bVisible ? 1.0f : 0.0f;
}

void ALensFlareManager::CheckVisibility()
{
	if (PlayerController)
	{
		FVector PlayerLocation;
		FRotator PlayerRotation;
		PlayerController->GetPlayerViewPoint(PlayerLocation, PlayerRotation);

		FVector Direction = PlayerRotation.Vector();
		FVector ActorLocation = GetActorLocation();

		// 射线检测
		if (!IsObstructed(PlayerLocation, ActorLocation))
		{
			SetVisibility(true);
		}
		else
		{
			SetVisibility(false);
		}
	}
}

bool ALensFlareManager::IsObstructed(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 忽略自身

	// 执行射线检测
	return GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
}

// Copyright SkyJus Works. All Rights Reserved.


#include "StairsSandstorm.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WaterChild/Spirit.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "DrawDebugHelpers.h"

// Sets default values
AStairsSandstorm::AStairsSandstorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComponent"));
	Root->SetupAttachment(RootComponent);
	SandstormSizeVisualiser = CreateDefaultSubobject<UBoxComponent>(TEXT("SandstormSizeVisualiser"));
	SandstormSizeVisualiser->SetupAttachment(Root);
	PushBackDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("PushBackDirection"));
	PushBackDirection->SetupAttachment(SandstormSizeVisualiser);
	SandstormStartingParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormStartingParticleSystem"));
	SandstormStartingParticleSystem->SetupAttachment(PushBackDirection);
	SandstormActiveParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormActiveParticleSystem"));
	SandstormActiveParticleSystem->SetupAttachment(PushBackDirection);

	SafeSpot1 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot1"));
	SafeSpot1->SetupAttachment(Root);
	SafeSpot2 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot2"));
	SafeSpot2->SetupAttachment(Root);
	SafeSpot3 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot3"));
	SafeSpot3->SetupAttachment(Root);
	SafeSpot4 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot4"));
	SafeSpot4->SetupAttachment(Root);
	SafeSpot5 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot5"));
	SafeSpot5->SetupAttachment(Root);
	SafeSpot6 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot6"));
	SafeSpot6->SetupAttachment(Root);
	
	SandstormSizeVisualiser->SetBoxExtent(FVector::OneVector * 100.f);
	SandstormSizeVisualiser->SetGenerateOverlapEvents(false);
	SandstormSizeVisualiser->SetCollisionProfileName("NoCollision");

	SandstormStartingParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());
	SandstormStartingParticleSystem->SetAutoActivate(false);
	SandstormActiveParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());
	SandstormActiveParticleSystem->SetAutoActivate(false);

	SafeSpot1->SetGenerateOverlapEvents(false);
	SafeSpot1->SetCollisionProfileName("NoCollision");
	SafeSpot2->SetGenerateOverlapEvents(false);
	SafeSpot2->SetCollisionProfileName("NoCollision");
	SafeSpot3->SetGenerateOverlapEvents(false);
	SafeSpot3->SetCollisionProfileName("NoCollision");
	SafeSpot4->SetGenerateOverlapEvents(false);
	SafeSpot4->SetCollisionProfileName("NoCollision");
	SafeSpot5->SetGenerateOverlapEvents(false);
	SafeSpot5->SetCollisionProfileName("NoCollision");
	SafeSpot6->SetGenerateOverlapEvents(false);
	SafeSpot6->SetCollisionProfileName("NoCollision");
	
	PlayerObjectList.Add(EObjectTypeQuery::ObjectTypeQuery3);
	ObjectsToIgnoreList.Empty();
	
}

// Called every frame
void AStairsSandstorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoBoxTrace();
	if(IsHitPlayer && !SpiritReference)
		SpiritReference = Cast<ASpirit>(SandstormHitResult.Actor);

	if(IsHitPlayer && SpiritReference)
	{
		CalculateSandstormStrength();
	}

	
	/*if(!IsHitPlayer && SpiritReference)
	{
		if (PlayerSandstormWalkAnimVar > 0.f)
		{
			//PlayerSandstormWalkAnimVar -= DeltaTime;
			PlayerSandstormWalkAnimVar = 0.f;
		}
		else if(PlayerSandstormWalkAnimVar < 0.f)
		{
			PlayerSandstormWalkAnimVar = 0.f;
		}
		
		if(SpiritReference->GetCharacterMovement()->MaxWalkSpeed < PlayerWalkSpeed_Default)
		{
			SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default;
		}
	}

	// Cycle sandstorm
	switch (SandstormState)
	{
	case ESandstormState::Inactive:
		CycleSandstormState(SandstormInterval_Tick, SandstormInterval, DeltaTime, ESandstormState::Starting);

		if(SandstormActiveParticleSystem->IsActive())
			SandstormActiveParticleSystem->Deactivate();
		
		if(IsHitPlayer && SpiritReference)
		{
			if (PlayerSandstormWalkAnimVar > 0.f)
			{
				PlayerSandstormWalkAnimVar -= DeltaTime;
			}
			else if(PlayerSandstormWalkAnimVar < 0.f)
			{
				PlayerSandstormWalkAnimVar = 0.f;
			}
			
			if (SpiritReference->GetCharacterMovement()->MaxWalkSpeed < PlayerWalkSpeed_Default)
			{
				SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default;
			}
		}
		
		break;
	case ESandstormState::Starting:
		CycleSandstormState(SandstormStartUpTime_Tick, SandstormStartUpTime, DeltaTime, ESandstormState::Active);

		if(!SandstormStartingParticleSystem->IsActive())
			SandstormStartingParticleSystem->Activate();

		if(IsHitPlayer && SpiritReference && SandstormState == ESandstormState::Active)
		{
			PlayerSandstormWalkAnimVar = SandstormStartUpTime_Tick;
			SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default - (PlayerSandstormWalkAnimVar * PlayerWalkSpeed_Difference);
			UE_LOG(LogTemp, Warning, TEXT("SandstormStartUpTime_Tick: %f, MovementSpeed: %f"), PlayerSandstormWalkAnimVar, SpiritReference->GetCharacterMovement()->MaxWalkSpeed);
		}
			
		break;
	case ESandstormState::Active:
		CycleSandstormState(SandstormDuration_Tick, SandstormDuration, DeltaTime, ESandstormState::Inactive);
		
		if(SandstormStartingParticleSystem->IsActive())
			SandstormStartingParticleSystem->Deactivate();
		if(!SandstormActiveParticleSystem->IsActive())
			SandstormActiveParticleSystem->Activate();
		
		if(IsHitPlayer && SpiritReference)
		{
			if (PlayerSandstormWalkAnimVar < 1.f)
			{
				//PlayerSandstormWalkAnimVar = 1.f;
				PlayerSandstormWalkAnimVar += DeltaTime;
			}
			
			if(SpiritReference->GetCharacterMovement()->MaxWalkSpeed > PlayerWalkSpeed_Sandstorm)
			{
				//SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Sandstorm;
				SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default - (PlayerSandstormWalkAnimVar * PlayerWalkSpeed_Difference);
			}

			if(!CheckSafeSpots() && PlayerSandstormWalkAnimVar >= 1.f)
			{
				//UE_LOG(LogTemp, Warning, TEXT("PushBackDirection: %s"), *PushBackDirection->GetForwardVector().ToString());
				const FVector PushBackForce = PushBackDirection->GetForwardVector() * SpiritReference->GetCharacterMovement()->MaxWalkSpeed * DeltaTime * PushBackForceMultiplier;
				SpiritReference->AddActorWorldOffset(PushBackForce);
			}
		}
		
		break;
	}*/

	//UE_LOG(LogTemp, Warning, TEXT("PlayerSandstormWalkAnimVar: %f"), PlayerSandstormWalkAnimVar);
}

void AStairsSandstorm::CycleSandstormState(float &TickValue, float TargetValue, float IncrementRate,
	ESandstormState TargetSandstormState)
{
	if(TickValue < TargetValue)
	{
		TickValue += IncrementRate;
	}
	else
	{
		TickValue = 0.f;
		SandstormState = TargetSandstormState;
	}
}

bool AStairsSandstorm::DoBoxTrace()
{
	// Main sandstorm body
	IsHitPlayer = UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), GetActorLocation(), GetActorLocation(), SandstormSizeVisualiser->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::None, SandstormHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

	return IsHitPlayer;
}

bool AStairsSandstorm::CheckSafeSpots()
{
	// Safe spot 1 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot1->GetComponentLocation(), SafeSpot1->GetComponentLocation(), SafeSpot1->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;
	// Safe spot 2 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot2->GetComponentLocation(), SafeSpot2->GetComponentLocation(), SafeSpot2->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;
	// Safe spot 3 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot3->GetComponentLocation(), SafeSpot3->GetComponentLocation(), SafeSpot3->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;
	// Safe spot 4 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot4->GetComponentLocation(), SafeSpot4->GetComponentLocation(), SafeSpot4->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;
	// Safe spot 5 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot5->GetComponentLocation(), SafeSpot5->GetComponentLocation(), SafeSpot5->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;
	// Safe spot 6 check
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), SafeSpot6->GetComponentLocation(), SafeSpot6->GetComponentLocation(), SafeSpot6->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::ForOneFrame, SafeSpotHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f))
		return true;

	return false;
}

float AStairsSandstorm::CalculateSandstormStrength()
{
	float totalLength = SandstormSizeVisualiser->GetScaledBoxExtent().X;
	float startPoint = GetActorLocation().X - (totalLength / 2);
	float endPoint = GetActorLocation().X + (totalLength / 2);
	float playerDistanceFromEndPoint = endPoint - SpiritReference->GetActorLocation().X;
	float travelledRatio = playerDistanceFromEndPoint / totalLength;

	//UE_LOG(LogTemp, Warning, TEXT("travelled Ratio: %f"), travelledRatio);
	UE_LOG(LogTemp, Warning, TEXT("playerDistanceFromEndPoint: %f"), playerDistanceFromEndPoint);
	return travelledRatio;
}

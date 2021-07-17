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
	PushBackDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("PushBackDirection"));
	PushBackDirection->SetupAttachment(Root);
	SandstormSizeVisualiser = CreateDefaultSubobject<UBoxComponent>(TEXT("SandstormSizeVisualiser"));
	SandstormSizeVisualiser->SetupAttachment(Root);
	SandstormStartingParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormStartingParticleSystem"));
	SandstormStartingParticleSystem->SetupAttachment(Root);
	SandstormActiveParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormActiveParticleSystem"));
	SandstormActiveParticleSystem->SetupAttachment(Root);
	
	SandstormSizeVisualiser->SetBoxExtent(FVector::OneVector * 100.f);
	SandstormSizeVisualiser->SetGenerateOverlapEvents(false);
	SandstormSizeVisualiser->SetCollisionProfileName("NoCollision");

	SandstormStartingParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());
	SandstormStartingParticleSystem->SetAutoActivate(false);
	SandstormActiveParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());
	SandstormActiveParticleSystem->SetAutoActivate(false);
	
	PlayerObjectList.Add(EObjectTypeQuery::ObjectTypeQuery3);
	ObjectsToIgnoreList.Empty();
	
}

// Called every frame
void AStairsSandstorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoBoxTrace();
	if(IsHitPlayer && !SpiritReference)
		SpiritReference = Cast<ASpirit>(HitResult.Actor);

	if(!IsHitPlayer && SpiritReference)
	{
		if (PlayerSandstormWalkAnimVar > 0.f)
		{
			PlayerSandstormWalkAnimVar -= DeltaTime;
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
		
		if(IsHitPlayer)
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

		if(IsHitPlayer)
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
		
		if(IsHitPlayer)
		{
			if (PlayerSandstormWalkAnimVar < 1.f)
			{
				PlayerSandstormWalkAnimVar = 1.f;
			}
			
			if(SpiritReference->GetCharacterMovement()->MaxWalkSpeed > PlayerWalkSpeed_Sandstorm)
			{
				SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Sandstorm;
			}

			UE_LOG(LogTemp, Warning, TEXT("PushBackDirection: %s"), *PushBackDirection->GetForwardVector().ToString());
			const FVector PushBackForce = PushBackDirection->GetForwardVector() * SpiritReference->GetCharacterMovement()->MaxWalkSpeed * DeltaTime * 3.f;
			SpiritReference->AddActorWorldOffset(PushBackForce);
		}
		
		break;
	}

	
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
	IsHitPlayer = UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), GetActorLocation(), GetActorLocation(), SandstormSizeVisualiser->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::Persistent, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);
	//DrawDebugBox(GetWorld(), GetActorLocation(), SandstormSizeVisualiser->GetScaledBoxExtent(), FColor::Green, false, -1, 0, 3);
	
	return IsHitPlayer; 
}


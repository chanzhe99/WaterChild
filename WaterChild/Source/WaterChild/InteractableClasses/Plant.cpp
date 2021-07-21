// Copyright SkyJus Works. All Rights Reserved.


#include "Plant.h"

APlant::APlant()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APlant::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LastWaterValue = CurrentWaterValue;

	if(LastWaterValue == CurrentWaterValue && IsReviveEffectsActive)
	{
		if(ReviveEndTime_Tick < ReviveEndTime)
		{
			ReviveEndTime_Tick += DeltaTime;
		}
		else
		{
			ReviveEndTime_Tick = 0.f;
			IsReviveEffectsActive = false;
			ActivateRevivingEffects(false);
		}
	}
}

void APlant::OnInteract_Implementation(ASpirit* Caller)
{
	if(!IsPlantAlive)
	{
		if(CurrentWaterValue < MaxWaterValue)
		{
			CurrentWaterValue += GetWorld()->GetDeltaSeconds() * WaterIncreaseRate;
			CurrentPreGrowAnimTime = (CurrentWaterValue / MaxWaterValue) * PreGrowAnimLength;
			
			ReviveEndTime_Tick = 0.f;
			if(!IsReviveEffectsActive)
			{
				IsReviveEffectsActive = true;
				ActivateRevivingEffects();
			}
		}
		else
		{
			IsPlantAlive = true;
		}
	}
}

// Copyright SkyJus Works. All Rights Reserved.


#include "Grass.h"
#include "Components/BoxComponent.h"

AGrass::AGrass()
{
	PrimaryActorTick.bCanEverTick = false;

	// Component creation
	GrassMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GrassMesh"));
	MultiCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MultiCollider"));

	// Attachment setup
	SetRootComponent(GrassMesh);
	MultiCollider->SetupAttachment(GrassMesh);

	// Location setup
	MultiCollider->SetRelativeLocation(FVector(0, 0, 0));

	MultiCollider->SetBoxExtent(FVector(50, 50, 50));

	MultiCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MultiCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGrass::BeginPlay()
{
	Super::BeginPlay();

	if (PreGrowAnim)
	{
		PreGrowAnimLength = PreGrowAnim->SequenceLength;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s - PreGrowAnim not found!"), *GetNameSafe(this));
	}
}

/*
void AGrass::OnInteract_Implementation(ASpirit* Caller)
{
	// State changer for dead to growing
	switch (PlantState)
	{
	case EPlantState::Dead:
		UE_LOG(LogTemp, Warning, TEXT("Grass is now growing"));
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;

	case EPlantState::Growing:
		CurrentWaterValue += GetWorld()->GetDeltaSeconds() * WaterIncreaseRate;
		UE_LOG(LogTemp, Warning, TEXT("Grass gaining water"));

		// WaterValue incrementer
		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			// Collision changer (Maybe not needed?)
			// Just need to change a boolean so that it can tell the grass when to react with player's movements
			//GrassCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			//GrassCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			//GrassCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			PlayPlantAnimation();
		}
		break;
	}
}

void AGrass::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("End call"));
	if (PlantState == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}
}
*/
// Copyright SkyJus Works. All Rights Reserved.


#include "BridgePlant.h"
#include "Components/BoxComponent.h"

ABridgePlant::ABridgePlant()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BridgeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BridgeMesh"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	Root->SetupAttachment(RootComponent);
	BridgeMesh->SetupAttachment(Root);
	ReviveCollider->SetupAttachment(Root);

	BridgeMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ABridgePlant::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABridgePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABridgePlant::OnInteract_Implementation(ASpirit* Caller)
{
	// State changer for dead to growing
	//UE_LOG(LogTemp, Warning, TEXT("Plant is now growing"));
	switch (PlantState)
	{
	case EPlantState::Dead:
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;

	case EPlantState::Growing:
		CurrentWaterValue += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning, TEXT("BridgePlant gaining water"));

		// WaterValue incrementer
		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			PlayPlantAnimation();
		}
		break;
	}
}

void ABridgePlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
	if (PlantState == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}
}
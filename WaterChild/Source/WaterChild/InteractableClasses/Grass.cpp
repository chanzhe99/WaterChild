// Copyright SkyJus Works. All Rights Reserved.


#include "Grass.h"
#include "Components/BoxComponent.h"

AGrass::AGrass()
{
	PrimaryActorTick.bCanEverTick = false;

	// Component creation
	GrassMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GrassMesh"));
	GrassCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("GrassCollider"));
	MultiCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MultiCollider"));

	// Attachment setup
	GrassMesh->SetupAttachment(RootComponent);
	GrassCollider->SetupAttachment(GrassMesh);
	MultiCollider->SetupAttachment(GrassMesh);

	// Location setup
	GrassCollider->SetRelativeLocation(FVector(0, 0, 0));
	MultiCollider->SetRelativeLocation(FVector(0, 0, 0));

	GrassCollider->SetBoxExtent(FVector(50, 50, 25));
	MultiCollider->SetBoxExtent(FVector(50, 50, 50));

	// Set dirt patch collider so player can walk on it
	GrassCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	GrassCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GrassCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	MultiCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MultiCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGrass::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGrass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

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
		CurrentWaterValue += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning, TEXT("Grass gaining water"));

		// WaterValue incrementer
		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			// Collision changer (Maybe not needed?)
			// Just need to change a boolean so that it can tell the grass when to react
			// with player's movements
			/*GrassCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			GrassCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			GrassCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);*/

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
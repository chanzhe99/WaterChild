// Copyright SkyJus Works. All Rights Reserved.


#include "InteractablePlant.h"


// Sets default values
AInteractablePlant::AInteractablePlant()
{
	PrimaryActorTick.bCanEverTick = true;

	//SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	//SceneComponent->SetupAttachment(RootComponent);

	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	FloorPlane->SetupAttachment(RootComponent);

	StemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StemMesh"));
	StemMesh->SetupAttachment(FloorPlane);

	PetalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh"));
	PetalMesh->SetupAttachment(StemMesh);

}

// Called when the game starts or when spawned
void AInteractablePlant::BeginPlay()
{
	Super::BeginPlay();
	RevivedPlantHeight = StemMesh->GetRelativeLocation().Z;
	StemMesh->SetRelativeLocation(FVector(0, 0, DefaultPlantHeight));

	UE_LOG(LogTemp, Warning, TEXT("%f"), RevivedPlantHeight)
}

// Called every frame
void AInteractablePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (PlantState)
	{
	case EPlantState::Dead:
		if (StemMesh->GetRelativeLocation().Z > DefaultPlantHeight)
			StemMesh->AddLocalOffset(FVector(0, 0, -ReviveSpeed * GetWorld()->GetDeltaSeconds()));
		break;
	case EPlantState::Reviving:
		if (StemMesh->GetRelativeLocation().Z < RevivedPlantHeight)
			StemMesh->AddLocalOffset(FVector(0, 0, ReviveSpeed * GetWorld()->GetDeltaSeconds()));
		else SetPlantState(EPlantState::Revived);
	}
}

void AInteractablePlant::OnInteract_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant is being revived"));
	switch (PlantState)
	{
	case EPlantState::Dead:
		SetPlantState(EPlantState::Reviving);
		break;
	}
}

void AInteractablePlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant stopped reviving"));
	switch (PlantState)
	{
	case EPlantState::Reviving:
		SetPlantState(EPlantState::Dead);
		break;
	}
}

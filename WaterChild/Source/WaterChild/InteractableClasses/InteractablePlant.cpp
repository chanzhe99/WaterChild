// Copyright SkyJus Works. All Rights Reserved.


#include "InteractablePlant.h"
#include "Components/BoxComponent.h"

// Sets default values
AInteractablePlant::AInteractablePlant()
{
	PrimaryActorTick.bCanEverTick = true;

	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	StemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StemMesh"));
	PetalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh"));
	PetalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("PetalCollider"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	SetRootComponent(FloorPlane);
	StemMesh->SetupAttachment(FloorPlane);
	PetalMesh->SetupAttachment(StemMesh);
	PetalCollider->SetupAttachment(PetalMesh);
	ReviveCollider->SetupAttachment(StemMesh);

	StemMesh->SetRelativeLocation(FVector(0, 0, DefaultPlantHeight));
	PetalCollider->SetRelativeLocation(FVector(0, 0, 382));
	ReviveCollider->SetRelativeLocation(FVector(0, 0, 50));

	PetalCollider->SetBoxExtent(FVector(50, 50, 0));
	ReviveCollider->SetBoxExtent(FVector(50, 50, 50));

	PetalCollider->SetCollisionProfileName(TEXT("NoCollision"));
	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
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

	//UE_LOG(LogTemp, Warning, TEXT("Plant Updating"))

	switch (PlantState)
	{
	case EOldPlantState::Dead:
		if (StemMesh->GetRelativeLocation().Z > DefaultPlantHeight)
			StemMesh->AddLocalOffset(FVector(0, 0, -ReviveSpeed * GetWorld()->GetDeltaSeconds()));
		break;
	case EOldPlantState::Reviving:
		if (StemMesh->GetRelativeLocation().Z < RevivedPlantHeight)
			StemMesh->AddLocalOffset(FVector(0, 0, ReviveSpeed * GetWorld()->GetDeltaSeconds()));
		else SetPlantState(EOldPlantState::Revived);
		break;
	case EOldPlantState::Revived:
		PetalCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		PetalCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		PetalCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		SetActorTickEnabled(false);
		break;
	}
}

void AInteractablePlant::OnInteract_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant is being revived"));
	switch (PlantState)
	{
	case EOldPlantState::Dead:
		SetPlantState(EOldPlantState::Reviving);
		break;
	}
}

void AInteractablePlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant stopped reviving"));
	switch (PlantState)
	{
	case EOldPlantState::Reviving:
		SetPlantState(EOldPlantState::Dead);
		break;
	}
}

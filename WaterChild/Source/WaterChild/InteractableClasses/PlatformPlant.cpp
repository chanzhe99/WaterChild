// Copyright SkyJus Works. All Rights Reserved.


#include "PlatformPlant.h"
#include "Components/BoxComponent.h"

// Sets default values
APlatformPlant::APlatformPlant()
{
	PrimaryActorTick.bCanEverTick = false;

	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	StemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StemMesh"));
	PetalMesh1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh1"));
	PetalMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh2"));
	PetalMesh3 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh3"));
	PetalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("PetalCollider"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	FloorPlane->SetupAttachment(RootComponent);
	StemMesh->SetupAttachment(FloorPlane);
	PetalMesh1->SetupAttachment(StemMesh);
	PetalMesh2->SetupAttachment(PetalMesh1);
	PetalMesh3->SetupAttachment(PetalMesh2);
	PetalCollider->SetupAttachment(PetalMesh1);
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
void APlatformPlant::BeginPlay()
{
	Super::BeginPlay();

	GrownPlantHeight = StemMesh->GetRelativeLocation().Z;
	StemMesh->SetRelativeLocation(FVector(0, 0, DefaultPlantHeight));
}

// Called every frame
void APlatformPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlantState == EPlantState::Alive && !bGrownUp)
	{
		if (StemMesh->GetRelativeLocation().Z < GrownPlantHeight)
		{
			StemMesh->AddLocalOffset(FVector(0, 0, GrowSpeed * GetWorld()->GetDeltaSeconds()));
		}
		else
		{
			bGrownUp = true;
			SetActorTickEnabled(false);
		}
	}
}

void APlatformPlant::OnInteract_Implementation(ASpirit* Caller)
{
	// State changer for dead to growing
	UE_LOG(LogTemp, Warning, TEXT("Plant is now growing"));
	switch (PlantState)
	{
	case EPlantState::Dead:
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;
	}

	// WaterValue incrementer
	if (CurrentWaterValue >= MaxWaterValue)
	{
		PlantState = EPlantState::Alive;

		// Collision changer
		PetalCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		PetalCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		PetalCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

		PlayPlantAnimation();
	}
	UE_LOG(LogTemp, Warning, TEXT("PlatformPlant gaining water"));
	CurrentWaterValue += GetWorld()->GetDeltaSeconds();
}

void APlatformPlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
	if (PlantState == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}
}
// Copyright SkyJus Works. All Rights Reserved.


#include "PlatformPlant.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
	PetalMesh1->SetupAttachment(StemMesh, "Bone008");
	PetalMesh2->SetupAttachment(PetalMesh1);
	PetalMesh3->SetupAttachment(PetalMesh2);
	PetalCollider->SetupAttachment(PetalMesh1);
	ReviveCollider->SetupAttachment(FloorPlane);

	PetalMesh1->SetRelativeRotation(FRotator(0, 270, 0));
	PetalMesh2->SetRelativeLocation(FVector(0, 0, 2.5f));
	PetalMesh2->SetRelativeScale3D(FVector(0.75f, 0.75f, 1));
	PetalMesh3->SetRelativeLocation(FVector(0, 0, 2.5f));
	PetalMesh3->SetRelativeScale3D(FVector(0.5f, 0.5f, 1));
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

	if (GetWorld()->GetName() != "Level_New")
	{
		GrownPlantSize = StemMesh->GetRelativeScale3D();
		StemMesh->SetRelativeScale3D(DeadPlantSize);
	}
}

// Called every frame
void APlatformPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlantState == EPlantState::Alive)
	{
		StemMesh->SetRelativeScale3D(FMath::Lerp(DeadPlantSize, GrownPlantSize, GrowTime / GrowDuration));
		GrowTime += GetWorld()->GetDeltaSeconds();
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
	case EPlantState::Growing:
		// WaterValue incrementer
		CurrentWaterValue += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning, TEXT("PlatformPlant gaining water"));

		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			// Collision changer
			PetalCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			PetalCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			PetalCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			PlayPlantAnimation();
		}
		break;
	}
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
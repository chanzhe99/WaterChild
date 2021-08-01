// Copyright SkyJus Works. All Rights Reserved.


#include "PlatformPlant.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlatformPlant::APlatformPlant()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	StemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StemMesh"));
	PetalMesh1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh1"));
	PetalMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh2"));
	PetalMesh3 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh3"));
	PetalCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("PetalCollider"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));
	WeedsComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeedsComp"));

	SetRootComponent(RootComp);
	StemMesh->SetupAttachment(RootComp);
	PetalMesh1->SetupAttachment(StemMesh, "Bone008");
	PetalMesh2->SetupAttachment(PetalMesh1);
	PetalMesh3->SetupAttachment(PetalMesh2);
	PetalCollider->SetupAttachment(PetalMesh1);
	ReviveCollider->SetupAttachment(RootComp);
	WeedsComp->SetupAttachment(RootComp);

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

	// if (PlantState == EPlantState::Alive)
	if (IsPlantAlive)
	{
		StemMesh->SetRelativeScale3D(FMath::Lerp(DeadPlantSize, GrownPlantSize, GrowTime / GrowDuration));
		GrowTime += GetWorld()->GetDeltaSeconds();
	}
}
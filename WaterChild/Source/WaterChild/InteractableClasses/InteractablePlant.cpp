// Copyright SkyJus Works. All Rights Reserved.


#include "InteractablePlant.h"


// Sets default values
AInteractablePlant::AInteractablePlant()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponent->SetupAttachment(RootComponent);

	StemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StemMesh"));
	StemMesh->SetupAttachment(SceneComponent);

	PetalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PetalMesh"));
	PetalMesh->SetupAttachment(StemMesh);

	StemMesh->SetRelativeLocation(FVector(0, 0, -300));
	//PetalMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
}

// Called when the game starts or when spawned
void AInteractablePlant::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractablePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractablePlant::OnInteract_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant should be grown"));
	bPlantIsGrown = true;
}

void AInteractablePlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Plant should not be grown"));
	bPlantIsGrown = false;
}

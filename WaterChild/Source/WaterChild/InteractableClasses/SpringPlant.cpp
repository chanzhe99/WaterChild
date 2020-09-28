// Copyright SkyJus Works. All Rights Reserved.


#include "SpringPlant.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpringPlant::ASpringPlant()
{
	PrimaryActorTick.bCanEverTick = false;

	// Component creation
	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	FlowerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlowerMesh"));
	FlowerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FlowerCollider"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	// Attachment setup
	FloorPlane->SetupAttachment(RootComponent);
	FlowerMesh->SetupAttachment(FloorPlane);
	FlowerCollider->SetupAttachment(FlowerMesh);
	ReviveCollider->SetupAttachment(FloorPlane);

	// Location setup
	FlowerCollider->SetRelativeLocation(FVector(0, 5, 50));
	ReviveCollider->SetRelativeLocation(FVector(0, 5, 25));

	FlowerCollider->SetBoxExtent(FVector(84, 76, 0));
	ReviveCollider->SetBoxExtent(FVector(84, 76, 25));

	FlowerCollider->SetCollisionProfileName(TEXT("NoCollision"));
	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ASpringPlant::BeginPlay()
{
	Super::BeginPlay();

	//TODO remove once the grow feature is done
	PlantStatus = EPlantState::Alive;

}

// Called every frame
void ASpringPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpringPlant::OnInteract_Implementation(ASpirit* Caller)
{
	switch (PlantStatus)
	{
	case EPlantState::Dead:
		UE_LOG(LogTemp, Warning, TEXT("Plant is now growing"));
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;
	}

	// Water value ticker
	if (CurrentWaterValue >= MaxWaterValue)
	{
		PlantStatus = EPlantState::Alive;

		// Collision changer
		FlowerCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		FlowerCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		FlowerCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		SetActorTickEnabled(false);
	}
	else
	{
		CurrentWaterValue += GetWorld()->GetDeltaSeconds();
	}
}

void ASpringPlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("End call"));

	if (PlantStatus == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}

	// Boost player
	switch (PlantStatus)
	{
	case EPlantState::Alive:
		Caller->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		//UE_LOG(LogTemp, Warning, TEXT("Boost"));
		Caller->LaunchCharacter(FVector(BounceForwardVelocity, BounceSideVelocity, BounceUpVelocity), false, false);
	}
}
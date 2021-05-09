// Copyright SkyJus Works. All Rights Reserved.


#include "SpringPlant.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpringPlant::ASpringPlant()
{
	PrimaryActorTick.bCanEverTick = false;

	// Component creation
	FlowerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlowerMesh"));
	FlowerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("FlowerCollider"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	// Attachment setup
	SetRootComponent(FlowerMesh);
	FlowerCollider->SetupAttachment(FlowerMesh);
	ReviveCollider->SetupAttachment(FlowerMesh);

	// Location setup
	FlowerCollider->SetRelativeLocation(FVector(-5, -3, 25));
	ReviveCollider->SetRelativeLocation(FVector(-5, -3, 25));

	FlowerCollider->SetBoxExtent(FVector(80, 80, 0));
	ReviveCollider->SetBoxExtent(FVector(80, 80, 25));

	FlowerCollider->SetCollisionProfileName(TEXT("NoCollision"));
	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ASpringPlant::BeginPlay()
{
	Super::BeginPlay();

	//TODO remove once the grow feature is done
	//PlantState = EPlantState::Alive;
}

// Called every frame
void ASpringPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpringPlant::OnInteract_Implementation(ASpirit* Caller)
{
	// State changer for dead to growing
	switch (PlantState)
	{
	case EPlantState::Dead:
		UE_LOG(LogTemp, Warning, TEXT("SpringPlant is now growing"));
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;

	case EPlantState::Growing:
		CurrentWaterValue += GetWorld()->GetDeltaSeconds() * WaterIncreaseRate;
		UE_LOG(LogTemp, Warning, TEXT("SpringPlant gaining water"));

		// WaterValue incrementer
		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			// Collision changer
			FlowerCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
			FlowerCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			FlowerCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			PlayPlantAnimation();
		}
		break;
	}	
}

void ASpringPlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("End call"));
	if (PlantState == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}
}

void ASpringPlant::BounceAnimation_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Boost"));
	// Boost player
	switch (PlantState)
	{
	case EPlantState::Alive:
		Caller->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		Caller->LaunchCharacter(FVector(BounceForwardVelocity, BounceSideVelocity, BounceUpVelocity), false, false);
	}
}
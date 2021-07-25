// Copyright SkyJus Works. All Rights Reserved.


#include "SpringPlant.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpringPlant::ASpringPlant()
{
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

	if (PreGrowAnim)
	{
		PreGrowAnimLength = PreGrowAnim->SequenceLength;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s - PreGrowAnim not found!"), *GetNameSafe(this));
	}
}

void ASpringPlant::BounceAnimation_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Boost"));
	// Boost player
	Caller->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	Caller->LaunchCharacter(FVector(BounceForwardVelocity, BounceSideVelocity, BounceUpVelocity), false, false);
}
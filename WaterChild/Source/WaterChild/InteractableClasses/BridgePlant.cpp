// Copyright SkyJus Works. All Rights Reserved.


#include "BridgePlant.h"
#include "Components/BoxComponent.h"

ABridgePlant::ABridgePlant()
{
	PrimaryActorTick.bCanEverTick = false;

	FloorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorPlane"));
	BridgeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BridgeMesh"));
	BridgeCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider1"));
	BridgeCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider2"));
	BridgeCollider3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider3"));
	BridgeCollider4 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider4"));
	BridgeCollider5 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider5"));
	BridgeCollider6 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider6"));
	BridgeCollider7 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider7"));
	BridgeCollider8 = CreateDefaultSubobject<UBoxComponent>(TEXT("BridgeCollider8"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));

	FloorPlane->SetupAttachment(RootComponent);
	BridgeMesh->SetupAttachment(FloorPlane);
	BridgeCollider1->SetupAttachment(BridgeMesh);
	BridgeCollider2->SetupAttachment(BridgeMesh);
	BridgeCollider3->SetupAttachment(BridgeMesh);
	BridgeCollider4->SetupAttachment(BridgeMesh);
	BridgeCollider5->SetupAttachment(BridgeMesh);
	BridgeCollider6->SetupAttachment(BridgeMesh);
	BridgeCollider7->SetupAttachment(BridgeMesh);
	BridgeCollider8->SetupAttachment(BridgeMesh);
	ReviveCollider->SetupAttachment(FloorPlane);

	BridgeCollider1->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider2->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider3->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider4->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider5->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider6->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider7->SetCollisionProfileName(TEXT("NoCollision"));
	BridgeCollider8->SetCollisionProfileName(TEXT("NoCollision"));
	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ABridgePlant::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void ABridgePlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABridgePlant::OnInteract_Implementation(ASpirit* Caller)
{
	// State changer for dead to growing
	//UE_LOG(LogTemp, Warning, TEXT("Plant is now growing"));
	switch (PlantState)
	{
	case EPlantState::Dead:
		SetPlantState(EPlantState::Growing);
		PrimaryActorTick.bCanEverTick = true;
		break;

	case EPlantState::Growing:
		CurrentWaterValue += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning, TEXT("BridgePlant gaining water"));

		// WaterValue incrementer
		if (CurrentWaterValue >= MaxWaterValue)
		{
			PlantState = EPlantState::Alive;

			// Collision changer
			BridgeCollider1->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider1->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider1->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider2->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider2->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider2->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider3->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider3->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider3->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider4->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider4->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider4->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider5->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider5->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider5->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider6->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider6->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider6->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider7->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider7->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider7->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			BridgeCollider8->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			BridgeCollider8->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			BridgeCollider8->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			PlayPlantAnimation();
		}
		break;
	}

	
}

void ABridgePlant::OnInteractEnd_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("Plant stopped growing"));
	if (PlantState == EPlantState::Growing && CurrentWaterValue <= 0)
	{
		SetPlantState(EPlantState::Dead);
		PrimaryActorTick.bCanEverTick = false;
	}
}
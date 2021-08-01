// Copyright SkyJus Works. All Rights Reserved.


#include "BridgePlant.h"
#include "Components/BoxComponent.h"

ABridgePlant::ABridgePlant()
{
	// Component creation
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BridgeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BridgeMesh"));
	ReviveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ReviveCollider"));
	WeedsComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeedsComp"));

	// Attachment setup
	SetRootComponent(Root);
	BridgeMesh->SetupAttachment(Root);
	ReviveCollider->SetupAttachment(Root);
	WeedsComp->SetupAttachment(Root);

	BridgeMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ReviveCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	ReviveCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ABridgePlant::BeginPlay()
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
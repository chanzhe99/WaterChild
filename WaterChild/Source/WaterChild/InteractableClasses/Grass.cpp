// Copyright SkyJus Works. All Rights Reserved.


#include "Grass.h"
#include "Components/BoxComponent.h"

AGrass::AGrass()
{
	// Component creation
	GrassMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GrassMesh"));
	MultiCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MultiCollider"));

	// Attachment setup
	SetRootComponent(GrassMesh);
	MultiCollider->SetupAttachment(GrassMesh);

	// Location setup
	MultiCollider->SetRelativeLocation(FVector(0, 0, 0));

	MultiCollider->SetBoxExtent(FVector(50, 50, 50));

	MultiCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	MultiCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGrass::BeginPlay()
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractablePlate.h"


// Sets default values
AInteractablePlate::AInteractablePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	SetRootComponent(FrameMesh);

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	PlateMesh->SetupAttachment(FrameMesh);
}

// Called when the game starts or when spawned
void AInteractablePlate::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractablePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractablePlate::OnInteract_Implementation(ASpirit* Caller)
{
	bIsSpiritOnPlate = true;
}

void AInteractablePlate::OnInteractEnd_Implementation(ASpirit* Caller)
{
	bIsSpiritOnPlate = false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableDebris.h"


// Sets default values
AInteractableDebris::AInteractableDebris()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DebrisMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebrisMesh"));
	DebrisMesh->SetupAttachment(RootComponent);

	DebrisMesh->SetCollisionObjectType(ECC_Destructible);
	DebrisMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	DebrisMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void AInteractableDebris::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AInteractableDebris::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableDebris::OnInteract_Implementation(ASpirit* Caller)
{
	//Destroy();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"

// Sets default values
AInteractable::AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractable::OnInteract_Implementation(ASpirit* Caller)
{
	ObjectName = StaticClass()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *ObjectName);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, *ObjectName);
	Destroy();
}

void AInteractable::OnInteractEnd_Implementation(ASpirit* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("Stopped hitting %s"), *ObjectName);
}

void AInteractable::StartFocus_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, TEXT("Start Focus"));
}

void AInteractable::EndFocus_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("End Focus"));
}


// Copyright SkyJus Works. All Rights Reserved.


#include "Plant.h"

APlant::APlant()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APlant::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
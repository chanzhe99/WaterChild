// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableCrack.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AInteractableCrack::AInteractableCrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SceneComponent->SetupAttachment(RootComponent);

	CrackMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrackMesh1"));
	CrackMesh1->SetupAttachment(SceneComponent);

	CrackMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrackMesh2"));
	CrackMesh2->SetupAttachment(SceneComponent);

	//CrackCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("CrackCollider1"));
	//CrackCollider1->SetupAttachment(CrackMesh1);
	//
	//CrackCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("CrackCollider2"));
	//CrackCollider2->SetupAttachment(CrackMesh2);

	CrackExit1 = CreateDefaultSubobject<USceneComponent>(TEXT("CrackExit1"));
	CrackExit1->SetupAttachment(CrackMesh1);

	CrackExit2 = CreateDefaultSubobject<USceneComponent>(TEXT("CrackExit2"));
	CrackExit2->SetupAttachment(CrackMesh2);

	CrackMesh2->SetRelativeLocation(FVector(0, -200, 0));
	CrackMesh2->SetRelativeRotation(FRotator(0, 180, 0));

	//CrackCollider1->SetRelativeLocation(FVector(0, 0, 100));
	//CrackCollider2->SetRelativeLocation(FVector(0, 0, 100));
	//
	//CrackCollider1->SetRelativeScale3D(FVector(1.5, 0.25, 3));
	//CrackCollider2->SetRelativeScale3D(FVector(1.5, 0.25, 3));

	CrackExit1->SetRelativeLocation(FVector(0, 50, 0));
	CrackExit2->SetRelativeLocation(FVector(0, 50, 0));
}

// Called when the game starts or when spawned
void AInteractableCrack::BeginPlay()
{
	Super::BeginPlay();

	Crack1 = &CrackMesh1->GetComponentTransform();
	Crack2 = &CrackMesh2->GetComponentTransform();
	Crack1Location = CrackMesh1->GetComponentTransform().GetLocation();
	Crack2Location = CrackMesh2->GetComponentTransform().GetLocation();
}

void AInteractableCrack::TraverseCrack(ASpirit* Caller, const FTransform* Entrance, const FTransform* Exit)
{
	FVector InitialCallerScale = FVector(1, 1, 1);
	FVector CurrentCallerScale = Caller->GetActorScale();

	if (CurrentCallerScale.X > InitialCallerScale.X / 2)
	{
		Caller->SetActorLocation(FMath::Lerp(Caller->GetActorLocation(), Entrance->GetLocation(), 0.05f));
		Caller->SetActorScale3D(FMath::Lerp(Caller->GetActorScale(), Caller->GetActorScale() / 2, 0.05f));
	}
	else if (CurrentCallerScale.X <= InitialCallerScale.X / 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Exit Crack"));
		// Sets position and rotation to the same as exit
		Caller->GetCapsuleComponent()->SetWorldLocationAndRotation(Exit->GetLocation(), Exit->GetRotation());

		// Adds offset rotation so that the player doesn't automatically trigger again
		Caller->AddActorWorldRotation(FRotator(0, 90, 0).Quaternion());
		Caller->SetActorScale3D(InitialCallerScale);
		Caller->SetState(Caller->ESpiritState::Walking);
	}
}

// Called every frame
void AInteractableCrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AInteractableCrack::OnInteract_Implementation(ASpirit* Caller)
{
	//UE_LOG(LogTemp, Warning, TEXT("CrackEntrance: %s"), Caller->GetCrackEntrance() ? TEXT("True") : TEXT("False"));
	if (Caller->GetCrackEntrance())
		TraverseCrack(Caller, Crack1, &CrackExit2->GetComponentTransform());
	else
		TraverseCrack(Caller, Crack2, &CrackExit1->GetComponentTransform());

}

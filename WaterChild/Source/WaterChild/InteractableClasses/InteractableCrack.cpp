// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableCrack.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AInteractableCrack::AInteractableCrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

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

	CrackMesh1->SetCollisionObjectType(ECC_Destructible);
	CrackMesh1->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CrackMesh1->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	CrackMesh2->SetCollisionObjectType(ECC_Destructible);
	CrackMesh2->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CrackMesh2->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void AInteractableCrack::BeginPlay()
{
	Super::BeginPlay();

	Crack1 = &CrackMesh1->GetComponentTransform();
	Crack2 = &CrackMesh2->GetComponentTransform();
}

void AInteractableCrack::TraverseCrack(ASpirit* Caller, const FTransform* Entrance, const FTransform* Exit)
{
	//FVector InitialCallerScale = FVector(1, 1, 1);
	//FVector CurrentCallerScale = Caller->GetActorScale();
	//FVector CorrectedEntranceLocation = FVector(Entrance->GetLocation().X, Entrance->GetLocation().Y, Entrance->GetLocation().Z + Caller->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	//FVector CorrectedExitLocation = FVector(Exit->GetLocation().X, Exit->GetLocation().Y, Exit->GetLocation().Z + Caller->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	//if (!Caller->GetActorLocation().Equals(CorrectedExitLocation, 1))
	//{
	//	if (CurrentCallerScale.X > InitialCallerScale.X / 2)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Entering Crack"));
	//		//Caller->SetActorLocationAndRotation(FMath::Lerp(Caller->GetActorLocation(), CorrectedEntranceLocation, 0.05f), FRotator(0, Exit->GetRotation().Rotator().Yaw, 0).Quaternion());
	//		Caller->SetActorRotation(FRotator(0, Exit->GetRotation().Rotator().Yaw, 0).Quaternion());
	//		Caller->AddActorWorldRotation(FRotator(0, 90, 0));
	//		Caller->SetActorScale3D(FMath::Lerp(Caller->GetActorScale(), Caller->GetActorScale() / 2, 0.05f));
	//	}
	//	else if (CurrentCallerScale.X <= InitialCallerScale.X / 2)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Travelling through Crack"));
	//		// Sets position and rotation to the same as exit
	//		Caller->GetSpringArm()->bDoCollisionTest = false;
	//		if (TraversalPercentage >= 1) TraversalPercentage = 1;
	//		else TraversalPercentage += GetWorld()->GetDeltaSeconds();
	//		Caller->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//		Caller->SetActorLocation(FMath::Lerp(CorrectedEntranceLocation, CorrectedExitLocation, TraversalPercentage));
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Actor reached exit"));
	//	TraversalPercentage = 0;
	//	Caller->GetSpringArm()->bDoCollisionTest = true;
	//	Caller->SetActorLocation(CorrectedExitLocation);
	//	Caller->SetActorScale3D(InitialCallerScale);
	//	Caller->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//	Caller->SetState(ESpiritState::Walking);
	//}
}

// Called every frame
void AInteractableCrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AInteractableCrack::OnInteract_Implementation(ASpirit* Caller)
{
	/*UE_LOG(LogTemp, Warning, TEXT("CrackEntrance: %s"), Caller->GetCrackEntrance() ? TEXT("True") : TEXT("False"));
	if (Caller->GetCrackEntrance())
		TraverseCrack(Caller, Crack1, &CrackExit2->GetComponentTransform());
	else
		TraverseCrack(Caller, Crack2, &CrackExit1->GetComponentTransform());*/

}

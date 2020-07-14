// Fill out your copyright notice in the Description page of Project Settings.


#include "Spirit.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InteractableClasses/InteractableInterface.h"
#include "InteractableClasses/InteractablePlant.h"
#include "InteractableClasses/InteractableCrack.h"
#include "InteractableClasses/InteractableDebris.h"
#include "InteractableClasses/InteractablePlate.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values
ASpirit::ASpirit()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(60.f);
	GetCapsuleComponent()->SetCapsuleRadius(30.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	DefaultMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMesh"));
	DefaultMesh->SetupAttachment(RootComponent);

	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterMesh"));
	WaterMesh->SetupAttachment(RootComponent);

	IceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IceMesh"));
	IceMesh->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BashCollider"));
	SphereComponent->SetupAttachment(IceMesh);

	SphereComponent->SetSphereRadius(55.f);

	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	SetForm(Default);
	SpiritState = Idle;

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;

}

// Called every frame
void ASpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BashCooldownDuration < BashCooldown)
		BashCooldownDuration += GetWorld()->GetDeltaSeconds();

	if (SpiritForm == Ice)
		TraceBelow();
	else if(ActivatedPlate)
		DeactivatePlate();

	switch (SpiritState)
	{
	case Idle:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, TEXT("Idle"));
		TraceForward();
		break;
	case Walking:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("Walking"));
		if (GetInputAxisValue("MoveForward") == 0 && GetInputAxisValue("MoveRight") == 0)
			SetState(Idle);
		TraceForward();
		break;
	case Jumping:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, TEXT("Jumping"));
		break;
	case Interacting:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Interacting"));
		Revive();
		break;
	case Squeezing:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Squeezing"));
		CallTraverseFromCrack();
		break;
	case Bashing:
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, TEXT("Bashing"));
		Bash();
		break;
	}
}

// Called to bind functionality to input
void ASpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the action buttons
	PlayerInputComponent->BindAction<SetFormDelegate>("DefaultFormButton", IE_Pressed, this, &ASpirit::SetForm, Default);
	PlayerInputComponent->BindAction<SetFormDelegate>("WaterFormButton", IE_Pressed, this, &ASpirit::SetForm, Water);
	PlayerInputComponent->BindAction<SetFormDelegate>("IceFormButton", IE_Pressed, this, &ASpirit::SetForm, Ice);
	PlayerInputComponent->BindAction("ActionButton", IE_Pressed, this, &ASpirit::Action);
	PlayerInputComponent->BindAction("ActionButton", IE_Released, this, &ASpirit::ReleaseReviveButton);

	// Binds the movement axes
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpirit::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpirit::MoveRight);

	// Binds the camera control axes
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpirit::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpirit::LookUpAtRate);

	// Binds the reset buttons
	PlayerInputComponent->BindAction("ResetLocationButton", IE_Pressed, this, &ASpirit::ResetLocation);
}

void ASpirit::SetForm(ESpiritForm CurrentForm)
{
	SpiritForm = CurrentForm;

	switch (SpiritForm)
	{
	case Default:
		DefaultMesh->SetVisibility(true);
		WaterMesh->SetVisibility(false);
		IceMesh->SetVisibility(false);
		break;
	case Water:
		DefaultMesh->SetVisibility(false);
		WaterMesh->SetVisibility(true);
		IceMesh->SetVisibility(false);
		break;
	case Ice:
		DefaultMesh->SetVisibility(false);
		WaterMesh->SetVisibility(false);
		IceMesh->SetVisibility(true);
		break;
	}
}

void ASpirit::SetState(ESpiritState CurrentState)
{
	SpiritState = CurrentState;
}

void ASpirit::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpirit::OnOverlapBegin);
	SphereComponent->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);

	InitialLocation = GetActorLocation();
}

void ASpirit::MoveForward(float Value)
{
	if ((SpiritState == Idle || SpiritState == Walking) && Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		SetState(Walking);
	}
}

void ASpirit::MoveRight(float Value)
{
	if ((SpiritState == Idle || SpiritState == Walking) && Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		SetState(Walking);
	}
}

void ASpirit::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpirit::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds());
}

void ASpirit::Revive()
{
	AActor* InteractableActor = HitResult.GetActor();
	if (InteractableActor)
	{
		AInteractablePlant* InteractablePlant = Cast<AInteractablePlant>(InteractableActor);
		UE_LOG(LogTemp, Warning, TEXT("Plant is grown: %s"), InteractablePlant->bPlantIsGrown ? TEXT("True") : TEXT("False"));
		if (!InteractablePlant->bPlantIsGrown)
		{
			IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractableActor);
			if (Interface)
				Interface->Execute_OnInteract(InteractableActor, this);
		}
		else
		{
			IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractableActor);
			if (Interface)
				Interface->Execute_OnInteractEnd(InteractableActor, this);
		}
	}
	SpiritState = Idle;
	UE_LOG(LogTemp, Warning, TEXT("Revived"));
}

void ASpirit::Jump()
{
	ACharacter::Jump();
}

void ASpirit::Bash()
{
	if (BashTime < BashDuration)
	{
		FRotator SpiritRotation = GetCapsuleComponent()->GetComponentRotation();
		const FVector Direction = FRotationMatrix(SpiritRotation).GetUnitAxis(EAxis::X);
		ACharacter::LaunchCharacter(FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal() * BashDistance, true, true);
		BashTime += GetWorld()->GetDeltaSeconds();
	}
	else
	{
		BashTime = 0.f;
		BashCooldownDuration = 0.f;
		SphereComponent->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
		SetState(Idle);
	}
}

void ASpirit::Action()
{
	switch (SpiritForm)
	{
	case Default:
		bReviveButtonPressed = true;
		break;
	case Water:
		Jump();
		break;
	case Ice:
		if (BashCooldownDuration >= BashCooldown)
		{
			SetState(Bashing);
			SphereComponent->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
		}
	}
}

void ASpirit::ReleaseReviveButton()
{
	if (bReviveButtonPressed)
		bReviveButtonPressed = false;
}

void ASpirit::TraceForward_Implementation()
{
	FRotator SpiritRotation = GetCapsuleComponent()->GetComponentRotation();

	FVector LineStart = GetActorLocation();
	LineStart.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight()/4;
	FVector LineEnd = LineStart + (SpiritRotation.Vector() * TraceForwardLength);

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	bool bHit = GetWorld()->LineTraceSingleByObjectType(OUT HitResult, LineStart, LineEnd, ECC_Destructible, TraceParameters);

	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red, false, 0.f, 0.f, 2.f);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 0.f, 0.f, 10.f);
		switch (SpiritForm)
		{
		case Default:
			if (HitResult.Actor->IsA<AInteractablePlant>() && bReviveButtonPressed)
			{
				//Revive();
				SpiritState = Interacting;
			}
			break;
		case Water:
			if (HitResult.Actor->IsA<AInteractableCrack>())
			{
				if (HitResult.GetComponent()->GetRelativeLocation() == FVector::ZeroVector)
					bIsCrackEntrance = true;
				else
					bIsCrackEntrance = false;
				SpiritState = Squeezing;
			}
			break;
		}
	}

	/*if (bHit && HitResult.Actor->IsA<AInteractableCrack>())
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 0.f, 0.f, 10.f);

		if (HitResult.GetComponent()->GetRelativeLocation() == FVector::ZeroVector)
			bIsCrackEntrance = true;
		else
			bIsCrackEntrance = false;
		SpiritState = Squeezing;
	}*/

	/*if (bHit && HitResult.Actor->IsA<AInteractablePlant>())
	{
		if(GetInputAxisKeyValue(""))
	}*/

	/*if (bHit)
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 0.f, 0.f, 10.f);

		AActor* Interactable = HitResult.GetActor();

		if (Interactable && Interactable != FocusedActor)
		{
			FocusedActor = Interactable;
			IInteractInterface* Interface = Cast<IInteractInterface>(FocusedActor);
			if (Interface)
				Interface->Execute_OnInteract(FocusedActor, this);
		}
	}
	else
	{
		if (FocusedActor)
		{
			IInteractInterface* Interface = Cast<IInteractInterface>(FocusedActor);
			if (Interface)
				Interface->Execute_EndFocus(FocusedActor);
		}
		FocusedActor = nullptr;
	}*/
}

void ASpirit::TraceBelow_Implementation()
{
	FVector LineStart = GetActorLocation();
	FVector LineEnd = LineStart + (FRotator(270, 0, 0).Vector() * TraceBelowLength);

	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	bool bHit = GetWorld()->LineTraceSingleByObjectType(OUT HitResult, LineStart, LineEnd, ECC_Destructible, TraceParameters);

	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red, false, 0.f, 0.f, 2.f);

	if (bHit && HitResult.Actor->IsA<AInteractablePlate>())
	{
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 0.f, 0.f, 10.f);

		AActor* InteractableActor = HitResult.GetActor();
		if (InteractableActor && InteractableActor != ActivatedPlate)
		{
			ActivatedPlate = InteractableActor;
			IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractableActor);
			if (Interface)
				Interface->Execute_OnInteract(InteractableActor, this);
		}
	}
	else
	{
		DeactivatePlate();
	}
}

void ASpirit::CallTraverseFromCrack()
{
	AActor* InteractableActor = HitResult.GetActor();
	if (InteractableActor)
	{
		IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractableActor);
		if (Interface)
			Interface->Execute_OnInteract(InteractableActor, this);
	}
}

void ASpirit::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (SpiritState == Bashing && OtherActor->IsA<AInteractableDebris>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap called"));
		IInteractableInterface* Interface = Cast<IInteractableInterface>(OtherActor);
		if (Interface)
			Interface->Execute_OnInteract(OtherActor, this);
	}

}

void ASpirit::DeactivatePlate()
{
	IInteractableInterface* Interface = Cast<IInteractableInterface>(ActivatedPlate);
	if (Interface)
		Interface->Execute_OnInteractEnd(ActivatedPlate, this);
	ActivatedPlate = nullptr;
}

void ASpirit::ResetMap()
{

}

void ASpirit::ResetLocation()
{
	SetActorLocation(InitialLocation);
}

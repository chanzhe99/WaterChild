// Fill out your copyright notice in the Description page of Project Settings.


#include "Spirit.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ASpirit::ASpirit()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	Morph(Default);

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;
}


void ASpirit::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}


void ASpirit::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
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
	if (SpiritState != Default)
		Morph(Default);
}

void ASpirit::Jump()
{
	if (SpiritState != Water)
		Morph(Water);

	ACharacter::Jump();
}

void ASpirit::Bash()
{
	if (SpiritState != Ice)
		Morph(Ice);

	FRotator CapsuleRotation = GetCapsuleComponent()->GetComponentRotation();

	const float BashDistance = 5000.f;
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(CapsuleRotation).GetUnitAxis(EAxis::X);
	ACharacter::LaunchCharacter(FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal() * BashDistance, true, true);
}

void ASpirit::Morph(ESpiritState CurrentState)
{
	SpiritState = CurrentState;

	switch (SpiritState)
	{
	case ASpirit::Default:
		DefaultMesh->SetVisibility(true);
		WaterMesh->SetVisibility(false);
		IceMesh->SetVisibility(false);
		break;
	case ASpirit::Water:
		DefaultMesh->SetVisibility(false);
		WaterMesh->SetVisibility(true);
		IceMesh->SetVisibility(false);
		break;
	case ASpirit::Ice:
		DefaultMesh->SetVisibility(false);
		WaterMesh->SetVisibility(false);
		IceMesh->SetVisibility(true);
		break;
	case ASpirit::Jumping:
		break;
	case ASpirit::Bashing:
		break;
	}
}

// Called to bind functionality to input
void ASpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the action buttons
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASpirit::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Bash", IE_Pressed, this, &ASpirit::Bash);
	PlayerInputComponent->BindAction("Revive", IE_Pressed, this, &ASpirit::Revive);

	// Binds the movement axes
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpirit::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpirit::MoveRight);

	// Binds the camera control axes
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpirit::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpirit::LookUpAtRate);
}


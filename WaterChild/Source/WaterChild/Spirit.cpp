// Fill out your copyright notice in the Description page of Project Settings.


#include "Spirit.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "InteractableClasses/InteractableInterface.h"
#include "InteractableClasses/InteractablePlant.h"
#include "InteractableClasses/InteractableCrack.h"
#include "InteractableClasses/InteractableDebris.h"
#include "InteractableClasses/Plant.h"
#include "InteractableClasses/SpringPlant.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetInputLibrary.h"
#include "GameFramework/PlayerInput.h"

#define OUT

// Sets default values
ASpirit::ASpirit()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set controller default values
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->MaxWalkSpeed = 300;
	GetCharacterMovement()->JumpZVelocity = 200;
	GetCharacterMovement()->AirControl = 0.25;
	GetCharacterMovement()->RotationRate.Yaw = 720;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Creates subobject for components
	SkeletalMeshWater = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshWater"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ArrowLineTrace = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowLineTrace"));
	ArrowLeftFoot = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowLeftFoot"));
	ArrowRightFoot = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowRightFoot"));
	NiagaraFootsteps = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFootsteps"));
	NiagaraRevive = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRevive"));
	NiagaraJumpDefault = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpDefault"));
	NiagaraJumpWater = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpWater"));

	// Setup attachments for components
	SkeletalMeshWater->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	ArrowLeftFoot->SetupAttachment(GetMesh(), "Spirit_Rig_LLegAnkle");
	ArrowRightFoot->SetupAttachment(GetMesh(), "Spirit_Rig_RLegAnkle");
	NiagaraFootsteps->SetupAttachment(GetMesh());
	NiagaraRevive->SetupAttachment(ArrowLineTrace);
	NiagaraJumpDefault->SetupAttachment(GetMesh());
	NiagaraJumpWater->SetupAttachment(SkeletalMeshWater);
	ArrowLineTrace->SetupAttachment(RootComponent);

	// Set component position offsets
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -27), FRotator(0, -90, 0));
	SkeletalMeshWater->SetRelativeLocationAndRotation(FVector(0, 0, -27), FRotator(0, -90, 0));

	SpringArm->SetRelativeLocation(FVector(-5, 0, 0));
	SpringArm->SocketOffset = FVector(0, 0, 60);
	SpringArm->bEnableCameraLag = true;

	NiagaraRevive->SetRelativeRotation(FRotator(0, 270, 0));

	// Set character mesh collision profiles
	SkeletalMeshWater->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Set component default values
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	ArrowLeftFoot->SetRelativeLocationAndRotation(FVector(0, -1, 0), FRotator(0, -97, 0));
	ArrowRightFoot->SetRelativeLocationAndRotation(FVector(0, -1, 0), FRotator(0, -97, 0));

	NiagaraFootsteps->SetAutoActivate(false);
	NiagaraRevive->SetAutoActivate(false);
	NiagaraJumpDefault->SetAutoActivate(false);
	NiagaraJumpWater->SetAutoActivate(false);

	GetCapsuleComponent()->SetCapsuleRadius(12.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(25.f);

	// Set default variable values
	//SpiritState = ESpiritState::Idle;

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;

	JumpChargeDuration = 0.1f;
	JumpChargeTime = 0;

	bIsCrackEntrance = true;
	SelectedPlant = nullptr;
	SelectedCrack = nullptr;
}

// Called when the game starts or when spawned
void ASpirit::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASpirit::OnOverlapBegin);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
}

// Called every frame
void ASpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SUE_LOG(LogTemp, Warning, TEXT("Using Gamepad: %s"), (bIsUsingGamepad) ? TEXT("TRUE") : TEXT("FALSE"));
	//UE_LOG(LogTemp, Warning, TEXT("Velocity: X: %f, Y: %f, Z: %f"), GetVelocity().X, GetVelocity().Y, GetVelocity().Z);

	FHitResult Hit;
	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FCollisionQueryParams TraceParams;
	Hit = FHitResult(ForceInit);

	FVector RightVector = FVector::CrossProduct(FVector::UpVector, WallNormal);
	FVector LineEnd = LineStart + (RightVector * 100);
	GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_WorldStatic, TraceParams);
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Magenta, false, 0, 0, 2);

	if (bIsCheckingForClimbable && bCanTransitionToClimb)
	{
		if (TraceLine(ClimbTraceLength).GetActor() && TraceLine(ClimbTraceLength).GetActor()->ActorHasTag("Climbable"))
		{
			bIsCheckingForClimbable = false;
			bCanTransitionToClimb = false; UE_LOG(LogTemp, Warning, TEXT("CanClimb False"))
			SetState(ESpiritState::Tutorial);
			TransitionToClimb();
		}
	}

	FRotator CameraTargetRotation = (bCameraFollowRevive) ? ArrowLineTrace->GetForwardVector().Rotation() : GetActorForwardVector().Rotation();
	CameraTargetRotation += FRotator(-20, 0, 0);
	//FRotator CameraTargetRotation = FRotator(ArrowLineTrace->GetForwardVector().Rotation().Pitch - 10, GetActorForwardVector().Rotation().Yaw, GetActorForwardVector().Rotation().Roll);
	//FRotator CameraTargetRotation = ArrowLineTrace->GetForwardVector().Rotation() + FRotator(-20, 0, 0);

	switch (SpiritState)
	{
	case ESpiritState::Idle:
		if (GetVelocity().Size() > 0)
			SetState(ESpiritState::Walking);
		break;
	case ESpiritState::Walking:
		if (GetVelocity().Size() == 0)
			SetState(ESpiritState::Idle);
		if (GetCharacterMovement()->IsFalling())
			SetState(ESpiritState::Falling);

		TraceHit = TraceLine(SqueezeTraceLength);
		if (TraceHit.GetActor() && TraceHit.GetActor()->IsA(AInteractableCrack::StaticClass()))
		{
			SelectedCrack = Cast<AInteractableCrack>(TraceHit.GetActor());
			if (TraceHit.GetComponent()->GetRelativeLocation() == FVector::ZeroVector)
				bIsCrackEntrance = true;
			else bIsCrackEntrance = false;
			SetState(ESpiritState::Squeezing);
		}
		break;
	case ESpiritState::Falling:
		TraceHit = TraceLine(ClimbTraceLength);
		if (!GetCharacterMovement()->IsFalling())
		{
			NiagaraJumpDefault->Deactivate();
			NiagaraJumpDefault->Activate();
			SetState(ESpiritState::Idle);
		}

		if (TraceHit.GetActor() && TraceHit.GetActor()->IsA(AInteractableCrack::StaticClass()))
		{
			SelectedCrack = Cast<AInteractableCrack>(TraceHit.GetActor());
			if (TraceHit.GetComponent()->GetRelativeLocation() == FVector::ZeroVector)
				bIsCrackEntrance = true;
			else bIsCrackEntrance = false;
			SetState(ESpiritState::Squeezing);
		}

		break;
	case ESpiritState::Reviving:
		Controller->SetControlRotation(UKismetMathLibrary::RInterpTo(Controller->GetControlRotation(), CameraTargetRotation, GetWorld()->GetDeltaSeconds(), 5.f));
		TraceHit = TraceLine(ReviveTraceLength);
		OnRevive(Cast<APlant>(TraceLine(ReviveTraceLength).GetActor()));
		break;
	case ESpiritState::ChargingJump:
		OnJump();
		break;
	case ESpiritState::Squeezing:
		if(SelectedCrack)
			OnSqueeze(SelectedCrack);
		break;
	case ESpiritState::Climbing:
		
		//FVector WallNormal = TraceLine(ClimbTraceLength).ImpactNormal;
		//WallNormal = UKismetMathLibrary::VInterpTo_Constant(WallNormal, TraceLine(ClimbTraceLength).ImpactNormal, GetWorld()->GetDeltaSeconds(), 10.f);
		//ClimbConstantVelocityDirection = GetVelocity().GetSafeNormal();
		
		// TODO If statement causes player to drop down after stopping and continuing to climb on wall
		// FIXED swapped current and target vector interp around and fixed falling off wall issue
		// which was caused by spirit rotating 180 degrees when they reached fully pointing up or down on the wall
		// This, however, causes the spirit to instantly face the direction they are climbing when they move after stopping for a while.

		// Rotation caused linetrace to not detect the wall, thus causing it to fall off. This issue could be remedied if
		// the spirit does not fall off when it doesnt detect a wall, ala edge of wall mechanic

		// After implementing the wall edge detection, the issue was still prevalent as the spirit model would rotate 180 degrees
		// when the player quickly inputs forward & backward
		// During implementation of the edge detection, I discovered that the formula that was used to produce the "RightVector" instead
		// creates a LeftVector. With this realisation, I tried inversing the RightVector value and used that to make the ClimbRotation
		// which finally fixed the issue. The current and target vector interp was swapped back around as the root issue had no relation
		// to the function. This eliminated the spirit instantly facing the direction they are climbing when they move after stopping for a while.

		if (GetVelocity().GetSafeNormal().Size() > 0.01f)
			ClimbConstantVelocityDirection = UKismetMathLibrary::VInterpTo_Constant(ClimbConstantVelocityDirection, GetVelocity().GetSafeNormal(), GetWorld()->GetDeltaSeconds(), 5.f);

		FVector CrossProduct = FVector::CrossProduct(ClimbConstantVelocityDirection, WallNormal) * -1;
		ClimbForwardVector = FVector::CrossProduct(CrossProduct, WallNormal);
		ClimbRightVector = FVector::CrossProduct(ClimbForwardVector, WallNormal) * -1;

		ClimbRotation = UKismetMathLibrary::MakeRotationFromAxes(ClimbForwardVector, ClimbRightVector, WallNormal);
		SetActorRotation(ClimbRotation);

		//UE_LOG(LogTemp, Warning, TEXT("WallNormal: X: %f, Y: %f, Z: %f"), WallNormal.X, WallNormal.Y, WallNormal.Z);
		//UE_LOG(LogTemp, Warning, TEXT("CrossProduct: X: %f, Y: %f, Z: %f"), CrossProduct.X, CrossProduct.Y, CrossProduct.Z);
		//UE_LOG(LogTemp, Warning, TEXT("ClimbFwVector: X: %f, Y: %f, Z: %f"), ClimbForwardVector.X, ClimbForwardVector.Y, ClimbForwardVector.Z);
		//UE_LOG(LogTemp, Warning, TEXT("ClimbRot: P: %f, Y: %f, R: %f"), ClimbRotation.Pitch, ClimbRotation.Yaw, ClimbRotation.Roll);
		//UE_LOG(LogTemp, Warning, TEXT("WallRot: P: %f, Y: %f, R: %f"), WallRotation.Pitch, WallRotation.Yaw, WallRotation.Roll);


		if (!bIsClimbButtonDown || !TraceLine(ClimbTraceLength).GetActor())
		{
			SetActorRotation(BaseRotation);
			ArrowLineTrace->SetWorldRotation(GetActorForwardVector().Rotation());

			GetMesh()->Activate();
			GetMesh()->SetVisibility(true);
			SkeletalMeshWater->Deactivate();
			SkeletalMeshWater->SetVisibility(false);
			GetCharacterMovement()->bOrientRotationToMovement = true;

			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			SetState(ESpiritState::Falling);
			TransitionToFall();
			//bCanTransitionToClimb = true; UE_LOG(LogTemp, Warning, TEXT("CanClimb True"))
		}

		break;
	}
}

// Called to bind functionality to input
void ASpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the PC buttons
	PlayerInputComponent->BindAction("ActionButton", IE_Pressed, this, &ASpirit::Revive);
	PlayerInputComponent->BindAction("ActionButton", IE_Released, this, &ASpirit::StopRevive);
	PlayerInputComponent->BindAction("JumpButton", IE_Pressed, this, &ASpirit::Jump);
	PlayerInputComponent->BindAction("ClimbButton", IE_Pressed, this, &ASpirit::Climb);
	PlayerInputComponent->BindAction("ClimbButton", IE_Released, this, &ASpirit::StopClimb);

	// Binds the PC axes
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpirit::MoveForwardKeyboard);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpirit::MoveRightKeyboard);
	PlayerInputComponent->BindAxis("Turn", this, &ASpirit::TurnAt);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpirit::LookUpAt);

	// Binds the controller axes
	PlayerInputComponent->BindAxis("MoveForwardGamepad", this, &ASpirit::MoveForwardGamepad);
	PlayerInputComponent->BindAxis("MoveRightGamepad", this, &ASpirit::MoveRightGamepad);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpirit::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpirit::LookUpAtRate);

	// Checks input type
	PlayerInputComponent->BindAction("AnyKey", IE_Pressed, this, &ASpirit::CheckInputType);
}

void ASpirit::MoveForward(float Value)
{
	if (Controller && SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Tutorial)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		
		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator RevivePitch(Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0, 0);
			ArrowLineTrace->AddRelativeRotation(RevivePitch);

			if (ArrowLineTrace->GetRelativeRotation().Pitch >= ReviveMaxHeight)
				ArrowLineTrace->SetRelativeRotation(FRotator(ReviveMaxHeight, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
			else if (ArrowLineTrace->GetRelativeRotation().Pitch <= ReviveMinHeight)
				ArrowLineTrace->SetRelativeRotation(FRotator(ReviveMinHeight, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
		}
		else if (SpiritState == ESpiritState::Climbing)
		{
			if (ClimbTraceLine(FVector2D(Value, 0)).GetActor() && ClimbTraceLine(FVector2D(Value, 0)).GetActor()->ActorHasTag("Climbable"))
			{
				const FRotator WallUpRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
				const FVector ClimbDirection = FRotationMatrix(WallUpRotation - FRotator(0, 180, 0)).GetUnitAxis(EAxis::Z);
				AddMovementInput(ClimbDirection, Value);
			}
		}
		else
		{
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void ASpirit::MoveRight(float Value)
{
	if (Controller && SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Tutorial)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator ReviveYaw(0, Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0);
			ArrowLineTrace->AddRelativeRotation(ReviveYaw);

			if (ArrowLineTrace->GetRelativeRotation().Yaw >= ReviveMaxYawAngle)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, ReviveMaxYawAngle, 0));
				AddActorWorldRotation(ReviveYaw);
			}
			else if (ArrowLineTrace->GetRelativeRotation().Yaw <= -ReviveMaxYawAngle)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, -ReviveMaxYawAngle, 0));
				AddActorWorldRotation(ReviveYaw);
			}
		}
		else if (SpiritState == ESpiritState::Climbing)
		{
			if (ClimbTraceLine(FVector2D(0, Value)).GetActor() && ClimbTraceLine(FVector2D(0, Value)).GetActor()->ActorHasTag("Climbable"))
			{
				const FRotator WallUpRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
				const FVector ClimbDirection = FRotationMatrix(WallUpRotation - FRotator(0, 180, 0)).GetUnitAxis(EAxis::Y);
				AddMovementInput(ClimbDirection, Value);
			}
		}
		else
		{
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
	}
}

void ASpirit::MoveForwardKeyboard(float Value)
{
	if (Value != 0.f)
	{
		bIsUsingGamepad = false;
		MoveForward(Value);
	}
}

void ASpirit::MoveRightKeyboard(float Value)
{
	if (Value != 0.f)
	{
		bIsUsingGamepad = false;
		MoveRight(Value);
	}
}

void ASpirit::MoveForwardGamepad(float Value)
{
	if (Value != 0.f)
	{
		bIsUsingGamepad = true;
		MoveForward(Value);
	}
}

void ASpirit::MoveRightGamepad(float Value)
{
	if (Value != 0.f)
	{
		bIsUsingGamepad = true;
		MoveRight(Value);
	}
}

void ASpirit::TurnAt(float Value)
{
	if (Value != 0.f) bIsUsingGamepad = false;
	if (SpiritState == ESpiritState::Reviving)
	{
		const FRotator ReviveYaw(0, Value, 0);
		ArrowLineTrace->AddRelativeRotation(ReviveYaw);

		if (ArrowLineTrace->GetRelativeRotation().Yaw >= ReviveMaxYawAngle)
		{
			ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, ReviveMaxYawAngle, 0));
			AddActorWorldRotation(ReviveYaw);
		}
		else if (ArrowLineTrace->GetRelativeRotation().Yaw <= -ReviveMaxYawAngle)
		{
			ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, -ReviveMaxYawAngle, 0));
			AddActorWorldRotation(ReviveYaw);
		}
	}
	else AddControllerYawInput(Value);
}

void ASpirit::LookUpAt(float Value)
{
	if (Value != 0.f) bIsUsingGamepad = false;
	if (SpiritState == ESpiritState::Reviving)
	{
		const FRotator RevivePitch(-Value, 0, 0);
		ArrowLineTrace->AddRelativeRotation(RevivePitch);

		if (ArrowLineTrace->GetRelativeRotation().Pitch >= ReviveMaxHeight)
			ArrowLineTrace->SetRelativeRotation(FRotator(ReviveMaxHeight, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
		else if (ArrowLineTrace->GetRelativeRotation().Pitch <= ReviveMinHeight)
			ArrowLineTrace->SetRelativeRotation(FRotator(ReviveMinHeight, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
	}
	else AddControllerPitchInput(Value);
}

void ASpirit::TurnAtRate(float Value)
{
	if (Value != 0.f) bIsUsingGamepad = true;
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpirit::LookUpAtRate(float Value)
{
	if (Value != 0.f) bIsUsingGamepad = true;
	AddControllerPitchInput(Value * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds());
}

void ASpirit::Action()
{
	if (SpiritState == ESpiritState::Idle || SpiritState == ESpiritState::Walking)
	{
		SetState(ESpiritState::Reviving);
		NiagaraRevive->Activate();
	}
}

void ASpirit::StopAction()
{
	if (SpiritState == ESpiritState::Reviving)
	{
		if (SelectedPlant)
		{
			IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedPlant);
			if (Interface) Interface->Execute_OnInteractEnd(SelectedPlant, this);
			SelectedPlant = nullptr;
		}
		ArrowLineTrace->SetRelativeRotation(FRotator::ZeroRotator);
		NiagaraRevive->Deactivate();
		SetState(ESpiritState::Idle);
	}
}

void ASpirit::Jump()
{
	if (SpiritState == ESpiritState::Idle || SpiritState == ESpiritState::Walking)
		//SetState(ESpiritState::ChargingJump);
		ACharacter::Jump();
}

void ASpirit::Climb()
{
	//UE_LOG(LogTemp, Warning, TEXT("Climb"))
	bIsClimbButtonDown = true;
	if (SpiritState != ESpiritState::Climbing)
		bIsCheckingForClimbable = true;
}

void ASpirit::StopClimb()
{
	//UE_LOG(LogTemp, Warning, TEXT("Stopped Climb"))
	bIsClimbButtonDown = false;
	bIsCheckingForClimbable = false;
	/*if (SpiritState == ESpiritState::Climbing)
	{
		SetActorRotation(BaseRotation);
		ArrowLineTrace->SetWorldRotation(GetActorForwardVector().Rotation());
		
		GetMesh()->Activate();
		GetMesh()->SetVisibility(true);
		SkeletalMeshWater->Deactivate();
		SkeletalMeshWater->SetVisibility(false);
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		SetState(ESpiritState::Falling);
	}*/
}

FHitResult ASpirit::TraceLine(float TraceLength)
{
	FHitResult Hit;

	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FRotator LineRotation = ArrowLineTrace->GetComponentRotation();
	FVector LineEnd = LineStart + (LineRotation.Vector() * TraceLength);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(TEXT("Trace"), true, this);
	TraceParams.bTraceComplex = true;
	Hit = FHitResult(ForceInit);

	//UE_LOG(LogTemp, Warning, TEXT("IsTraceComplex: %s"), (TraceParams.bTraceComplex) ? TEXT("True") : TEXT("False"));
	//TraceParams.bTraceComplex;

	//TODO try out kismet linetrace to see if it works for mural
	//UKismetSystemLibrary::LineTraceSingleForObjects();

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_WorldStatic, TraceParams);
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 0, 0, 2);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Orange, false, 2);
		
		//UE_LOG(LogTemp, Warning, TEXT("Crack Location: %f, %f"), Hit.GetComponent()->GetRelativeLocation().X, Hit.GetComponent()->GetRelativeLocation().Y);
		return Hit;
	}
	return FHitResult();
}

FHitResult ASpirit::ClimbTraceLine(FVector2D Direction)
{
	FHitResult Hit;
	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FVector LineEnd = FVector::ZeroVector;
	FCollisionQueryParams TraceParams;
	Hit = FHitResult(ForceInit);
	float DownTraceLength = 30;
	float TraceLength = 50;
	FVector WallRightVector = FVector::CrossProduct(FVector::UpVector, WallNormal) * -1;

	if (Direction.X > 0)
		LineEnd = LineStart + (WallNormal * -DownTraceLength) + (FVector::UpVector * TraceLength);
	else if (Direction.X < 0)
		LineEnd = LineStart + (WallNormal * -DownTraceLength) + (FVector::DownVector * TraceLength);
	else if (Direction.Y > 0)
		LineEnd = LineStart + (WallNormal * -DownTraceLength) + (WallRightVector * TraceLength);
	else if (Direction.Y < 0)
		LineEnd = LineStart + (WallNormal * -DownTraceLength) + (WallRightVector * -TraceLength);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_WorldStatic, TraceParams);
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Magenta, false, 0, 0, 2);

	if (bHit)
	{
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Orange, false, 2);
		return Hit;
	}
	return FHitResult();
}

void ASpirit::TransitionToClimb_Implementation()
{	
	FRotator TransitionStartRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation() - FRotator(0, 180, 0);
	
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	SetActorRotation(TransitionStartRotation);
	UE_LOG(LogTemp, Warning, TEXT("TransitionToClimb Called"))
}

void ASpirit::SetClimbOrientation_Implementation()
{
	//SpringArm->bUsePawnControlRotation = false;
	//UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
	AActor* HitActor = TraceLine(ClimbTraceLength).GetActor();

	BaseRotation = GetActorRotation();

	// Get wall hit location & rotation
	FVector WallLocation = TraceLine(ClimbTraceLength).Location;
	WallNormal = TraceLine(ClimbTraceLength).ImpactNormal;

	// Get start location & rotation
	FVector TransitionStartLocation = GetActorLocation();
	FRotator TransitionStartRotation = GetActorRotation();

	// Get base of mesh location
	FVector MeshBaseLocation = GetMesh()->GetComponentLocation();

	FVector CrossProduct = FVector::CrossProduct((WallLocation - MeshBaseLocation).GetSafeNormal(), WallNormal) * -1;
	FVector WallForwardVector = FVector::CrossProduct(CrossProduct, WallNormal);
	FVector WallRightVector = FVector::CrossProduct(WallForwardVector, WallNormal) * -1;

	FVector TransitionEndLocation = WallLocation + (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * WallNormal);
	FRotator TransitionEndRotation = UKismetMathLibrary::MakeRotationFromAxes(WallForwardVector, WallRightVector, WallNormal);

	//WallForwardVector = WallForwardVector.GetSafeNormal();
	//ClimbForwardVector = WallForwardVector.GetSafeNormal();
	//WallRightVector = WallRightVector.GetSafeNormal();
	//ClimbRightVector = WallRightVector.GetSafeNormal();

	SetActorLocationAndRotation(TransitionEndLocation, TransitionEndRotation);
	ClimbConstantVelocityDirection = GetActorForwardVector();

	GetMesh()->Deactivate();
	GetMesh()->SetVisibility(false);
	SkeletalMeshWater->Activate();
	SkeletalMeshWater->SetVisibility(true);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	ArrowLineTrace->SetWorldRotation((GetActorUpVector() * -1).GetSafeNormal().Rotation());

	//FRotator WallUpRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
	//UE_LOG(LogTemp, Warning, TEXT("WallNormal: X: %f, Y: %f, Z: %f"), WallNormal.X, WallNormal.Y, WallNormal.Z);
	//UE_LOG(LogTemp, Warning, TEXT("WallRightVector: X: %f, Y: %f, Z: %f"), WallRightVector.X, WallRightVector.Y, WallRightVector.Z);
	//UE_LOG(LogTemp, Warning, TEXT("WallFwVector: X: %f, Y: %f, Z: %f"), WallForwardVector.X, WallForwardVector.Y, WallForwardVector.Z);
	//UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
	//UE_LOG(LogTemp, Warning, TEXT("EndRot: P: %f, Y: %f, R: %f"), TransitionEndRotation.Pitch, TransitionEndRotation.Yaw, TransitionEndRotation.Roll);
	//UE_LOG(LogTemp, Warning, TEXT("WallUpRot: P: %f, Y: %f, R: %f"), WallUpRotation.Pitch, WallUpRotation.Yaw, WallUpRotation.Roll);

	//SetState(ESpiritState::Climbing);
}

void ASpirit::Revive_Implementation()
{
	if (SpiritState == ESpiritState::Idle || SpiritState == ESpiritState::Walking)
	{
		SetState(ESpiritState::Reviving);
		NiagaraRevive->Activate();
	}
}

void ASpirit::StopRevive_Implementation()
{
	if (SpiritState == ESpiritState::Reviving)
	{
		if (SelectedPlant)
		{
			IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedPlant);
			if (Interface) Interface->Execute_OnInteractEnd(SelectedPlant, this);
			SelectedPlant = nullptr;
		}
		ArrowLineTrace->SetRelativeRotation(FRotator::ZeroRotator);
		NiagaraRevive->Deactivate();
		SetState(ESpiritState::Idle);
	}
}

void ASpirit::OnRevive_Implementation(APlant* PlantHit)
{
	if (PlantHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Plant Hit"));
		if (PlantHit != SelectedPlant)
		{
			if (SelectedPlant)
			{
				IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedPlant);
				if (Interface) Interface->Execute_OnInteractEnd(SelectedPlant, this);
			}
		}
		IInteractableInterface* Interface = Cast<IInteractableInterface>(PlantHit);
		if (Interface) Interface->Execute_OnInteract(PlantHit, this);
		SelectedPlant = PlantHit;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Plant Hit"));
		if (SelectedPlant)
		{
			IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedPlant);
			if (Interface) Interface->Execute_OnInteractEnd(SelectedPlant, this);
		}
		SelectedPlant = nullptr;
	}
}

void ASpirit::OnJump_Implementation()
{
	if (JumpChargeTime < JumpChargeDuration) JumpChargeTime += GetWorld()->GetDeltaSeconds();
	else
	{
		ACharacter::Jump();
		JumpChargeTime = 0;
		SetState(ESpiritState::Falling);
	}
}

void ASpirit::OnSqueeze_Implementation(AInteractableCrack* CrackHit)
{
	if (CrackHit)
	{
		IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedCrack);
		if (Interface) Interface->Execute_OnInteract(SelectedCrack, this);
	}
}

void ASpirit::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap test called"));
	if (OtherActor->IsA(AInteractableDebris::StaticClass()))
	{
		IInteractableInterface* Interface = Cast<IInteractableInterface>(OtherActor);
		if (Interface) Interface->Execute_OnInteract(OtherActor, this);
	}

	if (OtherActor->IsA(ASpringPlant::StaticClass()) 
		&& Cast<ASpringPlant>(OtherActor)->GetPlantState() == EPlantState::Alive 
		&& SpiritState == ESpiritState::Falling)
	{
		UE_LOG(LogTemp, Warning, TEXT("Springing"));
		Cast<ASpringPlant>(OtherActor)->BounceAnimation(this);
	}

	/*if (OtherActor->ActorHasTag("Climbable"))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		SetState(ESpiritState::Climbing);
	}*/
}

void ASpirit::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (OtherActor->ActorHasTag("Climbable"))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		SetState(ESpiritState::Falling);
	}*/
}

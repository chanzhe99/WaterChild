// Fill out your copyright notice in the Description page of Project Settings.


#include "Spirit.h"
//#include "GameFramework/PlayerController.h"
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
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

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
	NiagaraFootsteps = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFootsteps"));
	NiagaraRevive = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRevive"));
	NiagaraJumpDefault = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpDefault"));
	NiagaraJumpWater = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpWater"));
	ArrowLineTrace = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowLineTrace"));

	// Setup attachments for components
	SkeletalMeshWater->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	NiagaraFootsteps->SetupAttachment(GetMesh());
	NiagaraRevive->SetupAttachment(ArrowLineTrace);
	NiagaraJumpDefault->SetupAttachment(GetMesh());
	NiagaraJumpWater->SetupAttachment(SkeletalMeshWater);
	ArrowLineTrace->SetupAttachment(RootComponent);

	// Set component position offsets
	GetMesh()->SetRelativeLocation(FVector(0, 0, -27));
	SkeletalMeshWater->SetRelativeLocation(FVector(-14, 0, -27));

	SpringArm->SetRelativeLocation(FVector(-5, 0, 0));

	NiagaraRevive->SetRelativeRotation(FRotator(0, 270, 0));

	// Set character mesh collision profiles
	SkeletalMeshWater->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Set component default values
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	NiagaraFootsteps->SetAutoActivate(false);
	NiagaraRevive->SetAutoActivate(false);
	NiagaraJumpDefault->SetAutoActivate(false);
	NiagaraJumpWater->SetAutoActivate(false);

	GetCapsuleComponent()->SetCapsuleRadius(12.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(25.f);

	// Set default variable values
	SpiritState = ESpiritState::Idle;

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;

	ReviveTraceLength = 200;

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

	//TraceLine(ClimbTraceLength);
	//UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);

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
		break;
	case ESpiritState::Reviving:
		OnRevive(Cast<AInteractablePlant>(TraceLine(ReviveTraceLength).GetActor()));
		break;
	case ESpiritState::ChargingJump:
		OnJump();
		break;
	case ESpiritState::Squeezing:
		if(SelectedCrack)
			OnSqueeze(SelectedCrack);
		break;
	case ESpiritState::Climbing:
		FVector WallNormal = TraceLine(ClimbTraceLength).ImpactNormal;
		WallNormal = UKismetMathLibrary::VInterpTo_Constant(WallNormal, TraceLine(ClimbTraceLength).ImpactNormal, GetWorld()->GetDeltaSeconds(), 10.f);
		FVector CrossProduct = FVector::CrossProduct(ClimbConstantVelocityDirection, WallNormal) * -1;
		
		if (GetVelocity().GetSafeNormal().Size() > 0.01f)
			ClimbConstantVelocityDirection = UKismetMathLibrary::VInterpTo_Constant(ClimbConstantVelocityDirection, GetVelocity().GetSafeNormal(), GetWorld()->GetDeltaSeconds(), 5.f);
		ClimbForwardVector = FVector::CrossProduct(CrossProduct, WallNormal);
		ClimbRightVector = FVector::CrossProduct(ClimbForwardVector, WallNormal);

		ClimbRotation = UKismetMathLibrary::MakeRotationFromAxes(ClimbForwardVector, ClimbRightVector, WallNormal);

		ClimbForwardVector = ClimbForwardVector.GetSafeNormal();
		ClimbRightVector = ClimbRightVector.GetSafeNormal();

		//UE_LOG(LogTemp, Warning, TEXT("ClimbFwVector: X: %f, Y: %f, Z: %f"), ClimbForwardVector.X, ClimbForwardVector.Y, ClimbForwardVector.Z);
		SetActorRotation(ClimbRotation);
		//FRotator Rotation = Controller->GetControlRotation();
		//FRotator WallRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
		//FRotator PitchRotation = FRotator().ZeroRotator - FRotator(WallRotation.Pitch, 0, 0);
		//FRotator YawRotation = WallRotation - FRotator(0, 180, 0);
		//FRotator RollRotation = GetActorForwardVector().Rotation();
		//FRotator ClimbRotation = FRotator(PitchRotation.Pitch, YawRotation.Yaw, Rotation.Roll);
		//SetActorRotation(ClimbRotation);

		/*if (!TraceLine(ClimbTraceLength).GetActor())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			SetState(ESpiritState::Falling);
		}*/

		//GetMesh()->SetWorldRotation(FRotator(90, 0, 0));
		break;
	}
}

// Called to bind functionality to input
void ASpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the action buttons
	PlayerInputComponent->BindAction("ActionButton", IE_Pressed, this, &ASpirit::Action);
	PlayerInputComponent->BindAction("ActionButton", IE_Released, this, &ASpirit::StopAction);
	PlayerInputComponent->BindAction("JumpButton", IE_Pressed, this, &ASpirit::Jump);
	PlayerInputComponent->BindAction("ClimbButton", IE_Pressed, this, &ASpirit::Climb);
	PlayerInputComponent->BindAction("ClimbButton", IE_Released, this, &ASpirit::StopClimb);

	// Binds the movement axes
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpirit::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpirit::MoveRight);

	// Binds the camera control axes
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpirit::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpirit::LookUpAtRate);

}

void ASpirit::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		
		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator RevivePitch(Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0, 0);
			ArrowLineTrace->AddRelativeRotation(RevivePitch);

			if (ArrowLineTrace->GetRelativeRotation().Pitch >= 50)
				ArrowLineTrace->SetRelativeRotation(FRotator(50, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
			else if (ArrowLineTrace->GetRelativeRotation().Pitch <= -5)
				ArrowLineTrace->SetRelativeRotation(FRotator(-5, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
		}
		else if (SpiritState == ESpiritState::Climbing)
		{
			UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
			
			float ForwardVectorComparison = (UKismetMathLibrary::GetForwardVector(Controller->GetControlRotation()) - ClimbForwardVector).Size() - 0.6;
			FVector NewForwardVector;
			if (ForwardVectorComparison > 1.0)
				NewForwardVector = ClimbForwardVector * ((ForwardVectorComparison - 1) * -1);
			else
				NewForwardVector = ClimbForwardVector * (1 - ForwardVectorComparison);
			

			float RightVectorComparison = (UKismetMathLibrary::GetForwardVector(Controller->GetControlRotation()) - ClimbRightVector).Size() - 0.4;
			FVector NewRightVector;
			if (RightVectorComparison > 1.0)
				NewRightVector = ClimbRightVector * ((RightVectorComparison - 1) * -1);
			else
				NewRightVector = ClimbRightVector * (1 - RightVectorComparison);

			AddMovementInput((NewForwardVector + NewRightVector).GetSafeNormal(), Value);

			const FRotator YawRotation(0, ClimbRotation.Yaw, 0);
			const FVector ClimbDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
			//AddMovementInput(ClimbDirection, Value);
			//AddMovementInput(ClimbForwardVector, Value);
		}
		else
		{
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			AddMovementInput(Direction, Value);
		}
	}

	/*if (Controller && Value != 0.f && SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Bashing)
	{
		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator RevivePitch(Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0, 0);
			ArrowLineTrace->AddRelativeRotation(RevivePitch);

			if (ArrowLineTrace->GetRelativeRotation().Pitch >= 50)
				ArrowLineTrace->SetRelativeRotation(FRotator(50, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
			else if (ArrowLineTrace->GetRelativeRotation().Pitch <= -5)
				ArrowLineTrace->SetRelativeRotation(FRotator(-5, ArrowLineTrace->GetRelativeRotation().Yaw, 0));
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}*/
	
}

void ASpirit::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator ReviveYaw(0, Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0);
			ArrowLineTrace->AddRelativeRotation(ReviveYaw);

			if (ArrowLineTrace->GetRelativeRotation().Yaw >= 45)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, 45, 0));
				AddActorWorldRotation(ReviveYaw);
			}
			else if (ArrowLineTrace->GetRelativeRotation().Yaw <= -45)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, -45, 0));
				AddActorWorldRotation(ReviveYaw);
			}
		}
		else if (SpiritState == ESpiritState::Climbing)
		{
			UE_LOG(LogTemp, Warning, TEXT("RiVector: X: %f, Y: %f, Z: %f"), GetActorRightVector().X, GetActorRightVector().Y, GetActorRightVector().Z);

			FRotator WallRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
			//const FRotator YawRotation(0, ClimbRotation.Yaw, 0);
			//FVector ClimbDirection = FRotationMatrix(WallRotation - FRotator(0, 180, 0)).GetUnitAxis(EAxis::Y);
			//FVector ClimbDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			//AddMovementInput(ClimbDirection, Value);

			float ForwardVectorComparison = (UKismetMathLibrary::GetRightVector(Controller->GetControlRotation()) - ClimbForwardVector).Size() - 0.4;
			FVector NewForwardVector;
			if (ForwardVectorComparison > 1.0)
				NewForwardVector = ClimbForwardVector * ((ForwardVectorComparison - 1) * -1);
			else
				NewForwardVector = ClimbForwardVector * (1 - ForwardVectorComparison);


			float RightVectorComparison = (UKismetMathLibrary::GetRightVector(Controller->GetControlRotation()) - ClimbRightVector).Size() - 0.4;
			FVector NewRightVector;
			if (RightVectorComparison > 1.0)
				NewRightVector = ClimbRightVector * ((RightVectorComparison - 1) * -1);
			else
				NewRightVector = ClimbRightVector * (1 - RightVectorComparison);

			AddMovementInput((NewForwardVector + NewRightVector).GetSafeNormal(), Value);

			//AddMovementInput(ClimbRightVector, Value);

			const FRotator YawRotation(0, ClimbRotation.Yaw, 0);
			//UE_LOG(LogTemp, Warning, TEXT("YawRot: P: %f, Y: %f, R: %f"), YawRotation.Pitch, YawRotation.Yaw, YawRotation.Roll);
			const FVector ClimbDirection = FRotationMatrix(WallRotation - FRotator(0, 180, 0)).GetUnitAxis(EAxis::Y);
			UE_LOG(LogTemp, Warning, TEXT("ClimbDir: X: %f, Y: %f, Z: %f"), ClimbDirection.X, ClimbDirection.Y * Value, ClimbDirection.Z);
			//AddMovementInput(ClimbDirection, Value);
			//AddMovementInput()
		}
		else
		{
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
	}

	/*if (Controller && Value != 0.f && SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Bashing)
	{
		if (SpiritState == ESpiritState::Reviving)
		{
			const FRotator ReviveYaw(0, Value * BaseTurnRate * 2 * GetWorld()->GetDeltaSeconds(), 0);
			ArrowLineTrace->AddRelativeRotation(ReviveYaw);

			if (ArrowLineTrace->GetRelativeRotation().Yaw >= 45)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, 45, 0));
				AddActorWorldRotation(ReviveYaw);
			}
			else if (ArrowLineTrace->GetRelativeRotation().Yaw <= -45)
			{
				ArrowLineTrace->SetRelativeRotation(FRotator(ArrowLineTrace->GetRelativeRotation().Pitch, -45, 0));
				AddActorWorldRotation(ReviveYaw);
			}
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Value);
		}
	}*/
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
		SetState(ESpiritState::ChargingJump);
}

void ASpirit::Climb()
{
	if (TraceLine(ClimbTraceLength).GetActor() && TraceLine(ClimbTraceLength).GetActor()->ActorHasTag("Climbable"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
		AActor* HitActor = TraceLine(ClimbTraceLength).GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Hitted something"))

		BaseRotation = GetActorRotation();

		// Get wall hit location & rotation
		FVector WallLocation = TraceLine(ClimbTraceLength).Location;
		FVector WallNormal = TraceLine(ClimbTraceLength).ImpactNormal;

		// Get start location & rotation
		FVector TransitionStartLocation = GetActorLocation();
		FRotator TransitionStartRotation = GetActorRotation();

		// Get base of mesh location
		FVector MeshBaseLocation = GetMesh()->GetComponentLocation();

		FVector CrossProduct = FVector::CrossProduct((WallLocation - MeshBaseLocation).GetSafeNormal(), WallNormal) * -1;
		FVector WallForwardVector = FVector::CrossProduct(CrossProduct, WallNormal);
		FVector WallRightVector = FVector::CrossProduct(WallForwardVector, WallNormal);

		FVector TransitionEndLocation = WallLocation + (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * WallNormal);
		FRotator TransitionEndRotation = UKismetMathLibrary::MakeRotationFromAxes(WallForwardVector, WallRightVector, WallNormal);

		WallForwardVector = WallForwardVector.GetSafeNormal();
		WallRightVector = WallRightVector.GetSafeNormal();

		SetActorLocationAndRotation(TransitionEndLocation, TransitionEndRotation);
		ClimbConstantVelocityDirection = GetActorForwardVector();

		GetMesh()->Deactivate();
		GetMesh()->SetVisibility(false);
		SkeletalMeshWater->Activate();
		SkeletalMeshWater->SetVisibility(true);
		GetCharacterMovement()->bOrientRotationToMovement = false;

		ArrowLineTrace->SetWorldRotation((GetActorUpVector() * -1).GetSafeNormal().Rotation());
			//AddLocalRotation(FRotator(-90, 0, 0));

		TransitionToClimb();

		//UE_LOG(LogTemp, Warning, TEXT("WallFwVector: X: %f, Y: %f, Z: %f"), WallForwardVector.X, WallForwardVector.Y, WallForwardVector.Z);
		//UE_LOG(LogTemp, Warning, TEXT("FwVector: X: %f, Y: %f, Z: %f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
		//UE_LOG(LogTemp, Warning, TEXT("EndRot: P: %f, Y: %f, R: %f"), TransitionEndRotation.Pitch, TransitionEndRotation.Yaw, TransitionEndRotation.Roll);
		

		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		SetState(ESpiritState::Climbing);
	}

	//SphereClimb->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//UE_LOG(LogTemp, Warning, TEXT("Climb pressed"));
}

void ASpirit::StopClimb()
{
	if (SpiritState == ESpiritState::Climbing)
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
	}

	//SphereClimb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetRelativeRotation(FRotator(0, 0, 0));
	//UE_LOG(LogTemp, Warning, TEXT("Climb released"));
}

FHitResult ASpirit::TraceLine(float TraceLength)
{
	FHitResult Hit;
	//AActor* ActorHit = nullptr;

	FRotator LineRotation = ArrowLineTrace->GetComponentRotation();
	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FVector LineEnd = LineStart + (LineRotation.Vector() * TraceLength);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_WorldStatic, TraceParams);
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 0, 0, 2);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Orange, false, 2);
		
		//UE_LOG(LogTemp, Warning, TEXT("Crack Location: %f, %f"), Hit.GetComponent()->GetRelativeLocation().X, Hit.GetComponent()->GetRelativeLocation().Y);
		return Hit;
	}
	else return FHitResult();
}

void ASpirit::OnRevive_Implementation(AInteractablePlant* PlantHit)
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

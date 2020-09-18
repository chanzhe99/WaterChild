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
#include "DrawDebugHelpers.h"

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
	StaticMeshIce = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshIce"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	NiagaraFootsteps = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFootsteps"));
	NiagaraRevive = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRevive"));
	NiagaraIceTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraIceTrail"));
	NiagaraJumpDefault = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpDefault"));
	NiagaraJumpWater = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpWater"));
	NiagaraJumpIce = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraJumpIce"));
	ArrowLineTrace = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowLineTrace"));
	SphereClimb = CreateDefaultSubobject<USphereComponent>(TEXT("SphereClimb"));

	// Setup attachments for components
	SkeletalMeshWater->SetupAttachment(RootComponent);
	StaticMeshIce->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	NiagaraFootsteps->SetupAttachment(GetMesh());
	NiagaraRevive->SetupAttachment(ArrowLineTrace);
	NiagaraIceTrail->SetupAttachment(StaticMeshIce);
	NiagaraJumpDefault->SetupAttachment(GetMesh());
	NiagaraJumpWater->SetupAttachment(SkeletalMeshWater);
	NiagaraJumpIce->SetupAttachment(StaticMeshIce);
	ArrowLineTrace->SetupAttachment(RootComponent);
	SphereClimb->SetupAttachment(RootComponent);

	// Set component position offsets
	GetMesh()->SetRelativeLocation(FVector(0, 0, -27));
	SkeletalMeshWater->SetRelativeLocation(FVector(-14, 0, -27));
	StaticMeshIce->SetRelativeLocation(FVector(0, 0, -13));

	SpringArm->SetRelativeLocation(FVector(-5, 0, 0));

	NiagaraRevive->SetRelativeRotation(FRotator(0, 270, 0));
	NiagaraIceTrail->SetRelativeLocationAndRotation(FVector(-19, 0, -5), FRotator(15, 180.f, 0));

	// Set character mesh collision profiles
	SkeletalMeshWater->SetCollisionProfileName(TEXT("CharacterMesh"));
	StaticMeshIce->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Set component default values
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	NiagaraFootsteps->SetAutoActivate(false);
	NiagaraRevive->SetAutoActivate(false);
	NiagaraIceTrail->SetAutoActivate(false);
	NiagaraJumpDefault->SetAutoActivate(false);
	NiagaraJumpWater->SetAutoActivate(false);
	NiagaraJumpIce->SetAutoActivate(false);

	GetCapsuleComponent()->SetCapsuleRadius(12.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(25.f);
	SphereClimb->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set default variable values
	SpiritState = ESpiritState::Idle;

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;

	ReviveTraceLength = 200;
	SqueezeTraceLength = 10;

	JumpChargeDuration = 0.1f;
	JumpChargeTime = 0;

	BashDistanceDefault = 300;
	BashDuration = 0.15f;
	BashTime = 0;
	CanBash = true;
	BashLocationStart = FVector().ZeroVector;
	BashLocationEnd = FVector().ZeroVector;

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
	SphereClimb->OnComponentBeginOverlap.AddDynamic(this, &ASpirit::OnOverlapBegin);
	SphereClimb->OnComponentEndOverlap.AddDynamic(this, &ASpirit::OnOverlapEnd);
}

// Called every frame
void ASpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, CharacterHeight));
	//SetActorWorldOffset(FVector(0, 0, CharacterHeight));
	//if(TraceLine() != nullptr)
		//UE_LOG(LogTemp, Warning, TEXT("Hit Name: %s"), *TraceLine()->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Height: %f"), CharacterHeight);

	if (!CanBash) TickBashCooldown(DeltaTime);

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
			SetState(ESpiritState::Idle);
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
	case ESpiritState::Bashing:
		OnBash();
		break;
	case ESpiritState::Climbing:
		FRotator Rotation = Controller->GetControlRotation();
		FRotator WallRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
		FRotator PitchRotation = FRotator().ZeroRotator - FRotator(WallRotation.Pitch, 0, 0);
		FRotator YawRotation = WallRotation - FRotator(0, 180, 0);
		FRotator ClimbRotation = FRotator(PitchRotation.Pitch, YawRotation.Yaw, Rotation.Roll);
		SetActorRotation(ClimbRotation);

		if (!TraceLine(ClimbTraceLength).GetActor())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			SetState(ESpiritState::Falling);
		}

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
		
		if (SpiritState == ESpiritState::Climbing)
		{
			const FRotator RollRotation(0, 0, Rotation.Roll);
			FVector ClimbDirection = FRotationMatrix(RollRotation).GetUnitAxis(EAxis::Z);
			AddMovementInput(ClimbDirection, Value);
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
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (SpiritState == ESpiritState::Climbing)
		{
			FRotator WallRotation = TraceLine(ClimbTraceLength).ImpactNormal.Rotation();
			const FRotator RollRotation(0, 0, Rotation.Roll);
			FVector ClimbDirection = FRotationMatrix(WallRotation - FRotator(0, 180, 0)).GetUnitAxis(EAxis::Y);
			AddMovementInput(ClimbDirection, Value);
		}
		else
		{
			//const FRotator YawRotation(0, Rotation.Yaw, 0);

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
	if (SpiritState != ESpiritState::Falling)
	{
	}
}

void ASpirit::StopAction()
{
	if (SpiritState != ESpiritState::Falling)
	{
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
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		SetState(ESpiritState::Falling);
	}

	//SphereClimb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetRelativeRotation(FRotator(0, 0, 0));
	//UE_LOG(LogTemp, Warning, TEXT("Climb released"));
}

void ASpirit::TickBashCooldown(float DeltaTime)
{
	float BashCooldown = 1.0f;
	static float BashCooldownDuration;

	BashCooldownDuration += DeltaTime;

	if (BashCooldownDuration >= BashCooldown)
	{
		CanBash = true;
		BashCooldownDuration = 0;
	}

	return;
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

float ASpirit::TraceLineDistance(float TraceLength)
{
	FHitResult Hit;
	AActor* ActorHit = nullptr;

	FRotator LineRotation = ArrowLineTrace->GetComponentRotation();
	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FVector LineEnd = LineStart + (LineRotation.Vector() * TraceLength);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_WorldStatic, TraceParams);
	//DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 0, 0, 2);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Orange, false, 2);
		if (Hit.GetActor() && !Hit.GetActor()->IsA(AInteractable::StaticClass()))
			return Hit.Distance;
		else return BashDistanceDefault;
	}
	else return BashDistanceDefault;
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

void ASpirit::OnBash_Implementation()
{
	CanBash = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	NiagaraIceTrail->Activate(true);

	if (BashTime < BashDuration)
	{
		SetActorRelativeLocation(FMath::Lerp(BashLocationStart, BashLocationEnd, BashTime / BashDuration));
		BashTime += GetWorld()->GetDeltaSeconds();
	}
	else
	{
		BashTime = 0;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
		NiagaraIceTrail->Deactivate();
		SetState(ESpiritState::Idle);
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



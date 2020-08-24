// Fill out your copyright notice in the Description page of Project Settings.


#include "Spirit.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
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
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->MaxWalkSpeed = 300;
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
	ArrowLineTrace = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowLineTrace"));

	// Setup attachments for components
	SkeletalMeshWater->SetupAttachment(RootComponent);
	StaticMeshIce->SetupAttachment(RootComponent);
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	NiagaraFootsteps->SetupAttachment(GetMesh());
	NiagaraRevive->SetupAttachment(GetMesh());
	NiagaraIceTrail->SetupAttachment(StaticMeshIce);
	ArrowLineTrace->SetupAttachment(RootComponent);

	// Set mesh position offsets
	GetMesh()->SetRelativeLocation(FVector(0, 0, -27));
	SkeletalMeshWater->SetRelativeLocation(FVector(-8, 0, -27));
	StaticMeshIce->SetRelativeLocation(FVector(0, 0, -13));

	// Set character mesh collision profiles
	SkeletalMeshWater->SetCollisionProfileName(TEXT("CharacterMesh"));
	StaticMeshIce->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Set spring arm properties
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	// Set Niagara system properties
	NiagaraRevive->SetRelativeLocationAndRotation(FVector(0, 0, 25), FRotator(0, 270, 0));
	NiagaraIceTrail->SetRelativeLocationAndRotation(FVector(-19, 0, -5), FRotator(15, 180.f, 0));
	NiagaraFootsteps->SetAutoActivate(false);
	NiagaraRevive->SetAutoActivate(false);
	NiagaraIceTrail->SetAutoActivate(false);

	// Set capsule base height
	GetCapsuleComponent()->SetCapsuleRadius(12.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(25.f);

	// Set spirit form to default
	SetForm(ESpiritForm::Default);

	BaseTurnRate = 45.f;
	BaseLookUpAtRate = 45.f;
}

// Called when the game starts or when spawned
void ASpirit::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASpirit::OnOverlapBegin);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	UE_LOG(LogTemp, Warning, TEXT("C++ Begin ran"));
}

// Called every frame
void ASpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if(TraceLine() != nullptr)
		//UE_LOG(LogTemp, Warning, TEXT("Hit Name: %s"), *TraceLine()->GetName());

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

		if (SpiritForm == ESpiritForm::Water)
			if (TraceLine(SqueezeTraceLength) && TraceLine(SqueezeTraceLength)->IsA(AInteractableCrack::StaticClass()))
			{
				SelectedCrack = Cast<AInteractableCrack>(TraceLine(SqueezeTraceLength));
				SetState(ESpiritState::Squeezing);
			}
		break;
	case ESpiritState::Falling:
		if (!GetCharacterMovement()->IsFalling())
			SetState(ESpiritState::Idle);
		break;
	case ESpiritState::Reviving:
		UE_LOG(LogTemp, Warning, TEXT("Reviving"));
		OnRevive(Cast<AInteractablePlant>(TraceLine(ReviveTraceLength)));
		break;
	case ESpiritState::Squeezing:
		if(SelectedCrack)
			OnSqueeze(SelectedCrack);
		break;
	case ESpiritState::Bashing:
		OnBash();
		break;
	}
}

// Called to bind functionality to input
void ASpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds the action buttons
	PlayerInputComponent->BindAction<SetFormDelegate>("DefaultFormButton", IE_Pressed, this, &ASpirit::SetForm, ESpiritForm::Default);
	PlayerInputComponent->BindAction<SetFormDelegate>("WaterFormButton", IE_Pressed, this, &ASpirit::SetForm, ESpiritForm::Water);
	PlayerInputComponent->BindAction<SetFormDelegate>("IceFormButton", IE_Pressed, this, &ASpirit::SetForm, ESpiritForm::Ice);
	PlayerInputComponent->BindAction("ActionButton", IE_Pressed, this, &ASpirit::Action);
	PlayerInputComponent->BindAction("ActionButton", IE_Released, this, &ASpirit::StopAction);

	// Binds the movement axes
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpirit::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpirit::MoveRight);

	// Binds the camera control axes
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpirit::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpirit::LookUpAtRate);

}

// Changes the Spirit's form according to input parameter
void ASpirit::SetForm(ESpiritForm DesiredForm)
{
	SpiritForm = DesiredForm;

	switch (SpiritForm)
	{
	case ESpiritForm::Default:
		GetMesh()->Activate();
		SkeletalMeshWater->Deactivate();
		StaticMeshIce->Deactivate();
		GetMesh()->SetVisibility(true);
		SkeletalMeshWater->SetVisibility(false);
		StaticMeshIce->SetVisibility(false);
		break;

	case ESpiritForm::Water:
		GetMesh()->Deactivate();
		SkeletalMeshWater->Activate();
		StaticMeshIce->Deactivate();
		GetMesh()->SetVisibility(false);
		SkeletalMeshWater->SetVisibility(true);
		StaticMeshIce->SetVisibility(false);
		if (SpiritState == ESpiritState::Reviving) SetState(ESpiritState::Idle);
		if (NiagaraRevive->IsActive()) NiagaraRevive->Deactivate();
		break;

	case ESpiritForm::Ice:
		GetMesh()->Deactivate();
		SkeletalMeshWater->Deactivate();
		StaticMeshIce->Activate();
		GetMesh()->SetVisibility(false);
		SkeletalMeshWater->SetVisibility(false);
		StaticMeshIce->SetVisibility(true);
		if (SpiritState == ESpiritState::Reviving) SetState(ESpiritState::Idle);
		if (NiagaraRevive->IsActive()) NiagaraRevive->Deactivate();
		break;
	}
}

void ASpirit::MoveForward(float Value)
{
	if (SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Bashing && Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASpirit::MoveRight(float Value)
{
	if (SpiritState != ESpiritState::Squeezing && SpiritState != ESpiritState::Bashing && Controller && Value != 0.f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ASpirit::Action()
{
	if (SpiritState != ESpiritState::Falling)
	{
		switch (SpiritForm)
		{
		case ESpiritForm::Default:
			SetState(ESpiritState::Reviving);
			NiagaraRevive->Activate();
			break;
		case ESpiritForm::Water:
			if(SpiritState == ESpiritState::Idle || SpiritState == ESpiritState::Walking)
				OnJump();
			break;
		case ESpiritForm::Ice:
			if (CanBash) //OnBash();
				SetState(ESpiritState::Bashing);
			break;
		}
	}
}

void ASpirit::StopAction()
{
	if (SpiritState != ESpiritState::Falling)
	{
		switch (SpiritForm)
		{
		case ESpiritForm::Default:
			if (SelectedPlant)
			{
				IInteractableInterface* Interface = Cast<IInteractableInterface>(SelectedPlant);
				if (Interface) Interface->Execute_OnInteractEnd(SelectedPlant, this);
			}
			SelectedPlant = nullptr;
			SetState(ESpiritState::Idle);
			NiagaraRevive->Deactivate();
			break;
		case ESpiritForm::Water:
			break;
		case ESpiritForm::Ice:
			break;
		}
	}
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

AActor* ASpirit::TraceLine(float TraceLength)
{
	FHitResult Hit;
	AActor* ActorHit = nullptr;

	FRotator LineRotation = ArrowLineTrace->GetComponentRotation();
	FVector LineStart = ArrowLineTrace->GetComponentLocation();
	FVector LineEnd = LineStart + (LineRotation.Vector() * TraceLength);

	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ECC_Destructible, TraceParams);
	//DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, 0, 0, 2);

	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Orange, false, 2);
		
		//UE_LOG(LogTemp, Warning, TEXT("Crack Location: %f, %f"), Hit.GetComponent()->GetRelativeLocation().X, Hit.GetComponent()->GetRelativeLocation().Y);

		if (Hit.GetComponent()->GetRelativeLocation() == FVector::ZeroVector)
			bIsCrackEntrance = true;
		else bIsCrackEntrance = false;

		if (Hit.GetActor()->IsA(AInteractable::StaticClass()))
			return ActorHit = Hit.GetActor();
		else return ActorHit = nullptr;
	}
	else return ActorHit = nullptr;
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
	float BashDuration = 0.15f;

	CanBash = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	FRotator SpiritRotation = GetCapsuleComponent()->GetComponentRotation();
	const FVector Direction = FRotationMatrix(SpiritRotation).GetUnitAxis(EAxis::X);

	if (BashTime < BashDuration)
	{
		AddActorWorldOffset(Direction * BashDistance * GetWorld()->GetDeltaSeconds());
		BashTime += GetWorld()->GetDeltaSeconds();
	}
	else
	{
		BashTime = 0;
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
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
}

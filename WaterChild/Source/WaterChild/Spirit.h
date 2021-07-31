// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spirit.generated.h"

UENUM(BlueprintType)
enum class ESpiritState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Walking			UMETA(DisplayName = "Walking"),
	Falling			UMETA(DisplayName = "Falling"),
	Reviving		UMETA(DisplayName = "Reviving"),
	ChargingJump	UMETA(DisplayName = "ChargingJump"),
	Squeezing		UMETA(DisplayName = "Squeezing"),
	Climbing		UMETA(DisplayName = "Climbing"),
	StuckInPlace	UMETA(DisplayName = "StuckInPlace"),
	WalkingBack		UMETA(DisplayName = "WalkingBack")
};

UCLASS()
class WATERCHILD_API ASpirit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpirit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Changes the Spirit's state and form according to input parameter
	UFUNCTION(BlueprintCallable)
	void SetState(ESpiritState DesiredState) { SpiritState = DesiredState; }
	ESpiritState GetState() { return SpiritState; }
	UFUNCTION(BlueprintCallable)
	bool GetCanTakeInput() { return bCanTakeInput; }
	UFUNCTION(BlueprintCallable)
	void SetCanTakeInput(bool Value) { bCanTakeInput = Value; }
	UFUNCTION(BlueprintCallable)
	void SetCanTransitionToClimb(bool CanTransition) { bCanTransitionToClimb = CanTransition; }
	class USpringArmComponent* GetSpringArm() {return SpringArm;}

#pragma region Input functions
	// Action Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void Revive();
	void Revive_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void StopRevive();
	void StopRevive_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void Climb();
	void Climb_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void StopClimb();
	void StopClimb_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void JumpAction();
	void JumpAction_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void LookAtAction();
	void LookAtAction_Implementation() {};
	
	// Revive Axis Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void ReviveAxis(float Value);
	void ReviveAxis_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void StopReviveAxis(float Value);
	void StopReviveAxis_Implementation(float Value);
	
	// Axis Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void MoveForward(float Value);
	void MoveForward_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void MoveRight(float Value);
	void MoveRight_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void TurnAt(float Value);
	void TurnAt_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void LookUpAt(float Value);
	void LookUpAt_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void TurnAtRate(float Value);
	void TurnAtRate_Implementation(float Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void LookUpAtRate(float Value);
	void LookUpAtRate_Implementation(float Value);
#pragma endregion

#pragma region Climb functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void TransitionToClimb();
	void TransitionToClimb_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void SetClimbOrientation();
	void SetClimbOrientation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void TransitionToFall();
	void TransitionToFall_Implementation() {}
#pragma endregion

#pragma region Squeeze functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void TransitionToSqueeze();
	void TransitionToSqueeze_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void TransitionToStopSqueeze();
	void TransitionToStopSqueeze_Implementation() {};
#pragma endregion

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnRevive(class AInteractable* PlantHit);
	void OnRevive_Implementation(class AInteractable* PlantHit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnJump();
	void OnJump_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnSqueeze(class AInteractableCrack* CrackHit);
	void OnSqueeze_Implementation(class AInteractableCrack* CrackHit);

private:
#pragma region Spirit components
	// Spirit water and ice mesh components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshWater;

	// Spirit spring arm and camera components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowLineTrace;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ReviveArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Footsteps", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowLeftFoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Footsteps", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowRightFoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraFootsteps;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraRevive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraJumpDefault;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraJumpWater;

#pragma endregion

#pragma region Component variables
	bool bCanTakeInput = false;
	bool bIsUsingGamepad = false;
	bool bIsTurningBehindWhenReviving = false;
	float SidewaysInputValue = 0.f;
	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESpiritState SpiritState = ESpiritState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	float BaseLookUpAtRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	FHitResult TraceHit = FHitResult();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	float ClimbTraceLength = 30;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climbing", meta = (AllowPrivateAccess = "true"));
	AActor* WallBeingClimbed = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bCameraFollowRevive = false;
	bool bActorSetToControllerDirection = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultSpringArmLength = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float RevivingSpringArmLength = 25.f;
	float TargetSpringArmLength = DefaultSpringArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FVector DefaultSocketOffset = FVector(0, 0, 60);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FVector RevivingSocketOffset = FVector(0, 25, 0);
	FVector TargetSocketOffset = DefaultSocketOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	float ReviveTraceLength = 300;
	float ReviveMaxHeight = 75;
	float ReviveMinHeight = -50;
	float ReviveMaxYawAngle = 45;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Revive", meta = (AllowPrivateAccess = "true"));
	float ReviveForceMultiplier = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	float SqueezeTraceLength = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"));
	float JumpChargeDuration;
	float JumpChargeTime;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsCrackEntrance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class AInteractable* SelectedPlant;
	class AInteractableCrack* SelectedCrack;

	bool bIsClimbButtonDown = false;
	bool bIsCheckingForClimbable = false;
	bool bCanTransitionToClimb = true;
	AActor* SelectedClimbable;
	FRotator BaseRotation = FRotator::ZeroRotator;
	FVector WallNormal = FVector::ZeroVector;
	FVector ClimbConstantVelocityDirection = FVector::ZeroVector;
	FVector ClimbForwardVector = FVector::ZeroVector;
	FVector ClimbRightVector = FVector::ZeroVector;
	FRotator ClimbRotation = FRotator::ZeroRotator;

	// Look At vars
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Look At", meta = (AllowPrivateAccess = "true"))
	bool IsInLookAtArea = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Look At", meta = (AllowPrivateAccess = "true"))
	FVector LookAtVector = FVector::ZeroVector;*/

#pragma endregion

	void CheckInputType(FKey Key)	{ bIsUsingGamepad = (Key.IsMouseButton() || !Key.IsGamepadKey()) ? false : true; }
	
	FHitResult TraceLine(float TraceLength);
	FHitResult ClimbTraceLine(FVector2D Direction);
	FHitResult ReviveTraceLine(float TraceLength);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

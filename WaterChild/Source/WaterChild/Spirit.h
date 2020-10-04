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
	Climbing		UMETA(DisplayName = "Climbing")
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Changes the Spirit's state and form according to input parameter
	UFUNCTION(BlueprintCallable)
	void SetState(ESpiritState DesiredState) { SpiritState = DesiredState; }
	ESpiritState GetState() { return SpiritState; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void TransitionToClimb();
	void TransitionToClimb_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnRevive(class APlant* PlantHit);
	void OnRevive_Implementation(class APlant* PlantHit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnJump();
	void OnJump_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void OnSqueeze(class AInteractableCrack* CrackHit);
	void OnSqueeze_Implementation(class AInteractableCrack* CrackHit);

	bool GetCrackEntrance() { return bIsCrackEntrance; }
	class USpringArmComponent* GetSpringArm() const { return SpringArm; }

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraFootsteps;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraRevive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraJumpDefault;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UNiagaraComponent* NiagaraJumpWater;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowLineTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ClimbLineTraceUp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ClimbLineTraceLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ClimbLineTraceRight;
#pragma endregion

#pragma region Component variables
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	float ReviveTraceLength = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Line Trace", meta = (AllowPrivateAccess = "true"));
	float SqueezeTraceLength = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"));
	float JumpChargeDuration;
	float JumpChargeTime;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsCrackEntrance;
	class APlant* SelectedPlant;
	class AInteractableCrack* SelectedCrack;

	bool bIsCheckingForClimbable = false;
	AActor* SelectedClimbable;
	FRotator BaseRotation = FRotator::ZeroRotator;
	FVector WallNormal = FVector::ZeroVector;
	FVector ClimbConstantVelocityDirection = FVector::ZeroVector;
	FVector ClimbForwardVector = FVector::ZeroVector;
	FVector ClimbRightVector = FVector::ZeroVector;
	FRotator ClimbRotation = FRotator::ZeroRotator;

#pragma endregion

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAt(float Value)		{ AddControllerYawInput(Value); }
	void LookUpAt(float Value)		{ AddControllerPitchInput(Value); }
	void TurnAtRate(float Value)	{ AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds()); }
	void LookUpAtRate(float Value)	{ AddControllerPitchInput(Value * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds()); }
	void Action();
	void StopAction();
	void Jump();
	void Climb();
	void StopClimb();
	FHitResult TraceLine(float TraceLength);
	FHitResult ClimbTraceLine(FVector2D Direction);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

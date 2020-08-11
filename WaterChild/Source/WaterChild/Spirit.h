// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spirit.generated.h"

UENUM(BlueprintType)
enum class ESpiritState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Walking		UMETA(DisplayName = "Walking"),
	Falling		UMETA(DisplayName = "Falling"),
	Squeezing	UMETA(DisplayName = "Squeezing"),
	Bashing		UMETA(DisplayName = "Bashing"),
};

UENUM(BlueprintType)
enum class ESpiritForm : uint8
{
	Default		UMETA(DisplayName = "Default Form"),
	Water		UMETA(DisplayName = "Water Form"),
	Ice			UMETA(DisplayName = "Ice Form")
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
	void SetForm(ESpiritForm DesiredForm);
	DECLARE_DELEGATE_OneParam(SetFormDelegate, ESpiritForm);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void OnRevive(AActor* ActorHit);
	void OnRevive_Implementation(AActor* ActorHit);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void OnJump();
	void OnJump_Implementation() {}
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void OnBash();
	void OnBash_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
		void OnSqueeze(AActor* ActorHit);
	void OnSqueeze_Implementation(AActor* ActorHit);

	bool GetCrackEntrance() { return bIsCrackEntrance; }

private:
	// Spirit water and ice mesh components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMeshWater;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* StaticMeshIce;

	// Spirit spring arm and camera components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		ESpiritState SpiritState = ESpiritState::Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		ESpiritForm SpiritForm = ESpiritForm::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	float BaseLookUpAtRate;

	bool CanBash = true;
	float BashDistance = 2000.f;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsCrackEntrance;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value) { AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds()); }
	void LookUpAtRate(float Value) { AddControllerPitchInput(Value * BaseLookUpAtRate * GetWorld()->GetDeltaSeconds()); }
	void Action();
	void TickBashCooldown(float DeltaTime);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

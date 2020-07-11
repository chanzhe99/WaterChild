// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spirit.generated.h"


UCLASS()
class WATERCHILD_API ASpirit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpirit();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision");
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* DefaultMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* IceMesh;

	enum ESpiritForm
	{
		Default,
		Water,
		Ice
	};

	enum ESpiritState
	{
		Idle,
		Walking,
		Jumping,
		Bashing,
		Interacting
	};

	bool bWithinReviveRadius = false;
	bool bIsCrackEntrance = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseLookUpAtRate;

	ESpiritForm SpiritForm;
	ESpiritState SpiritState;
	FHitResult HitResult;
	AActor* ActivatedPlate = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LineTrace Settings")
		float TraceForwardLength = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LineTrace Settings")
		float TraceBelowLength = 65.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bash Settings")
		float BashDistance = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bash Settings")
		float BashDuration = 0.15f;
	float BashTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bash Settings")
		float BashCooldown = 1.0f;
	float BashCooldownDuration = BashCooldown;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void Revive();
	void Jump();
	void Bash();
	void Action();

	UFUNCTION(BlueprintNativeEvent)
		void TraceForward();
	void TraceForward_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void TraceBelow();
	void TraceBelow_Implementation();

	void CallTraverseFromCrack();
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void DeactivatePlate();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	DECLARE_DELEGATE_OneParam(SetFormDelegate, ESpiritForm);
	void SetForm(ESpiritForm CurrentForm);
	void SetState(ESpiritState CurrentState);
};

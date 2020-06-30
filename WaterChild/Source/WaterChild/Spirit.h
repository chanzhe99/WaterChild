// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Spirit.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class WATERCHILD_API ASpirit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpirit();

	//UENUM()
	enum ESpiritState
	{
		Default,
		Water,
		Ice,
		Jumping,
		Bashing
	};

	ESpiritState SpiritState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* DefaultMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* IceMesh;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void Revive();
	void Jump();
	void Bash();
	void Morph(ESpiritState CurrentState);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera");
	float BaseLookUpAtRate;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

};

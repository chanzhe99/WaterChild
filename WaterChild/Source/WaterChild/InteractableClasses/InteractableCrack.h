// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractableCrack.generated.h"


UCLASS()
class WATERCHILD_API AInteractableCrack : public AInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableCrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;

private:
	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"));
	UStaticMeshComponent* CrackMesh1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"));
	UStaticMeshComponent* CrackMesh2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit", meta = (AllowPrivateAccess = "true"));
	USceneComponent* CrackExit1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit", meta = (AllowPrivateAccess = "true"));
	USceneComponent* CrackExit2;

	const FTransform* Crack1 = nullptr;
	const FTransform* Crack2 = nullptr;
	float TraversalPercentage = 0;

	void TraverseCrack(ASpirit* Caller, const FTransform* EntranceLocation, const FTransform* ExitLocation);

};

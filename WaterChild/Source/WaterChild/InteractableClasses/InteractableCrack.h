// Copyright SkyJus Works. All Rights Reserved.

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

	USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* CrackMesh1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* CrackMesh2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision");
	class UBoxComponent* CrackCollider1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision");
	class UBoxComponent* CrackCollider2;

	const FTransform* Crack1 = nullptr;
	const FTransform* Crack2 = nullptr;
	FVector Crack1Location = FVector::ZeroVector;
	FVector Crack2Location = FVector::ZeroVector;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void TraverseCrack(ASpirit* Caller, const FTransform* EntranceLocation, const FTransform* ExitLocation);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;
};

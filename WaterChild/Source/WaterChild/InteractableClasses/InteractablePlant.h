// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractablePlant.generated.h"


UCLASS()
class WATERCHILD_API AInteractablePlant : public AInteractable
{
	GENERATED_BODY()
	
public:
	AInteractablePlant();

	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	class USkeletalMeshComponent* StemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	class USkeletalMeshComponent* PetalMesh;

	UPROPERTY(EditAnywhere)
	bool bPlantIsGrown = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings");
	float GrowthHeight = 300.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;
	virtual void OnInteractEnd_Implementation(ASpirit* Caller) override;

};

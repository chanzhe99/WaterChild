// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractablePlant.generated.h"

UENUM(BlueprintType)
enum class EPlantState : uint8
{
	Dead		UMETA(DisplayName = "Dead"),
	Reviving	UMETA(DisplayName = "Reviving"),
	Revived		UMETA(DisplayName = "Revived"),
};

UCLASS()
class WATERCHILD_API AInteractablePlant : public AInteractable
{
	GENERATED_BODY()
	
public:
	AInteractablePlant();

	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	class UStaticMeshComponent* FloorPlane;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	class USkeletalMeshComponent* StemMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	class USkeletalMeshComponent* PetalMesh;

	EPlantState PlantState = EPlantState::Dead;
	EPlantState GetPlantState() { return PlantState; }
	void SetPlantState(EPlantState DesiredState) { PlantState = DesiredState; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Revive Settings")
	float DefaultPlantHeight = -372;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Revive Settings")
	float RevivedPlantHeight = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Revive Settings")
	float ReviveSpeed = 100;

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

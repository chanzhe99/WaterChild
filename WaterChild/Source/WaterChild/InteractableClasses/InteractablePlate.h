// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InteractablePlate.generated.h"


UCLASS()
class WATERCHILD_API AInteractablePlate : public AInteractable
{
	GENERATED_BODY()

public:
	AInteractablePlate();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
	UStaticMeshComponent* PlateMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsSpiritOnPlate;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;
	virtual void OnInteractEnd_Implementation(ASpirit* Caller) override;
};

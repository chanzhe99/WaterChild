// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "Grass.generated.h"

UCLASS()
class WATERCHILD_API AGrass : public APlant
{
	GENERATED_BODY()

public:
	AGrass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/*virtual void OnInteract_Implementation(ASpirit* Caller) override;
	virtual void OnInteractEnd_Implementation(ASpirit* Caller) override;*/

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* GrassMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* MultiCollider;
};
// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "BridgePlant.generated.h"

UCLASS()
class WATERCHILD_API ABridgePlant : public APlant
{
	GENERATED_BODY()
	
public:
	ABridgePlant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;
	virtual void OnInteractEnd_Implementation(ASpirit* Caller) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FloorPlane;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* BridgeMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BridgeCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ReviveCollider;
};

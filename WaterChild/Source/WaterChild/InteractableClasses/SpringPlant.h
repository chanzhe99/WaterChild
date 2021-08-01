// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Plant.h"
#include "SpringPlant.generated.h"

UCLASS()
class WATERCHILD_API ASpringPlant : public APlant
{
	GENERATED_BODY()
	
public:
	ASpringPlant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpringPlant Features")
	void BounceAnimation(ASpirit* Caller);
	virtual void BounceAnimation_Implementation(ASpirit* Caller);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FlowerMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* FlowerCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ReviveCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scene Component", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeedsComp;

	UPROPERTY(EditAnywhere, Category = "SpringPlant Variables", meta = (AllowPrivateAccess = "true"))
	float BounceForwardVelocity = 0.;
	float BounceSideVelocity = 0.;
	UPROPERTY(EditAnywhere, Category = "SpringPlant Variables", meta = (AllowPrivateAccess = "true"))
	float BounceUpVelocity = 1000.;
};

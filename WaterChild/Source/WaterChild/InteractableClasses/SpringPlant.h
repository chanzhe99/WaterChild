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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnInteract_Implementation(ASpirit* Caller) override;
	virtual void OnInteractEnd_Implementation(ASpirit* Caller) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spring Plant Features")
	void BounceAnimation(ASpirit* Caller);
	virtual void BounceAnimation_Implementation(ASpirit* Caller);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FloorPlane;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FlowerMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* FlowerCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ReviveCollider;

	UPROPERTY(EditAnywhere, Category = "Plant variables", meta = (AllowPrivateAccess = "true"))
	float BounceForwardVelocity = 0.;
	float BounceSideVelocity = 0.;
	float BounceUpVelocity = 1000.;
};

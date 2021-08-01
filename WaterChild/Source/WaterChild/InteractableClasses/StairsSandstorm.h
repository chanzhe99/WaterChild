// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StairsSandstorm.generated.h"

class UBoxComponent;
class UArrowComponent;
class UNiagaraComponent;

UCLASS()
class WATERCHILD_API AStairsSandstorm : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AStairsSandstorm();

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DoBoxTrace();
	float CalculateSandstormStrength();

	// Main sandstorm components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBillboardComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SandstormSizeVisualiser; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* SandstormStartingParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* SandstormActiveParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBillboardComponent* SandstormLookAtTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* WindFX_6;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector SandstormCameraOffset = FVector(0, 40, 20);
	float PlayerSpringArmLength_Default = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float PlayerSpringArmLength_Sandstorm = 100.f;
	float PlayerSpringArmLength_Difference = PlayerSpringArmLength_Default - PlayerSpringArmLength_Sandstorm;
	
	// BoxTrace variables
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> PlayerObjectList;
	UPROPERTY()
    TArray<AActor*> ObjectsToIgnoreList;
	UPROPERTY()
	FHitResult SandstormHitResult;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	bool IsHitPlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	bool IsSandstormOn = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	class ASpirit* SpiritReference;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	TArray<UNiagaraComponent*> WindFXList;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerSandstormWalkAnimVar = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Sandstorm = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Default = 300.f;
	float PlayerWalkSpeed_Difference = PlayerWalkSpeed_Default - PlayerWalkSpeed_Sandstorm;
};

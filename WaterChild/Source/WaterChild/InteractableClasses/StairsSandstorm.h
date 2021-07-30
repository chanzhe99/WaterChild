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

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UArrowComponent* PushBackDirection;*/
	// Sandstorm safe spots components
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SafeSpot6;*/
	
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
	float PlayerSandstormWalkAnimVar = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Sandstorm = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Default = 300.f;
	float PlayerWalkSpeed_Difference = PlayerWalkSpeed_Default - PlayerWalkSpeed_Sandstorm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerSpringArmLength_Default = 300.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerSpringArmLength_Sandstorm = 100.f;
	float PlayerSpringArmLength_Difference = PlayerSpringArmLength_Default - PlayerSpringArmLength_Sandstorm;
};

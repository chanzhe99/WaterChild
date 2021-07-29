// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StairsSandstorm.generated.h"

UENUM(BlueprintType)
enum class ESandstormState : uint8
{
	Inactive = 0		UMETA(DisplayName = "Sandstorm Inactive"),
	Starting			UMETA(DisplayName = "Sandstorm Starting"),
	Active				UMETA(DisplayName = "Sandstorm Active")
};

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

	void CycleSandstormState(float &TickValue, float TargetValue, float IncrementRate, ESandstormState TargetSandstormState);
	bool DoBoxTrace();
	bool CheckSafeSpots();
	float CalculateSandstormStrength();

	// Main sandstorm components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBillboardComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBoxComponent* SandstormSizeVisualiser; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UArrowComponent* PushBackDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* SandstormStartingParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UNiagaraComponent* SandstormActiveParticleSystem;

	// Sandstorm safe spots components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
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
	UBoxComponent* SafeSpot6;
	
	// BoxTrace variables
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> PlayerObjectList;
	UPROPERTY()
    TArray<AActor*> ObjectsToIgnoreList;
	UPROPERTY()
	FHitResult SandstormHitResult;
	UPROPERTY()
	FHitResult SafeSpotHitResult;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	bool IsHitPlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	class ASpirit* SpiritReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerSandstormWalkAnimVar = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Sandstorm = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Default = 300.f;
	float PlayerWalkSpeed_Difference = PlayerWalkSpeed_Default - PlayerWalkSpeed_Sandstorm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sandstorm Values")
	ESandstormState SandstormState = ESandstormState::Inactive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Values")
	float PushBackForceMultiplier = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormInterval = 3.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormInterval_Tick = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormStartUpTime = 1.f; // Time it takes for Sandstorm Intensity to reach maximum
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormStartUpTime_Tick = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormDuration = 5.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormDuration_Tick = 0.f;
};

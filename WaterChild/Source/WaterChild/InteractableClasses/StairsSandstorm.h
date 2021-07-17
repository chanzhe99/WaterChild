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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	UBillboardComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	class UArrowComponent* PushBackDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	class UBoxComponent* SandstormSizeVisualiser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	class UNiagaraComponent* SandstormStartingParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Components")
	class UNiagaraComponent* SandstormActiveParticleSystem;
	
	// BoxTrace variables
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> PlayerObjectList;
	UPROPERTY()
    TArray<AActor*> ObjectsToIgnoreList;
	UPROPERTY()
	FHitResult HitResult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	bool IsHitPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	class ASpirit* SpiritReference;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	//float SandstormIntensity = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerSandstormWalkAnimVar = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Sandstorm = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Reference")
	float PlayerWalkSpeed_Default = 300.f;
	float PlayerWalkSpeed_Difference = PlayerWalkSpeed_Default - PlayerWalkSpeed_Sandstorm;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Values")
	ESandstormState SandstormState = ESandstormState::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormInterval = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormInterval_Tick = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormStartUpTime = 1.f; // Time it takes for Sandstorm Intensity to reach maximum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormStartUpTime_Tick = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormDuration = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sandstorm Time Values")
	float SandstormDuration_Tick = 0.f;

};

// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Plant.generated.h"

UENUM(BlueprintType)
enum class EPlantState : uint8
{
    Dead        UMETA(DisplayName = "Dead"),
    Growing     UMETA(DisplayName = "Growing"),
    Alive       UMETA(DisplayName = "Alive"),
};

UCLASS()
class WATERCHILD_API APlant : public AInteractable
{
    GENERATED_BODY()

public:
    APlant();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    EPlantState PlantState = EPlantState::Dead;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool IsPlantAlive;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float CurrentWaterValue = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float LastWaterValue = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float MaxWaterValue = 5.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float WaterIncreaseRate = 2.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    bool IsReviveEffectsActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float ReviveEndTime = 0.5f;
    float ReviveEndTime_Tick = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    UAnimSequence* PreGrowAnim;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float PreGrowAnimLength = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
    float CurrentPreGrowAnimTime = 0.f;
    
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Plant Interaction")
    void ActivateRevivingEffects(bool IsActivateEffects = true);
    virtual void ActivateRevivingEffects_Implementation(bool IsActivateEffects = true) {};

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Plant Interaction")
    void PlayReviveDoneEffects();
    virtual void PlayReviveDoneEffects_Implementation() {};
    
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual void OnInteract_Implementation(ASpirit* Caller) override;

    // To play plants animation
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Plant Interaction")
    void PlayPlantAnimation();
    virtual void PlayPlantAnimation_Implementation() {};

    EPlantState GetPlantState() { return PlantState; }
    void SetPlantState(EPlantState DesiredState) { PlantState = DesiredState; }

    bool GetIsPlantAlive() { return IsPlantAlive; }
    void SetIsPlantAlive(bool DesiredStatus) { IsPlantAlive = DesiredStatus; }
};
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
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        EPlantState PlantStatus = EPlantState::Dead;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
        float CurrentWaterValue = 0.0;
    UPROPERTY(EditAnywhere, Category = "Plant Variables", meta = (AllowPrivateAccess = "true"))
        float MaxWaterValue = 5.0;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    EPlantState GetPlantState() { return PlantStatus; }
    void SetPlantState(EPlantState DesiredState) { PlantStatus = DesiredState; }
};
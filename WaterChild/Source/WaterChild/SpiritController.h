// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpiritController.generated.h"

/**
 * 
 */
UCLASS()
class WATERCHILD_API ASpiritController : public APlayerController
{
	GENERATED_BODY()
	
private:
	bool bIsUsingGamepad = true;

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsUsingGamepad() { return bIsUsingGamepad; }
};

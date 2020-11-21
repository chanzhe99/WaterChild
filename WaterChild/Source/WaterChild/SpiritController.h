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
	bool bIsUsingGamepad = false;

public:
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	bool GetIsUsingGamepad() { return bIsUsingGamepad; }
	UFUNCTION(BlueprintCallable)
	void SetIsUsingGamepad(bool Value) { bIsUsingGamepad = Value; }

#pragma region Action Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void ReviveAction();
	void ReviveAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void StopReviveAction();
	void StopReviveAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void ClimbAction();
	void ClimbAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void StopClimbAction();
	void StopClimbAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void JumpAction();
	void JumpAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAction")
	void PauseAction();
	void PauseAction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerAction")
	void AnyKeyAction(FKey Key);
	void AnyKeyAction_Implementation(FKey Key) { bIsUsingGamepad = (Key.IsGamepadKey()) ? true : false; }
#pragma endregion

#pragma region Movement Axis Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void KeyboardForwardAxis(float Value);
	void KeyboardForwardAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void KeyboardRightAxis(float Value);
	void KeyboardRightAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void GamepadForwardAxis(float Value);
	void GamepadForwardAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = true; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void GamepadRightAxis(float Value);
	void GamepadRightAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = true; }
#pragma endregion

#pragma region Look Axis Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void MouseLookUpAxis(float Value);
	void MouseLookUpAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void MouseTurnAxis(float Value);
	void MouseTurnAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void GamepadLookUpAxis(float Value);
	void GamepadLookUpAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = true; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpiritAction")
	void GamepadTurnAxis(float Value);
	void GamepadTurnAxis_Implementation(float Value) { if (Value != 0.f) bIsUsingGamepad = true; }
#pragma endregion

};

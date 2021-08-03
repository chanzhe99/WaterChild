// Copyright SkyJus Works. All Rights Reserved.


#include "SpiritController.h"

void ASpiritController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Binds the buttons
	InputComponent->BindAction("ReviveButton", IE_Pressed, this, &ASpiritController::ReviveAction);
	InputComponent->BindAction("ReviveButton", IE_Released, this, &ASpiritController::StopReviveAction);
	InputComponent->BindAction("ClimbButton", IE_Pressed, this, &ASpiritController::ClimbAction);
	InputComponent->BindAction("ClimbButton", IE_Released, this, &ASpiritController::StopClimbAction);
	InputComponent->BindAction("JumpButton", IE_Pressed, this, &ASpiritController::JumpAction);
	InputComponent->BindAction("LookAtButton", IE_Pressed, this, &ASpiritController::LookAtAction).bExecuteWhenPaused = true;
	InputComponent->BindAction("PauseButton", IE_Pressed, this, &ASpiritController::PauseAction).bExecuteWhenPaused = true;
	InputComponent->BindAction("AnyKey", IE_Pressed, this, &ASpiritController::AnyKeyAction).bExecuteWhenPaused = true;

	// Binds Revive Axis
	InputComponent->BindAxis("ReviveAxis", this, &ASpiritController::ReviveAxis).bExecuteWhenPaused = true;
	
	// Binds the movement axes
	InputComponent->BindAxis("KeyboardForwardAxis", this, &ASpiritController::KeyboardForwardAxis);
	InputComponent->BindAxis("KeyboardRightAxis", this, &ASpiritController::KeyboardRightAxis);
	InputComponent->BindAxis("GamepadForwardAxis", this, &ASpiritController::GamepadForwardAxis).bExecuteWhenPaused = true;
	InputComponent->BindAxis("GamepadRightAxis", this, &ASpiritController::GamepadRightAxis).bExecuteWhenPaused = true;

	// Binds the look axes
	InputComponent->BindAxis("MouseLookUpAxis", this, &ASpiritController::MouseLookUpAxis);
	InputComponent->BindAxis("MouseTurnAxis", this, &ASpiritController::MouseTurnAxis);
	InputComponent->BindAxis("GamepadLookUpAxis", this, &ASpiritController::GamepadLookUpAxis).bExecuteWhenPaused = true;
	InputComponent->BindAxis("GamepadTurnAxis", this, &ASpiritController::GamepadTurnAxis).bExecuteWhenPaused = true;

}

void ASpiritController::KeyboardForwardAxis_Implementation(float Value)
{
	if (Value != 0.f)
		bIsUsingGamepad = false;

	if(!bIsUsingGamepad)
		ForwardAxisInput = Value;
	
}

void ASpiritController::KeyboardRightAxis_Implementation(float Value)
{
	if (Value != 0.f)
		bIsUsingGamepad = false;

	if(!bIsUsingGamepad)
		RightAxisInput = Value;
}

void ASpiritController::GamepadForwardAxis_Implementation(float Value)
{
	if (Value != 0.f)
		bIsUsingGamepad = true;

	if(bIsUsingGamepad)
		ForwardAxisInput = Value;
}

void ASpiritController::GamepadRightAxis_Implementation(float Value)
{
	if (Value != 0.f)
		bIsUsingGamepad = true;

	if(bIsUsingGamepad)
		RightAxisInput = Value;
}

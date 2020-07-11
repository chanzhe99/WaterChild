// Copyright SkyJus Works. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "../Spirit.h"
#include "Interactable.generated.h"

UCLASS()
class WATERCHILD_API AInteractable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractable();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FString ObjectName;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(ASpirit* Caller);
	virtual void OnInteract_Implementation(ASpirit* Caller);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractEnd(ASpirit* Caller);
	virtual void OnInteractEnd_Implementation(ASpirit* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StartFocus();
	virtual void StartFocus_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus();
	virtual void EndFocus_Implementation();

};

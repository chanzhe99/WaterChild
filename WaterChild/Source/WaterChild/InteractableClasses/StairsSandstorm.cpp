// Copyright SkyJus Works. All Rights Reserved.


#include "StairsSandstorm.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WaterChild/Spirit.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
//#include "DrawDebugHelpers.h"

// Sets default values
AStairsSandstorm::AStairsSandstorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComponent"));
	Root->SetupAttachment(RootComponent);
	SandstormSizeVisualiser = CreateDefaultSubobject<UBoxComponent>(TEXT("SandstormSizeVisualiser"));
	SandstormSizeVisualiser->SetupAttachment(Root);
	SandstormStartingParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormStartingParticleSystem"));
	SandstormStartingParticleSystem->SetupAttachment(Root);
	SandstormActiveParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SandstormActiveParticleSystem"));
	SandstormActiveParticleSystem->SetupAttachment(Root);
	SandstormLookAtTarget = CreateDefaultSubobject<UBillboardComponent>(TEXT("SandstormLookAtTarget"));
	SandstormLookAtTarget->SetupAttachment(Root);

	WindFX_1 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_1"));
	WindFX_1->SetupAttachment(Root);
	WindFX_2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_2"));
	WindFX_2->SetupAttachment(Root);
	WindFX_3 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_3"));
	WindFX_3->SetupAttachment(Root);
	WindFX_4 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_4"));
	WindFX_4->SetupAttachment(Root);
	WindFX_5 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_5"));
	WindFX_5->SetupAttachment(Root);
	WindFX_6 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindFX_6"));
	WindFX_6->SetupAttachment(Root);
	/*PushBackDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("PushBackDirection"));
	PushBackDirection->SetupAttachment(Root);*/
	/*SafeSpot1 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot1"));
	SafeSpot1->SetupAttachment(Root);
	SafeSpot2 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot2"));
	SafeSpot2->SetupAttachment(Root);
	SafeSpot3 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot3"));
	SafeSpot3->SetupAttachment(Root);
	SafeSpot4 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot4"));
	SafeSpot4->SetupAttachment(Root);
	SafeSpot5 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot5"));
	SafeSpot5->SetupAttachment(Root);
	SafeSpot6 = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeSpot6"));
	SafeSpot6->SetupAttachment(Root);*/
	
	SandstormSizeVisualiser->SetBoxExtent(FVector::OneVector * 100.f);
	SandstormSizeVisualiser->SetGenerateOverlapEvents(false);
	SandstormSizeVisualiser->SetCollisionProfileName("NoCollision");

	SandstormStartingParticleSystem->SetAutoActivate(false);
	SandstormActiveParticleSystem->SetAutoActivate(false);

	WindFX_1->SetAutoActivate(false);
	WindFX_2->SetAutoActivate(false);
	WindFX_3->SetAutoActivate(false);
	WindFX_4->SetAutoActivate(false);
	WindFX_5->SetAutoActivate(false);
	WindFX_6->SetAutoActivate(false);
	//SandstormStartingParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());
	//SandstormActiveParticleSystem->SetRelativeRotation(PushBackDirection->GetRelativeRotation());

	/*SafeSpot1->SetGenerateOverlapEvents(false);
	SafeSpot1->SetCollisionProfileName("NoCollision");
	SafeSpot2->SetGenerateOverlapEvents(false);
	SafeSpot2->SetCollisionProfileName("NoCollision");
	SafeSpot3->SetGenerateOverlapEvents(false);
	SafeSpot3->SetCollisionProfileName("NoCollision");
	SafeSpot4->SetGenerateOverlapEvents(false);
	SafeSpot4->SetCollisionProfileName("NoCollision");
	SafeSpot5->SetGenerateOverlapEvents(false);
	SafeSpot5->SetCollisionProfileName("NoCollision");
	SafeSpot6->SetGenerateOverlapEvents(false);
	SafeSpot6->SetCollisionProfileName("NoCollision");*/
	
	PlayerObjectList.Add(EObjectTypeQuery::ObjectTypeQuery3);
	ObjectsToIgnoreList.Empty();
	
}

void AStairsSandstorm::BeginPlay()
{
	UNiagaraComponent* tempWindFXList[] = {WindFX_1, WindFX_2, WindFX_3, WindFX_4, WindFX_5, WindFX_6 };
	WindFXList.Append(tempWindFXList, ARRAY_COUNT(tempWindFXList));
}

// Called every frame
void AStairsSandstorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoBoxTrace();
	if(IsHitPlayer && !SpiritReference)
		SpiritReference = Cast<ASpirit>(SandstormHitResult.Actor);

	if(SpiritReference)
	{
		const float travelledRatio = CalculateSandstormStrength();
		if(IsHitPlayer && IsSandstormOn)
		{
			SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default - (PlayerWalkSpeed_Difference * travelledRatio);
			if (travelledRatio < 0.f)
			{
				if(PlayerSandstormWalkAnimVar > 0.f)
					PlayerSandstormWalkAnimVar -= DeltaTime;
			}
			if(travelledRatio > 0.f)
			{
				if(!SandstormStartingParticleSystem->IsActive())
					SandstormStartingParticleSystem->Activate();

				/*if(SpiritReference->GetState() != ESpiritState::Reviving && SpiritReference->GetSpringArm()->TargetArmLength > PlayerSpringArmLength_Sandstorm)
					SpiritReference->GetSpringArm()->TargetArmLength = PlayerSpringArmLength_Default - (PlayerSpringArmLength_Difference * travelledRatio);*/
				//UE_LOG(LogTemp, Warning, TEXT("SpringArmLength: %f"), SpiritReference->GetSpringArm()->TargetArmLength);
			}
			if(travelledRatio > 0.1f)
			{
				if(PlayerSandstormWalkAnimVar < 1.f)
				{
					PlayerSandstormWalkAnimVar += DeltaTime * 0.5f;
				}
			}
			if(travelledRatio > 0.75f)
			{
				if(!SandstormActiveParticleSystem->IsActive())
					SandstormActiveParticleSystem->Activate();
			}
		}
		if(travelledRatio > 0.99f)
		{
			if(IsSandstormOn)
				IsSandstormOn = false;

			if(SandstormStartingParticleSystem->IsActive())
				SandstormStartingParticleSystem->Deactivate();
			if(SandstormActiveParticleSystem->IsActive())
				SandstormActiveParticleSystem->Deactivate();
			
			if(SpiritReference->GetCharacterMovement()->MaxWalkSpeed < PlayerWalkSpeed_Default)
				SpiritReference->GetCharacterMovement()->MaxWalkSpeed += DeltaTime * 100;
			else
				SpiritReference->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed_Default;
			
			if(PlayerSandstormWalkAnimVar > 0.f)
				PlayerSandstormWalkAnimVar -= DeltaTime;
			else
				PlayerSandstormWalkAnimVar = 0.f;
		}	
	}
	//UE_LOG(LogTemp, Warning, TEXT("PlayerSandstormWalkAnimVar: %f"), PlayerSandstormWalkAnimVar);
}

bool AStairsSandstorm::DoBoxTrace()
{
	// Main sandstorm body
	const FVector boxOrigin = GetActorLocation() + SandstormSizeVisualiser->GetRelativeLocation(); 
	IsHitPlayer = UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), boxOrigin, boxOrigin, SandstormSizeVisualiser->GetScaledBoxExtent(), FRotator::ZeroRotator, PlayerObjectList, false, ObjectsToIgnoreList, EDrawDebugTrace::None, SandstormHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

	return IsHitPlayer;
}

float AStairsSandstorm::CalculateSandstormStrength()
{
	const float totalLength = SandstormSizeVisualiser->GetScaledBoxExtent().Y * 2;
	const float startPoint = GetActorLocation().Y;
	const float playerDistanceFromStartPoint = SpiritReference->GetActorLocation().Y - startPoint;
	const float travelledRatio = playerDistanceFromStartPoint / totalLength;

	/*UE_LOG(LogTemp, Warning, TEXT("startPoint: %f"), startPoint);
	UE_LOG(LogTemp, Warning, TEXT("playerDistanceFromStartPoint: %f"), playerDistanceFromStartPoint);
	UE_LOG(LogTemp, Warning, TEXT("travelled Ratio: %f"), travelledRatio);*/
	return travelledRatio;
}

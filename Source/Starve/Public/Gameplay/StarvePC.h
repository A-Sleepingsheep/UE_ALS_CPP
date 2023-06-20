// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/Starve_ControllerInterface.h"

#include "StarvePC.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API AStarvePC : public APlayerController
{
	GENERATED_BODY()

public:
	AStarvePC();

	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	ACharacter* DebugFocusCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	TArray<ACharacter*> AvailableDebugCharacters;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool DebugView;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowHUD;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowTraces;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowDebugShapes;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowLayerColors;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool Slomo;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowCharacterInfo;
};

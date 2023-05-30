// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
	
};

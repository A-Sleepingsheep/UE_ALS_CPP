// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Enums/Starve_LocomotionEnum.h"
#include "Structs/Starve_LocomotionStructs.h"

#include "Starve_CharacterInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStarve_CharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARVE_API IStarve_CharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*获得当前角色的运动状态*/
	virtual FStarveCharacterState I_GetCurrentState() = 0;

	/*获得角色运动的主要信息*/
	virtual FEssentialValues I_GetEssentialValues() = 0;

};

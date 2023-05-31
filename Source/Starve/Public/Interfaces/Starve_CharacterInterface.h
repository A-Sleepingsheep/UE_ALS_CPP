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
	/*��õ�ǰ��ɫ���˶�״̬*/
	virtual FStarveCharacterState I_GetCurrentState() = 0;

	/*��ý�ɫ�˶�����Ҫ��Ϣ*/
	virtual FEssentialValues I_GetEssentialValues() = 0;

	/*���ý�ɫ�˶�״̬*/
	virtual void I_SetMovementState(EStarve_MovementState NewMovementState) = 0;

	/*���ý�ɫ�ƶ�ʱ���ڸ�ʲô*/
	virtual void I_SetMovementAction(EStarve_MovementAction NewMovementAction) = 0;

	/*�����������תģʽ*/
	virtual void I_SetRotationMode(EStarve_RotationMode NewRotationMode) = 0;

	/*������״̬*/
	virtual void I_SetGait(EStarve_Gait NewGait) = 0;

	/*�����ӽ�ģʽ*/
	virtual void I_SetViewMode(EStarve_ViewMode NewViewMode) = 0;

	/*���õ���״̬*/
	virtual void I_SetOverlayState(EStarve_OverlayState NewOverlayState) = 0;
};

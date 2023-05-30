// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "Starve_LocomotionStructs.generated.h"

/* 
* Starve_CharacterInterface �� I_GetCurrentState() �ӿڵķ���ֵ
*/
USTRUCT(BlueprintType)
struct FStarveCharacterState
{
	GENERATED_BODY()

public:

	FStarveCharacterState();

	/*�Դ����ƶ�״̬*/
	EMovementMode PawnMovementMode;

	/*��ǰ�˶�״̬*/
	EStarve_MovementState MovementState;

	/*��һʱ�̵��˶�״̬*/
	EStarve_MovementState PrevMovementState;

	/*��Ӧ�Ķ�����̫��*/
	EStarve_MovementAction MovementAction;

	/*�������תģʽ*/
	EStarve_RotationMode RotationMode;

	/*��ǰ�������˶�״̬*/
	EStarve_Gait ActualGait;

	/*����״̬�ķ�״̬*/
	EStarve_Stance ActualStance;

	/*�ӽ�ģʽ*/
	EStarve_ViewMode ViewMode;

	/*����̬*/
	EStarve_OverlayState OverlayState;
};


USTRUCT(BlueprintType)
struct FEssentialValues
{
	GENERATED_BODY()

public:
	FEssentialValues();

	/*�ٶ�*/
	FVector Velocity;

	/*���ٶ�*/
	FVector Acceleration;

	/*����*/
	FVector MovementInput;

	/*�Ƿ����ƶ�*/
	bool bIsMoving;

	/*�Ƿ�������*/
	bool bHasMovementInput;

	/*XYƽ����ٶ�*/
	float Speed;

	/*����ֵ*/
	float MovementInputAmount;

	/*��׼��ת*/
	FRotator AimingRatation;

	/*��׼Yaw��ת�ٶ�*/
	float AimYawRate;
};


/**
 * 
 */
UCLASS()
class STARVE_API UStarve_LocomotionStructs : public UObject
{
	GENERATED_BODY()
	
};

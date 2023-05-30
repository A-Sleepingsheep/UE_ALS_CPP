// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "Starve_LocomotionStructs.generated.h"

/* 
* Starve_CharacterInterface 的 I_GetCurrentState() 接口的返回值
*/
USTRUCT(BlueprintType)
struct FStarveCharacterState
{
	GENERATED_BODY()

public:

	FStarveCharacterState();

	/*自带的移动状态*/
	EMovementMode PawnMovementMode;

	/*当前运动状态*/
	EStarve_MovementState MovementState;

	/*上一时刻的运动状态*/
	EStarve_MovementState PrevMovementState;

	/*对应的动画蒙太奇*/
	EStarve_MovementAction MovementAction;

	/*摄像机旋转模式*/
	EStarve_RotationMode RotationMode;

	/*当前的行走运动状态*/
	EStarve_Gait ActualGait;

	/*行走状态的分状态*/
	EStarve_Stance ActualStance;

	/*视角模式*/
	EStarve_ViewMode ViewMode;

	/*叠加态*/
	EStarve_OverlayState OverlayState;
};


USTRUCT(BlueprintType)
struct FEssentialValues
{
	GENERATED_BODY()

public:
	FEssentialValues();

	/*速度*/
	FVector Velocity;

	/*加速度*/
	FVector Acceleration;

	/*输入*/
	FVector MovementInput;

	/*是否在移动*/
	bool bIsMoving;

	/*是否在输入*/
	bool bHasMovementInput;

	/*XY平面的速度*/
	float Speed;

	/*输入值*/
	float MovementInputAmount;

	/*瞄准旋转*/
	FRotator AimingRatation;

	/*瞄准Yaw旋转速度*/
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

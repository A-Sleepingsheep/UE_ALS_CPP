// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Starve_LocomotionEnum.generated.h"

/*
	Starve_LocomotionEnum
	角色站立状态下根据移动速度做出的分类
*/
UENUM(BlueprintType)
enum class EStarve_Gait : uint8 {
	//正常行走
	Walking,

	//跑步
	Running,

	//冲刺
	Sprinting
};

/*该枚举主要是针对有位移的蒙太奇动画使用，包括攀爬，翻滚，Ragdoll的起身*/
UENUM(BlueprintType)
enum class EStarve_MovementAction : uint8 {
	/*无，当不需要Montage时的状态，例如行走、跑步、冲刺*/
	None,

	/*低攀爬*/
	LowMantle,

	/*高攀爬*/
	HighMantle,

	/*翻滚*/
	Rolling,

	/*起身*/
	GettingUp,

};

/*运动时的所处状态,记录是在空中，地面等*/
UENUM(BlueprintType)
enum class EStarve_MovementState : uint8 {
	None,

	/*在地面*/
	Grounded,

	/*在空中*/
	InAir,

	/*正在翻墙*/
	Mantling,

	/*布娃娃系统*/
	Ragdoll
};


/*记录叠加状态，例如端着枪走路等*/
UENUM(BlueprintType)
enum class EStarve_OverlayState : uint8 {
	Default,
	Masculine,
	Feminine,
	Injured,
	HandsTied,
	Rifle,
	Pistol_1H,
	Postol_2H,
	Bow,
	Touch,
	Binoculars,
	Box,
	Barrel,
};


/*摄像机的旋转模式*/
UENUM(BlueprintType)
enum class EStarve_RotationMode : uint8 {
	/*速度方向，代表着角色面向的方向跟他的移动方向是一致的*/
	VelocityDirection,

	/*摄像机方向，角色的朝向跟摄像机朝向一致*/
	LookingDirection,

	/*持枪、弓箭时有瞄准的需要*/
	Aiming
};


/*行走状态下的状态，例如正常或蹲伏*/
UENUM(BlueprintType)
enum class EStarve_Stance : uint8 {
	/*站立*/
	Standing,

	/*蹲伏*/
	Crouching
};


/*视角模式，第一或第三人称*/
UENUM(BlueprintType)
enum class EStarve_ViewMode : uint8 {
	/*第三人称*/
	ThirdPerson,

	/*第一人称*/
	FirstPerson
};


/*动画特征示例*/
UENUM(BlueprintType)
enum class EAnimFeatureExample : uint8 {
	/*步距混合*/
	StrideBlending,

	/*叠加态*/
	AdditiveLeaning,

	/**/
	SprintImpulse
};


/*脚步状态*/
UENUM(BlueprintType)
enum class EFootstepType : uint8 {
	Step,
	WalkOrRun,
	Jump,
	Land
};


/*进入行走前的状态*/
UENUM(BlueprintType)
enum class EGroundedEntryState : uint8 {
	//不是从翻滚状态进入GroundedEntry的都是None
	None,

	//从翻滚状态进入GroundedEntry
	Roll
};


/*跨步骨骼的朝向*/
UENUM(BlueprintType)
enum class EHipsDirection : uint8 {
	F,
	B,
	RF,
	RB,
	LF,
	LB
};


/*哪种攀爬*/
UENUM(BlueprintType)
enum class EMantleType : uint8 {
	HighMantle,
	LowMantle,
	FallingCatch
};


/**/
UENUM(BlueprintType)
enum class EMovementDirecction : uint8 {
	Forward,
	Right,
	Left,
	Backward
};


enum class EDoWhtileReturn : uint8 {
	WhileTrue,
	WhileFalse,
	ChangeToTrue,
	ChangeToFalse,
};

/*仅仅是为了编辑器有个点击的位置*/
UCLASS()
class STARVE_API UStarve_LocomotionEnum : public UObject
{
	GENERATED_BODY()
	
};

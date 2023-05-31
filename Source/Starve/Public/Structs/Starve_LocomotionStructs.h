// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "Starve_LocomotionStructs.generated.h"

class UPrimitiveComponent;
class UAnimMontage;
class UCurveVector;
class UCurveFloat;
class UAnimSequenceBase;

#pragma region Starve_CharacterInterface_Return
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
#pragma endregion

#pragma region ALS_Structs
USTRUCT(BlueprintType)
struct FStarve_ComponentAndTransform
{
	GENERATED_BODY()

public:
	FTransform Transform;
	UPrimitiveComponent* PrimitiveComponent;
};


USTRUCT(BlueprintType)
struct FCameraSettings
{
	GENERATED_BODY()

public:
	float TargetAimLength;
	FVector SocketOffset;
	float LagSpeed;
	float RotationLagSpeed;
	bool bDoCollisionTest;
};

USTRUCT(BlueprintType)
struct FCameraSettings_Gait
{
	GENERATED_BODY()

public:
	FCameraSettings Walking;
	FCameraSettings Running;
	FCameraSettings Sprinting;
	FCameraSettings Crouching;
};

USTRUCT(BlueprintType)
struct FCameraSettings_State
{
	GENERATED_BODY()

public:
	FCameraSettings_Gait VelocityDirection;
	FCameraSettings_Gait LookingDirection;
	FCameraSettings_Gait Aiming;
};

USTRUCT(BlueprintType)
struct FDynamicMontageParams
{
	GENERATED_BODY()

public:
	UAnimSequenceBase* Animation;
	float BlendInTime;
	float BlendOutTime;
	float PlayRate;
	float StartTime;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()

public:
	float LR;
	float FB;
};

USTRUCT(BlueprintType)
struct FMantle_Asset
{
	GENERATED_BODY()

public:
	class UAnimMontage* AnimMontage;
	UCurveVector* PositionCorrectionCurve;
	FVector StartingOffset;
	float LowHeight;
	float LowPlayRate;
	float LowStartPosition;
	float HighHeight;
	float HighPlayRate;
	float HighStartPosition;
};

USTRUCT(BlueprintType)
struct FMantle_Params
{
	GENERATED_BODY()

public:
	UAnimMontage* AnimMontage;
	UCurveVector* PositionCorrectionCurve;
	float PlayRate;
	float StartingPosition;
	FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FMantle_TraceSettings
{
	GENERATED_BODY()

public:
	float MaxLedgeHeight;
	float MinLedgeHeight;
	float ReachDistance;
	float ForwardTraceRadius;
	float DownwardTracrRadius;
};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct FMovementSettings_Stance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings Crouching;
};


USTRUCT(BlueprintType, Blueprintable)
struct FMovementSettings_State :public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings_Stance VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings_Stance LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovementSettings_Stance Aiming;
};

USTRUCT(BlueprintType)
struct FRotationPlace_Asset
{
	GENERATED_BODY()

public:
	UAnimSequenceBase* Animation;
	FName SlotName;
	float SlowTurnRate;
	float FastTurnRate;
	float SlowPlayRate;
	float FastPlayRate;
};

USTRUCT(BlueprintType)
struct FTurnInPlace_Asset
{
	GENERATED_BODY()

public:
	UAnimSequenceBase* Animation;
	float AnimatedAngle;
	FName SlotName;
	float PlayRate;
	bool  bScaleTurnAngle;
};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

public:
	float F;
	float B;
	float L;
	float  R;
};

#pragma endregion

/**
 * 
 */
UCLASS()
class STARVE_API UStarve_LocomotionStructs : public UObject
{
	GENERATED_BODY()
	
};

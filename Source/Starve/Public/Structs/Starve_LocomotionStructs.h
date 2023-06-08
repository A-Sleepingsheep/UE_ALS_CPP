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
	FStarve_ComponentAndTransform() {};
	FStarve_ComponentAndTransform(FTransform transform, UPrimitiveComponent* primitivecom) :Transform(transform), PrimitiveComponent(primitivecom) {};
	
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
	FLeanAmount() :LR(0.f), FB(0.f) {};
	FLeanAmount(float lr, float fb) :LR(lr), FB(fb) {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float LR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float FB;
};


USTRUCT(BlueprintType)
struct FMantle_Asset
{
	GENERATED_BODY()

public:
	//翻墙蒙太奇
	class UAnimMontage* AnimMontage;

	//修正用的动画曲线
	UCurveVector* PositionCorrectionCurve;

	//开始的位置偏移
	FVector StartingOffset;

	//低翻墙的高度
	float LowHeight;

	//低翻墙的播放速率
	float LowPlayRate;

	//低翻开始位置
	float LowStartPosition;

	/*高翻高度*/
	float HighHeight;

	/*高翻播放速率*/
	float HighPlayRate;

	/*高翻开始位置*/
	float HighStartPosition;
};

USTRUCT(BlueprintType)
struct FMantle_Params
{
	GENERATED_BODY()

public:
	/*翻墙蒙太奇*/
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
	FMantle_TraceSettings() {};
	FMantle_TraceSettings(float MaxLH, float MinLH, float RD, float FTR, float DTR) :MaxLedgeHeight(MaxLH), MinLedgeHeight(MinLH),ReachDistance(RD), ForwardTraceRadius(FTR), DownwardTracrRadius(DTR) {};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* Animation;//转向动画

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimatedAngle;//动画能够旋转多少角度

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SlotName;//插槽名称

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate;//播放速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool  bScaleTurnAngle;//是否缩放角度
};


USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float F;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float B;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float L;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float R;
};


/*下面两个结构体是动画修改器使用的*/
//动画修改所需数据
USTRUCT(BlueprintType)
struct FAnimCurveCreationData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int FrameNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurveValue;
};

//动画修改所需参数
USTRUCT(BlueprintType)
struct FAnimCurveCreationParams
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName CurveName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bKeyEachFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray< FAnimCurveCreationData> Keys;
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

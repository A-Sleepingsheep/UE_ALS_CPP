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

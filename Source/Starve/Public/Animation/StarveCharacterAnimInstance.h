// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Enums/Starve_LocomotionEnum.h"
#include "Structs/Starve_LocomotionStructs.h"
#include "Interfaces/Starve_AnimationInterface.h"

#include "StarveCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UStarveCharacterAnimInstance : public UAnimInstance,public IStarve_AnimationInterface
{
	GENERATED_BODY()

public:
	UStarveCharacterAnimInstance();
	
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


private:
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DeltaTimeX;

	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ACharacter* CharacterRef;

	#pragma region CharacterEssentialValues
	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector Acceleration;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector MovementInput;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasMovementInput;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MovementInputAmount;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator AimingRatation;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYawRate;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementState MovementState;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementState PrevMovementState;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementAction MovementAction;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_RotationMode RotationMode;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Gait Gait;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Stance Stance;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_ViewMode ViewMode;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_OverlayState OverlayState;
	#pragma endregion

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bShouldMove = false;

	bool bPreShouldMove = false;/*保存前一帧的bShouldMove*/

	//FVelocityBlend是自己创建的结构体，进行速度混合，主要是FBLR，对应前后左右
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVelocityBlend VelocityBlend;

	UPROPERTY(Category = Anim_GroundedConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float VelocityBlendInterpSpeed = 12.f;//进行速度混合的速度

	//对角线修正混合曲线
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiagonalScaleAmountCurve;

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DiagonalScaleAmount;//对角线修正混合曲线处获得的值
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector RelativeAccelerationAmount;//局部坐标系下加速度方向
		
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FLeanAmount LeanAmount;//移动偏移
	
	UPROPERTY(Category = AnimConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GroundedLeanInterpSpeed = 4.f;//LeanAmount差值速度

	//walk 和 Run 的过渡值
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkRunBlend = 0.f;

	//站立情况下行走的步距过渡曲线
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Walk;
	
	//站立情况下奔跑的步距过渡曲线
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Run;
	
	//蹲伏情况下行走的步距过渡曲线
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_C_Walk;
	
	//步距混合值
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StrideBlend = 0.f;
	
	//站立动画播放速率
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StandingPlayRate = 0.f;

	//下蹲动画播放速率
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CrouchingPlayRate = 0.f;

	//行走动画的速度
	UPROPERTY(Category = Anim_GroundedConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedWalkSpeed = 150.f;

	//奔跑动画的速度
	UPROPERTY(Category = Anim_GroundedConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedRunSpeed = 350.f;

	//冲刺动画的速度
	UPROPERTY(Category = Anim_GroundedConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedSprintSpeed = 600.f;

	//下蹲动画的速度
	UPROPERTY(Category = Anim_GroundedConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedCrouchSpeed = 150.f;

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EMovementDirecction MovementDirection;//运动方向（前、后、左、右）

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetFB;//摄像机前后偏移曲线
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetLR;//摄像机左右偏移曲线

	//前向偏移
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FYaw;

	//后向偏移
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float BYaw;

	//左向偏移
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LYaw;

	//右向偏移
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RYaw;

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EHipsDirection TrackedHipsDirection;//Hips朝向,通过动画通知修改
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPivot;//Pivot,动画通知修改

	//从什么状态进入Grounded状态
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EGroundedEntryState GroundedEntryState;
	
	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TriggerPivotLimitSpeed = 200.f;//Pivot,State中有Pivot动画通知的时候修改

	//AimingRotation中间过渡的值
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator SmoothedAimingRotation;
	
	UPROPERTY(Category = AimingConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SmoothedAimingRotationInterpSpeed = 10.f;//Rotation差值速度
	
	//AimingAngle，X代表Yaw偏移，Y代表Pitch偏移
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector2D AimingAngle;

	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector2D SmoothedAimingAngle;//中间的Aiming角度

	//原地转向的最小角度
	UPROPERTY(Category = "TurnInPlaceConfig", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float TurnCheckMinAngle = 45.f;

	//原地转向的速度限制
	UPROPERTY(Category = "TurnInPlaceConfig", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYawRateLimit = 50.f;

	UPROPERTY(Category = "TurnInPlace", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;//转向开始前的延迟时间

	//原地转向最小角度前延迟时间
	UPROPERTY(Category = "TurnInPlaceConfig", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MinAngleDelay = 0.75f;

	//原地转向最大角度延迟时间
	UPROPERTY(Category = "TurnInPlaceConfig", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxAngleDelay = 0.f;

	//站立左转90，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset N_TurnIP_L90;

	//站立右转90，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset N_TurnIP_R90;

	//站立左转180，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset N_TurnIP_L180;

	//站立右转180，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset N_TurnIP_R180;

	//蹲伏左转90，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLF_TurnIP_L90;

	//蹲伏右转90，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLF_TurnIP_R90;

	//蹲伏左转180，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLF_TurnIP_L180;

	//蹲伏右转180，在蓝图中进行赋值
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTurnInPlace_Asset CLF_TurnIP_R180;

	//根据这个阈值判断是否旋转180还是90
	UPROPERTY(Category = "TurnInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Turn180Threshold = 130.f;

	UPROPERTY(Category = "TurnInPlace", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotationScale;//原地转向的缩放

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool Rotate_L;//是否向左旋转

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool Rotate_R;//是否向右旋转

	//是否向左进行原地旋转最小阈值
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotateMinThreshold = -50.f;
	
	//是否向右进行原地旋转最大阈值
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotateMaxThreshold = 50.f;
	
	//AimYawRate映射的最小值
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYawRateMinRange = 90.f;
	
	//AimYawRate映射的最大值
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYawRateMaxRange = 270.f;

	//AimYawRate映射到原地旋转动画最小PlayRate
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MinPlayRate = 1.15f;

	//AimYawRate映射到原地旋转动画最大PlayRate
	UPROPERTY(Category = "RotateInPlaceConfig", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxPlayRate = 3.f;

	UPROPERTY(Category = "RotateInPlace", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotateRate = 1.f;//Rotate的值

	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool Jumped;//是否跳跃

	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float JumpPlayRate;//跳跃动画的速率

	/*左脚锁定的Alpha值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FootLock_L_Alpha;

	/*右脚锁定的Alpha值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FootLock_R_Alpha;

	/*左脚锁定的Location值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector FootLock_L_Location = FVector(0.f);

	/*右脚锁定的Location值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector FootLock_R_Location = FVector(0.f);

	/*左脚锁定的Rotation值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator FootLock_L_Rotation = FRotator(0.f);

	/*右脚锁定的Rotation值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator FootLock_R_Rotation = FRotator(0.f);

	/*左脚步IK的Locatin偏移值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector FootOffset_L_Location = FVector(0.f);

	/*右脚步IK的Locatin偏移值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector FootOffset_R_Location = FVector(0.f);

	/*左脚IK的Rotation偏移值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator FootOffset_L_Rotation = FRotator(0.f);

	/*右脚IK的Rotation偏移值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator FootOffset_R_Rotation = FRotator(0.f);

	/*IK射线向上检测距离,配置变量*/
	UPROPERTY(Category = FootIKConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float IKTraceDistanceAboveFoot = 100.f;

	/*IK射线向下检测距离,配置变量*/
	UPROPERTY(Category = FootIKConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float IKTraceDistanceBlowFoot = 45.f;

	/*这里是相当于脚踝到脚面的距离,配置变量*/
	UPROPERTY(Category = FootIKConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FootHeight = 13.5f;

	/*人物整体偏移的Alpha值*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float PelvisAlpha;

	/*身体的整体偏移*/
	UPROPERTY(Category = FootIK, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector PelvisOffset = FVector(0.f);

	/*Transition的动画*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimSequenceBase* lefttransitionanimtion;

	/*Transition的动画*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimSequenceBase* righttransitionanimtion;

	/*Transition的动画*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimSequenceBase* stopldowntransitionanimtion;

	/*Transition的动画*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimSequenceBase* stoprdowntransitionanimtion;

	/*下落数值方向的速度大小*/
	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FallSpeed;

	/*落地时的混合度*/
	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LandPredicion;

	/*着地的曲线*/
	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LandPredictionCurve;

	/*在空中的偏移修正曲线*/
	UPROPERTY(Category = Anim_InAir, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LeanInAirCurve;

	/*在空中差值的偏移速度*/
	UPROPERTY(Category = Anim_InAir, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float InAirLeanInterpSpeed = 4.f;

	/*Alpha值，是否开启AimOffset*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Enable_AimOffset;

	/*站立权重*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BasePose_N;

	/*CLF蹲伏权重*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BasePose_CLF;

	/*Spine骨骼动画增量的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Spine_Add;
	
	/*Hend骨骼动画增量的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Head_Add;
	
	/*Arm_L左手臂骨骼动画增量的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_L_Add;

	/*Arm_R右手臂骨骼动画增量的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_R_Add;

	/*HandL左手混合Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Hand_L;
	
	/*HandR右手混合Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Hand_R;

	/*是否开左手IK*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Enable_HandIK_L;
	
	/*是否开启右手IK*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Enable_HandIK_R;
	
	/*LocalSpace空间下的左手臂混合Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_L_LS;
		
	/*LocalSpace空间下右手臂混合的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_R_LS;
	
	/*MeshSpace空间下的左手臂混合Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_L_MS;
		
	/*MeshSpace空间下右手臂混合的Alpha值*/
	UPROPERTY(Category = Anim_LayerBlending, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Arm_R_MS;

	/*Overlay的Override值*/
	UPROPERTY(Category = Anim_LayerBlending, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int OverlayOverrideState;

	/*人物瞄准偏移的Alpha值，用该值来对Aiming状态下角色的Pitch值进行修改*/
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimSweepTime;
	
	/*瞄准模式下Spine旋转*/
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator SpineRotation;
	
	/*有输入时的yaw值偏移时间*/
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float InputYawOffsetTime;

	/*Velocity模式下Character朝向Camera视角的插值速度*/
	UPROPERTY(Category = AimingConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float InputYawOffsetInterpSpeed;

	
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LeftYawTime;

	
	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RightYawTime;

	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ForwardYawTime;
	
	/*Ragdoll速率*/
	UPROPERTY(Category = Ragdoll, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FlailRate;

	//MovementSystem
	void UpdateCharacterInfo(); /*更新角色信息*/
	void UpdateMovementValues();/*更新角色运动的相关信息*/
	void UpdateRotationValues();/*更新旋转模式相关信息*/
	
	/*角色是否在移动判断*/
	bool ShouldMoveCheck();
	
	/*用俩判断shouldMove是否发生了改变，对应原来里面的DoWhile(True，false)宏*/
	//EDoWhtileReturn DoWhile(bool bSMove, bool bLastShouldMove);
	
	//根据Velocity计算FVelocityBlend的值，用于前后左右速度的混合
	FVelocityBlend CalculateVelocityBlend();
	
	//进行FVelocityBlend的差值处理(为了平滑过渡)
	FVelocityBlend InterpVelocityBlend(FVelocityBlend& Current, FVelocityBlend& Target, float InterpSpeed, float DeltaTime);
	
	//对对角线上的数值进行映射，返回的值最终会运用在IK骨骼中
	float CalculateDiagonalScaleAmount();
	
	//返回人物局部坐标下角色的单位加速度向量
	FVector CalculateRelativeAccelerationAmount();
	
	//LeanAmount插值
	FLeanAmount InterpLeanAmount(FLeanAmount& Current, FLeanAmount& Target, float InterpSpeed, float DeltaTime);
	
	//计算walk和run的混合度
	float CalculateWalkRunBlend();
	
	//计算步距Alpha
	float CalculateStrideBlend();
	
	//获得角色动画曲线并进行钳制,代替 GetAnimCurveClamp 宏
	float GetAnimCurveClamp(FName CurveName, float Bias, float ClampMin, float ClampMax);
	
	float CalculateStandingPlayRate();//计算站立动画播放速率
	
	float CalculateCrouchingPlayRate();//计算下蹲动画播放塑料厂

	//RotationSystem
	//计算运动朝向,该值表示在“环视/瞄准”旋转模式期间角色相对于摄影机移动的方向，并在“循环混合动画层”中用于混合到适当的方向状态。
	EMovementDirecction CalculateMovementDirection();
	//计算运动朝向的象限
	EMovementDirecction CalculateQuadrant(EMovementDirecction CurrentMD,float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angle);
	//角度是否在某个范围
	bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer);


	//AnimNotify，是在六向状态机 State 中的 CustomBlueprintEvent 通知
	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_F(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_B(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_LF(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_LB(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_RF(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Hips_RB(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "HipsAnimNotify")
		void AnimNotify_Pivot(UAnimNotify* Notify);

	//延迟Delay结束后调用的函数
	UFUNCTION()
	void PivotDelayFinish(); //Delay结束调用函数

	 //原地转向相关函数
	bool CanRotateInPlace();//是否能够原地转头
	
	bool CanTurnInPlace();//是否能过原地转向
	
	bool CanDynamicTransition();//是否开启动态过渡
	
	void TurnInPlaceCheck();//原地转向检测并调用转向操作
	
	void UpdateAimingValues();//更新Aiming时的某些参数
	//转向函数
	void TurnInPlace(const FRotator& TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent);//原地转向检测并进行转向操作

	void RotateInPlaceCheck();//原地旋转检测

	//AnimationInterfaces
	virtual void I_Jumped() override;

	virtual void I_SetGroundedEntryState(EGroundedEntryState GroundEntryState) override;

	virtual void I_SetOverlayOverrideState(int overlayOverrideState) override;

	//延迟Delay结束后调用的函数
	UFUNCTION()
	void I_JumpedDelayFinish(); //Delay结束调用函数

	/*每帧更新Layer相关的东西*/
	void UpdateLayerValues();

	/*每帧更新IK相关的内容*/
	void UpdateFootIK();

	/*判断是否开启脚步锁定，锁定脚步相关参数的赋值*/
	void SetFootLocking(FName EnableFootIKCurve, FName FootLockCuve, FName FootIKBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);

	/*脚步锁定的偏移量计算*/
	void SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation);

	/*移动状态下脚步的IK偏移计算*/
	void SetFootOffsets(FName EnableFootIKCurve, FName IK_FootBone, FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset);

	/*通过查找平均脚部IK权重来计算“PelvisAlpha”。如果alpha为0，清除偏移。整体人物的位置偏移*/
	void SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target);

	/*将IK偏移归零*/
	void ResetIKOffsets();

	/*动态过渡检测*/
	void DynamicTransitionCheck();

	/*普通过渡*/
	void PlayTransition(FDynamicMontageParams Paramters);

	/*动态过渡*/
	void PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Paramters);

	/*是否动态过渡*/
	bool bCanPlayDynamicTransition = true;

	/*动态过渡时间处理*/
	FTimerHandle PlayDynamicTransitionTimer;

	/*动态过渡延迟触发*/
	UFUNCTION()
	void PlayDynamicTransitionDelay();

	/*这个是 UKismetAnimationLibrary::K2_DistanceBetweenTwoSocketsAndMapRange() 函数的复制，主要是这个函数在这里无法找到*/
	static float GetDistanceBetweenTwoSockets(
		const USkeletalMeshComponent* Component,
		const FName SocketOrBoneNameA,
		ERelativeTransformSpace SocketSpaceA,
		const FName SocketOrBoneNameB,
		ERelativeTransformSpace SocketSpaceB,
		bool bRemapRange,
		float InRangeMin,
		float InRangeMax,
		float OutRangeMin,
		float OutRangeMax);

	/*在State中触发的动画通知N_Stop_L*/
	UFUNCTION(BlueprintCallable, Category = "Transition")
		void AnimNotify_N_Stop_L(UAnimNotify* Notify);

	/*在State中触发的动画通知N_Stop_R*/
	UFUNCTION(BlueprintCallable, Category = "Transition")
		void AnimNotify_N_Stop_R(UAnimNotify* Notify);

	/*在State中触发的动画通知StopTransition*/
	UFUNCTION(BlueprintCallable, Category = "Transition")
		void AnimNotify_StopTransition(UAnimNotify* Notify);

	/*在空中时每帧更新数据*/
	void UpdateInAirValues();

	/*计算LandPrediction*/
	float CalculateLandPrediction();

	/*计算空中LeanAmount*/
	FLeanAmount CalculateInAirLeanAmount();

	/*在State中触发的动画通知LandToIdle*/
	UFUNCTION(BlueprintCallable, Category = "LandToIdle")
		void AnimNotify_LandToIdle(UAnimNotify* Notify);

	/*在State中触发的动画通知To_CLF_Stop*/
	UFUNCTION(BlueprintCallable, Category = "Stop")
		void AnimNotify_To_CLF_Stop(UAnimNotify* Notify);

	/*在State中触发的动画通知To_N_QuickStop*/
	UFUNCTION(BlueprintCallable, Category = "Stop")
		void AnimNotify_To_N_QuickStop(UAnimNotify* Notify);

	/*在State中触发的动画通知RollToIdle*/
	UFUNCTION(BlueprintCallable, Category = "Stop")
		void AnimNotify_RollToIdle(UAnimNotify* Notify);

	UFUNCTION(BlueprintCallable, Category = "Reset_GroundedEntryState")
		void AnimNotify_Reset_GroundedEntryState(UAnimNotify* Notify);

	/*Bow的过渡通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_BowRelaxedToReady(UAnimNotify* Notify);
	
	/*Bow的过渡通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_BowReadyToRelaxed(UAnimNotify* Notify);
	
	/*步枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_RifleRelaxedToReady(UAnimNotify* Notify);

	/*步枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_RifleReadyToRelaxed(UAnimNotify* Notify);

	/*单手手枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_Pistol_1H_RelaxedToReady(UAnimNotify* Notify);
	
	/*单手手枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_Pistol_1H_ReadyToRelaxed(UAnimNotify* Notify);
	
	/*双手手枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_Pistol_2H_RelaxedToReady(UAnimNotify* Notify);
	
	/*双手手枪的动画通知*/
	UFUNCTION(BlueprintCallable, Category = "Overlay")
		void AnimNotify_Pistol_2H_ReadyToRelaxed(UAnimNotify* Notify);

	/*叠加态能够过渡*/
	bool CanOverlayTransition();

	/*每帧更新Ragdoll相关信息*/
	void UpdateRagdollValues();

	EDrawDebugTrace::Type GetDebugTraceType(EDrawDebugTrace::Type ShowTraceType);
};

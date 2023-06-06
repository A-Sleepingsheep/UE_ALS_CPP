// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Enums/Starve_LocomotionEnum.h"
#include "Structs/Starve_LocomotionStructs.h"

#include "StarveCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UStarveCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UStarveCharacterAnimInstance();
	
	virtual void NativeInitializeAnimation() override;

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
	bool bShouldMove;

	bool bDoOnce = false;/*用来判断当角色bShouldMove发生变动时，对用原来里面的DoWhile*/

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVelocityBlend VelocityBlend;//FVelocityBlend是自己创建的结构体，进行速度混合，主要是FBLR，对应前后左右

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float VelocityBlendInterpSpeed = 12.f;//进行速度混合的速度

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiagonalScaleAmountCurve;//对角线修正需要的混合曲线

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DiagonalScaleAmount;//对角线修正混合曲线处获得的值
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector RelativeAccelerationAmount;//局部坐标系下加速度方向
		
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FLeanAmount LeanAmount;//移动偏移
	
	UPROPERTY(Category = AnimConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GroundedLeanInterpSpeed = 4.f;//LeanAmount差值速度

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkRunBlend = 0.f;//walk 和 Run 的过渡值

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Walk;//站立情况下行走的步距过渡曲线
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_N_Run;//站立情况下奔跑的步距过渡曲线
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StrideBlend_C_Walk;//蹲伏情况下行走的步距过渡曲线
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StrideBlend = 0.f;//步距
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StandingPlayRate = 0.f;//站立动画播放速率

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CrouchingPlayRate = 0.f;//下蹲动画播放速率

	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedWalkSpeed = 150.f;//行走动画的速度

	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedRunSpeed = 350.f;//奔跑动画的速度

	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedSprintSpeed = 600.f;//冲刺动画的速度

	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AnimatedCrouchSpeed = 150.f;//下蹲动画的速度

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EMovementDirecction MovementDirection;//运动方向（前、后、左、右）

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetFB;//摄像机前后偏移曲线
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveVector* YawOffsetLR;//摄像机左右偏移曲线

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FYaw;//前向偏移

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float BYaw;//后向偏移

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LYaw;//左向偏移

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RYaw;//右向偏移

	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EHipsDirection TrackedHipsDirection;//Hips朝向,通过动画通知修改
	
	UPROPERTY(Category = Anim_Grounded, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPivot;//Pivot,动画通知修改
	
	UPROPERTY(Category = AnimConfig, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TriggerPivotLimitSpeed = 200.f;//Pivot,State中有Pivot动画通知的时候修改

	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator SmoothedAimingRotation;//旋转的中间过渡值
	
	UPROPERTY(Category = AnimConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SmoothedAimingRotationInterpSpeed = 10.f;//Rotation差值速度

	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector2D AimingAngle;//Aiming角度

	UPROPERTY(Category = Anim_Aiming, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector2D SmoothedAimingAngle;//中间的Aiming角度

	UPROPERTY(Category = AnimConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float TurnCheckMinAngle = 45.f;//原地转向的最小角度

	UPROPERTY(Category = AnimConfig, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYawRateLimit = 50.f;//原地转向的速度限制

	UPROPERTY(Category = "TurnInPlace", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ElapsedDelayTime;//转向开始前的延迟时间

	UPROPERTY(Category = "TurnInPlace", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MinAngleDelay = 0.75f;//最小角度转向前延迟时间

	UPROPERTY(Category = "TurnInPlace", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxAngleDelay = 0.f;//最大角度转向前延迟时间

	//MovementSystem
	void UpdateCharacterInfo(); /*更新角色信息*/
	void UpdateMovementValues();/*更新角色运动的相关信息*/
	void UpdateRotationValues();/*更新旋转模式相关信息*/
	bool ShouldMoveCheck();/*角色是否应该移动判断*/
	EDoWhtileReturn DoWhile(bool bSMove, bool bLastShouldMove);/*用俩判断shouldMove是否发生了改变，对应原来里面的DoWhile(True，false)宏*/
	FVelocityBlend CalculateVelocityBlend();//计算FVelocityBlend的值，用于前后左右速度的混合
	//进行FVelocityBlend的差值处理(为了平滑过渡)
	FVelocityBlend InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed, float DeltaTime);
	float CalculateDiagonalScaleAmount();//对对角线上的数值进行映射，返回的值最终会运用在IK骨骼中
	FVector CalculateRelativeAccelerationAmount();//返回人物局部坐标下角色的单位加速度向量
	//LeanAmount差值获得平滑过渡效果
	FLeanAmount InterpLeanAmount(FLeanAmount Current, FLeanAmount Target, float InterpSpeed, float DeltaTime);
	float CalculateWalkRunBlend();//计算walk和run的混合度
	float CalculateStrideBlend();//计算步距Alpha
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


	//AnimNotify，是在状态机 State 中的 CustomBlueprintEvent 通知
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

};

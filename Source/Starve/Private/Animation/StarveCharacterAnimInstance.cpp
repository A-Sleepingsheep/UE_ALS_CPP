// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/StarveCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

#include "Interfaces/Starve_CharacterInterface.h"
#include "Interfaces/Starve_ControllerInterface.h"

UStarveCharacterAnimInstance::UStarveCharacterAnimInstance() {
	static ConstructorHelpers::FObjectFinder<UCurveFloat> cf(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/DiagonalScaleAmount.DiagonalScaleAmount'"));
	if (cf.Succeeded()) {
		DiagonalScaleAmountCurve = cf.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> cf2(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/StrideBlend_N_Walk.StrideBlend_N_Walk'"));
	if (cf2.Succeeded()) {
		StrideBlend_N_Walk = cf2.Object;
		StrideBlend_C_Walk = cf2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> cf3(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/StrideBlend_N_Run.StrideBlend_N_Run'"));
	if (cf3.Succeeded()) {
		StrideBlend_N_Run = cf3.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveVector> cf4(TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/YawOffset_FB.YawOffset_FB'"));
	if (cf4.Succeeded()) {
		YawOffsetFB = cf4.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveVector> cf5(TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/YawOffset_LR.YawOffset_LR'"));
	if (cf5.Succeeded()) {
		YawOffsetLR = cf5.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> leftfinder(TEXT("AnimSequence'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Base/Transitions/Starve_N_Transition_R.Starve_N_Transition_R'"));
	if (leftfinder.Succeeded()) {
		lefttransitionanimtion = leftfinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> rightfinder(TEXT("AnimSequence'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Base/Transitions/Starve_N_Transition_L.Starve_N_Transition_L'"));
	if (rightfinder.Succeeded()) {
		righttransitionanimtion = rightfinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> stoplfinder(TEXT("AnimSequence'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Base/Transitions/Starve_N_Stop_L_Down.Starve_N_Stop_L_Down'"));
	if (stoplfinder.Succeeded()) {
		stopldowntransitionanimtion = stoplfinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> stoprfinder(TEXT("AnimSequence'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Base/Transitions/Starve_N_Stop_R_Down.Starve_N_Stop_R_Down'"));
	if (stoprfinder.Succeeded()) {
		stoprdowntransitionanimtion = stoprfinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> landpredictionfinder(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/LandPredictionBlend.LandPredictionBlend'"));
	if (landpredictionfinder.Succeeded()) {
		LandPredictionCurve = landpredictionfinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> landinairfinder(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/LeanInAirAmount.LeanInAirAmount'"));
	if (landinairfinder.Succeeded()) {
		LeanInAirCurve = landinairfinder.Object;
	}
}

void UStarveCharacterAnimInstance::NativeBeginPlay()
{
	APawn* pawn = TryGetPawnOwner();
	if (IsValid(pawn)) {
		this->CharacterRef = Cast<ACharacter>(pawn);
	}
}

void UStarveCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	this->DeltaTimeX = DeltaSeconds;
	if (DeltaTimeX != 0.f && IsValid(CharacterRef)) {
		/*每帧更新运动信息*/
		UpdateCharacterInfo();
		/*更新Aiming信息*/
		UpdateAimingValues();
		/*更新Layer骨骼混合相关信息，一般都是些Alpha值*/
		UpdateLayerValues();
		/*更新FootIK*/
		UpdateFootIK();
		/*更新不同MovementState状态下的信息*/
		switch (MovementState)
		{
			case EStarve_MovementState::Grounded: {
				bPreShouldMove = bShouldMove;
				bShouldMove = ShouldMoveCheck();
				//前一帧没有输入，这一帧有输入
				if (bPreShouldMove == false && bShouldMove) {
					ElapsedDelayTime = 0.f;
					Rotate_L = false;
					Rotate_R = false;
				}

				//bShouldMove为真，角色有运动输入
				if (bShouldMove) {
					UpdateMovementValues();
					UpdateRotationValues();
				}
				else {
					//角色没有运动输入
					/*第一步，是否可以原地转头*/
					if (CanRotateInPlace()) {
						RotateInPlaceCheck();
					}
					else {
						Rotate_L = false;
						Rotate_R = false;
					}

					/*第二步，是否能够原地转向*/
					if (CanTurnInPlace()) {
						TurnInPlaceCheck();
					}
					else {
						ElapsedDelayTime = 0.f;
					}

					/*第三步，是否动态过渡*/
					if (CanDynamicTransition()) {
						DynamicTransitionCheck();
					}
				}
				return;
			}
			case EStarve_MovementState::InAir: {
				UpdateInAirValues();
				return;
			}

			case EStarve_MovementState::Ragdoll: {
				UpdateRagdollValues();
				return;
			}
		}
	}
}


void UStarveCharacterAnimInstance::UpdateCharacterInfo()
{
	IStarve_CharacterInterface* sci = Cast<IStarve_CharacterInterface>(CharacterRef);
	if (sci != nullptr) {
		FEssentialValues e_v = sci->I_GetEssentialValues();
		this->Velocity = e_v.Velocity;
		this->Acceleration = e_v.Acceleration;
		this->MovementInput = e_v.MovementInput;
		this->bIsMoving = e_v.bIsMoving;
		this->bHasMovementInput = e_v.bHasMovementInput;
		this->Speed = e_v.Speed;
		this->MovementInputAmount = e_v.MovementInputAmount;
		this->AimingRatation = e_v.AimingRatation;
		this->AimYawRate = e_v.AimYawRate;

		FStarveCharacterState starve_cs =sci->I_GetCurrentState();
		this->MovementState = starve_cs.MovementState;
		this->PrevMovementState = starve_cs.PrevMovementState;
		this->MovementAction = starve_cs.MovementAction;
		this->RotationMode = starve_cs.RotationMode;
		this->Gait = starve_cs.ActualGait;
		this->Stance = starve_cs.ActualStance;
		this->ViewMode = starve_cs.ViewMode;
		this->OverlayState = starve_cs.OverlayState;
	}
}

void UStarveCharacterAnimInstance::UpdateMovementValues()
{
	/*1.利用插值设置FVelocityBlend*/
	FVelocityBlend vb = CalculateVelocityBlend();
	VelocityBlend = InterpVelocityBlend(VelocityBlend, vb, VelocityBlendInterpSpeed, DeltaTimeX);

	/*2.获得DiagonalScaleAmount，用于根骨骼位置修正*/
	DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	/*3.计算RelativeAccelerationAmount并使用它进行LeanAmount插值*/
	/*利用加速度获取偏移量，来实现速度变换的平滑过渡*/
	//获得局部坐标系下的单位加速度向量
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	//LeanAmount插值
	FLeanAmount targetlean = FLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X);
	LeanAmount = InterpLeanAmount(LeanAmount, targetlean, GroundedLeanInterpSpeed, DeltaTimeX);

	/*4.计算WalkRunBlend、StrideBlend、StandingPlayRate、CrouchingPlayRate*/
	WalkRunBlend = CalculateWalkRunBlend();
	StrideBlend = CalculateStrideBlend();
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();

}

void UStarveCharacterAnimInstance::UpdateRotationValues()
{
	/*第一步、计算运动方向*/
	MovementDirection = CalculateMovementDirection();

	/*第二步*/
	//设置YawOffset。这些值会影响动画图中的“YawOffset”曲线，
	//并用于偏移角色旋转以获得更自然的移动。曲线允许对偏移在每个移动方向上的行为进行精细控制。
	FRotator rotation_delta = UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(), CharacterRef->GetControlRotation());
	FVector yaw_offset = YawOffsetFB->GetVectorValue(rotation_delta.Yaw);
	FYaw = yaw_offset.X;
	BYaw = yaw_offset.Y;
	
	yaw_offset = YawOffsetLR->GetVectorValue(rotation_delta.Yaw);
	LYaw = yaw_offset.X;
	RYaw = yaw_offset.Y;
}

bool UStarveCharacterAnimInstance::ShouldMoveCheck()
{
	//前一个是输入判断，后一个是当你没有进行输入，但角色还处于减速状态
	return (bIsMoving && bHasMovementInput) || Speed > 150.f;
}

//EDoWhtileReturn UStarveCharacterAnimInstance::DoWhile(bool bSMove, bool bLastShouldMove)
//{
//	EDoWhtileReturn dwr;
//	if (bSMove) {
//		dwr = (bLastShouldMove == bShouldMove) ? EDoWhtileReturn::WhileTrue : EDoWhtileReturn::ChangeToTrue;
//	}
//	else {
//		dwr = (bLastShouldMove == bShouldMove) ? EDoWhtileReturn::WhileFalse : EDoWhtileReturn::ChangeToFalse;
//	}
//	return dwr;
//}

FVelocityBlend UStarveCharacterAnimInstance::CalculateVelocityBlend()
{
	//速度单位化保证在斜方向上速度的值为0.5
	FVector velocity_direction = Velocity.GetSafeNormal(0.1f);
	//UnrotateVector表示将速度向量放到Local坐标系中，表示将人物的模型坐标用世界坐标表示出来，可以理解为将速度的处理放在自己模型的空间
	velocity_direction = CharacterRef->GetActorRotation().UnrotateVector(velocity_direction);
	
	//这一步的目的还是为了限制velocity_direction的各值在0~1之间
	float sum = FMath::Abs(velocity_direction.X) + FMath::Abs(velocity_direction.Y) + FMath::Abs(velocity_direction.Z);
	velocity_direction = velocity_direction / sum;

	FVelocityBlend vb;
	vb.F = FMath::Clamp(velocity_direction.X, 0.f, 1.f);
	vb.B = FMath::Abs(FMath::Clamp(velocity_direction.X, -1.f, 0.f));
	vb.L = FMath::Abs(FMath::Clamp(velocity_direction.Y, -1.f, 0.f));
	vb.R = FMath::Clamp(velocity_direction.Y, 0.f, 1.f);

	return vb;
}

FVelocityBlend UStarveCharacterAnimInstance::InterpVelocityBlend(FVelocityBlend& Current, FVelocityBlend& Target, float InterpSpeed, float DeltaTime)
{
	FVelocityBlend vb;

	vb.F = FMath::FInterpTo(Current.F, Target.F, DeltaTime, InterpSpeed);
	vb.B = FMath::FInterpTo(Current.B, Target.B, DeltaTime, InterpSpeed);
	vb.L = FMath::FInterpTo(Current.L, Target.L, DeltaTime, InterpSpeed);
	vb.R = FMath::FInterpTo(Current.R, Target.R, DeltaTime, InterpSpeed);

	return vb;
}

float UStarveCharacterAnimInstance::CalculateDiagonalScaleAmount()
{
	float xy = FMath::Abs(VelocityBlend.F + VelocityBlend.B);
	return DiagonalScaleAmountCurve->GetFloatValue(xy);
}

FVector UStarveCharacterAnimInstance::CalculateRelativeAccelerationAmount()
{
	//上面代码优化
	//利用向量点乘判断是加速还是减速
	float value = FVector::DotProduct(Acceleration, Velocity);
	
	UCharacterMovementComponent* cmc = CharacterRef->GetCharacterMovement();
	value = value >0 ? cmc->GetMaxAcceleration(): cmc->GetMaxBrakingDeceleration();
	//将加速度的最大值限制在maxacce
	FVector clamp_acce = Acceleration.GetClampedToMaxSize(value);
	//返回一个局部坐标的加速度向量，限制了最大值
	return CharacterRef->GetActorRotation().UnrotateVector(clamp_acce / value);
}


FLeanAmount UStarveCharacterAnimInstance::InterpLeanAmount(FLeanAmount& Current, FLeanAmount& Target, float InterpSpeed, float DeltaTime)
{
	FLeanAmount la;

	la.LR = FMath::FInterpTo(Current.LR, Target.LR, DeltaTime, InterpSpeed);
	la.FB = FMath::FInterpTo(Current.FB, Target.FB, DeltaTime, InterpSpeed);

	return la;
}

float UStarveCharacterAnimInstance::CalculateWalkRunBlend()
{
	if (Gait == EStarve_Gait::Walking)
		return 0.f;
	return 1.f;
}

float UStarveCharacterAnimInstance::CalculateStrideBlend()
{
	float curvevalue_1 = StrideBlend_N_Walk->GetFloatValue(Speed);
	float curvevalue_2 = StrideBlend_N_Run->GetFloatValue(Speed);
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f);
	//站立状态下walk到run的过渡值
	curvevalue_1 = UKismetMathLibrary::Lerp(curvevalue_1, curvevalue_2, alpha);

	curvevalue_2 = StrideBlend_C_Walk->GetFloatValue(Speed);
	
	//站立到crouch的混合
	alpha = GetCurveValue(FName("Basepose_CLF"));
	alpha = UKismetMathLibrary::Lerp(curvevalue_1, curvevalue_2, alpha);

	return alpha;
}

float UStarveCharacterAnimInstance::GetAnimCurveClamp(FName CurveName, float Bias, float ClampMin, float ClampMax)
{
	float value = this->GetCurveValue(CurveName);
	value = FMath::Clamp<float>(value + Bias, ClampMin, ClampMax);
	return value;
}

float UStarveCharacterAnimInstance::CalculateStandingPlayRate()
{	
	//计算当前速度与动画速度的比值，并进行差值
	/*Walk->Run*/
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.f, 0.f, 1.f);
	float value = UKismetMathLibrary::Lerp(Speed / AnimatedWalkSpeed, Speed / AnimatedRunSpeed, alpha);

	/*Walk/Run->Sprint*/
	alpha = GetAnimCurveClamp(FName("Weight_Gait"), -2.f, 0.f, 1.f);
	value = UKismetMathLibrary::Lerp(value, Speed / AnimatedSprintSpeed, alpha);
	
	//缩放步距大小，受人物本身步距大小和人物世界缩放影响
	alpha = GetOwningComponent()->GetComponentScale().Z;
	alpha = FMath::Clamp<float>((value / StrideBlend) / alpha, 0.f, 3.f);
	return alpha;

}

float UStarveCharacterAnimInstance::CalculateCrouchingPlayRate()
{
	float alpha = Speed / AnimatedCrouchSpeed;

	//下蹲缩放步距大小，受人物本身步距大小和人物世界缩放影响
	alpha = (alpha / StrideBlend) / (GetOwningComponent()->GetComponentScale().Z);

	return FMath::Clamp<float>(alpha, 0.f, 2.f);
}

EMovementDirecction UStarveCharacterAnimInstance::CalculateMovementDirection()
{
	switch (Gait)
	{
		case EStarve_Gait::Walking:
		case EStarve_Gait::Running: {
			switch (RotationMode)
			{
				case EStarve_RotationMode::VelocityDirection: {
					return EMovementDirecction::Forward;
					
				}
				case EStarve_RotationMode::LookingDirection:
				case EStarve_RotationMode::Aiming: {
					//通过Rotation差量计算MovementDirection
					float angle = UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(), AimingRatation).Yaw;
					return CalculateQuadrant(MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f, angle);
				}
			}
		}
		case EStarve_Gait::Sprinting: {
			return EMovementDirecction::Forward;
			
		}
	}

	return EMovementDirecction::Forward;
}


EMovementDirecction UStarveCharacterAnimInstance::CalculateQuadrant(EMovementDirecction CurrentMD, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angle)
{
	//前或后
	bool direction = CurrentMD != EMovementDirecction::Forward || CurrentMD != EMovementDirecction::Backward;
	//是否在前边 FL~FR 范围内
	bool in_range = AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, direction);
	if (in_range) {
		return EMovementDirecction::Forward;
	}
	else {
		//左或右
		direction = CurrentMD != EMovementDirecction::Left || CurrentMD != EMovementDirecction::Right;
		//是否在右边 FR~BR 范围内
		in_range = AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, direction);
		if (in_range) {
			return EMovementDirecction::Right;
		}
		else {
			//是否在左边 BL~FL 范围内，不在前三者代表后面
			in_range = AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, direction);
			return in_range ? EMovementDirecction::Left : EMovementDirecction::Backward;
		}
	}

	//上面的主要目的是将前后左右360分成四份进行判断
	return EMovementDirecction::Forward;
}

bool UStarveCharacterAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool bIncreaseBuffer)
{
	float min = bIncreaseBuffer ? MinAngle - Buffer : MinAngle + Buffer;
	float max = bIncreaseBuffer ? MaxAngle + Buffer : MaxAngle - Buffer;
	//是否在这个范围，后面两个bool表示是否包含边界
	return UKismetMathLibrary::InRange_FloatFloat(Angle, min, max, true, true);
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_F(UAnimNotify* Notify)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("AnimNotify_Hips_F"), true, false, FLinearColor::Blue, 5.f);
	TrackedHipsDirection = EHipsDirection::F;
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_B(UAnimNotify* Notify)
{
	TrackedHipsDirection = EHipsDirection::B;
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_LF(UAnimNotify* Notify)
{
	TrackedHipsDirection = EHipsDirection::LF;
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_LB(UAnimNotify* Notify)
{
	TrackedHipsDirection = EHipsDirection::LB;
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_RF(UAnimNotify* Notify)
{
	TrackedHipsDirection = EHipsDirection::RF;
}

void UStarveCharacterAnimInstance::AnimNotify_Hips_RB(UAnimNotify* Notify)
{
	TrackedHipsDirection = EHipsDirection::RB;
}

void UStarveCharacterAnimInstance::AnimNotify_Pivot(UAnimNotify* Notify)
{
	bPivot = Speed < TriggerPivotLimitSpeed;

	//使用Delay节点
	// 创建一个LatentInfo, 用不到Linkage直接传0(不能是-1)， UUID随机生成，指定延迟后要执行的函数ExecutionFunction，ExecutionFunction的归属者this
	const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("PivotDelayFinish"), this);
	UKismetSystemLibrary::Delay(this, 0.1f, LatentInfo);
}

void UStarveCharacterAnimInstance::PivotDelayFinish()
{
	//UKismetSystemLibrary::PrintString(this, TEXT("PivotDelayFinish"), true, false, FLinearColor::Blue, 5.f);
	bPivot = false;
}

bool UStarveCharacterAnimInstance::CanRotateInPlace()
{
	return RotationMode == EStarve_RotationMode::Aiming || ViewMode == EStarve_ViewMode::FirstPerson;
}

bool UStarveCharacterAnimInstance::CanTurnInPlace()
{
	return RotationMode == EStarve_RotationMode::LookingDirection && ViewMode == EStarve_ViewMode::ThirdPerson && GetCurveValue(FName("Enable_Transition")) > 0.99f;
}

bool UStarveCharacterAnimInstance::CanDynamicTransition()
{
	return GetCurveValue(FName("Enable_Transition")) >= 0.99f;
}

void UStarveCharacterAnimInstance::TurnInPlaceCheck()
{
	/*1、检查瞄准角度是否在转弯检查最小角度之外，以及目标横摆角速率是否低于目标横摆率限制。如果是，请开始计算经过的延迟时间。如果没有，请重置“经过的延迟时间”。这确保了在转变到位之前，条件在一段持续的时间内保持不变。*/
	if (FMath::Abs(AimingAngle.X) > TurnCheckMinAngle && AimYawRate < AimYawRateLimit) {
		ElapsedDelayTime += DeltaTimeX;
		//通过映射角度获得转向开始前延迟的时间（角度小的延迟较大，角度大的延迟较小）
		float aimangle_delaytime = FMath::GetMappedRangeValueClamped(FVector2D(TurnCheckMinAngle, 180.f), FVector2D(MinAngleDelay, MaxAngleDelay), FMath::Abs(AimingAngle.X));
		
		if (ElapsedDelayTime > aimangle_delaytime) {
			TurnInPlace(FRotator(0, AimingRatation.Yaw, 0), 1.f, 0.f, false);
		}
	}
	else {
		ElapsedDelayTime = 0.f;
	}
}

void UStarveCharacterAnimInstance::UpdateAimingValues()
{
	/*1、获得旋转中间过渡值*/
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation, AimingRatation, DeltaTimeX, SmoothedAimingRotationInterpSpeed);

	/*2、通过Actor的Rotation和AimingRotation计算AimAngle以及中间过渡的AimAngle*/
	FRotator actor_rotation = CharacterRef->GetActorRotation();
	FRotator delta_rotation = UKismetMathLibrary::NormalizedDeltaRotator(AimingRatation, actor_rotation);
	AimingAngle.X = delta_rotation.Yaw;
	AimingAngle.Y = delta_rotation.Pitch;

	delta_rotation = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, actor_rotation);
	SmoothedAimingAngle.X = delta_rotation.Yaw;
	SmoothedAimingAngle.Y = delta_rotation.Pitch;

	/*3、Looking模式个Aiming模式下通过AimAngle即AimingRotation的Pitch限制AimSweepTime*/
	/*同时使用AimingAngle的X值即Yaw值除以Spine + Pelvis骨骼的总数获得面向摄像机朝向需要旋转的Yaw值*/
	if (RotationMode == EStarve_RotationMode::LookingDirection || RotationMode == EStarve_RotationMode::Aiming) {
		AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.f, 90.f), FVector2D(1.f, 0.f), AimingAngle.Y);
		SpineRotation = FRotator(0.f, AimingAngle.X / 4.f, 0.f);
	}
	else if(RotationMode == EStarve_RotationMode::VelocityDirection)
	{
	/*4、速度模式下获得MovementInput和Actor之间的Rotation差量，并将其映射到0~1之间，该值用于*/
		if (bHasMovementInput) {
			delta_rotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementInput.ToOrientationRotator(), actor_rotation);
			float mapyaw = FMath::GetMappedRangeValueClamped(FVector2D(-180.f, 180.f), FVector2D(0.f, 1.f), delta_rotation.Yaw);
			InputYawOffsetTime = FMath::FInterpTo(InputYawOffsetTime, mapyaw, DeltaTimeX, InputYawOffsetInterpSpeed);
		}
	}

	/*5.将SmoothAimingAngle对应的X（Yaw）值分别限制成3个值，这3个值将用于AimOffset，*/
	LeftYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(0.5f, 0.f), FMath::Abs(SmoothedAimingAngle.X));
	RightYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(0.5f, 1.f), FMath::Abs(SmoothedAimingAngle.X));
	ForwardYawTime = FMath::GetMappedRangeValueClamped(FVector2D(-180.f, 180.f), FVector2D(0.f, 1.f), SmoothedAimingAngle.X);
}

void UStarveCharacterAnimInstance::TurnInPlace(const FRotator& TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent)
{
	/*1、计算旋转角度*/
	float turnangle = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRef->GetActorRotation()).Yaw;

	/*2、根据判断获得正确的 FTurnInPlace_Asset（决定播放哪个旋转的动画以及其它的一些信息，是在该类的生成的子类赋值的）*/
	//大于130进行180旋转，小于进行90旋转
	FTurnInPlace_Asset TargetTurnAsset;
	if (FMath::Abs(turnangle) < Turn180Threshold) {
		//判断左右
		if (turnangle < 0) {
			TargetTurnAsset = Stance == EStarve_Stance::Standing ? N_TurnIP_L90 : CLF_TurnIP_L90;
		}
		else {
			TargetTurnAsset = Stance == EStarve_Stance::Standing ? N_TurnIP_R90 : CLF_TurnIP_R90;
		}
	}
	else {
		if (turnangle < 0) {
			TargetTurnAsset = Stance == EStarve_Stance::Standing ? N_TurnIP_L180 : CLF_TurnIP_L180;
		}
		else {
			TargetTurnAsset = Stance == EStarve_Stance::Standing ? N_TurnIP_R180 : CLF_TurnIP_R180;
		}
	}

	/*3、根据上一步获取到的信息进行播放操作*/
	//IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName)判断是不是正在播放这个动画
	if (bOverrideCurrent || !IsPlayingSlotAnimation(TargetTurnAsset.Animation, TargetTurnAsset.SlotName)) {
		//将插槽动画作为蒙太奇播放，
		PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation, TargetTurnAsset.SlotName, 0.2f, 0.2f, TargetTurnAsset.PlayRate * PlayRateScale, 1, 0.f, StartTime);
		RotationScale = TargetTurnAsset.bScaleTurnAngle ? 
			(turnangle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale : 
			TargetTurnAsset.PlayRate * PlayRateScale;
	}

}

void UStarveCharacterAnimInstance::RotateInPlaceCheck()
{
	/*1、根据AimingAngle与Rotate的阈值进行比较判断Rotate是向左还是向右*/
	Rotate_L = AimingAngle.X < RotateMinThreshold;
	Rotate_R = AimingAngle.X > RotateMaxThreshold;

	/*2、RotateRate将用于修改曲线RotationAmount*/
	if (Rotate_L || Rotate_R) {
		RotateRate = FMath::GetMappedRangeValueClamped(FVector2D(AimYawRateMinRange, AimYawRateMaxRange), FVector2D(MinPlayRate, MinPlayRate), AimYawRate);
	}
}


void UStarveCharacterAnimInstance::I_Jumped()
{
	Jumped = true;
	JumpPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 600.f), FVector2D(1.2f, 1.5f), Speed);
	//跳跃后重置Jumped
	const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("I_JumpedDelayFinish"), this);
	UKismetSystemLibrary::Delay(this, 0.1f, LatentInfo);
}

void UStarveCharacterAnimInstance::I_SetGroundedEntryState(EGroundedEntryState GroundEntryState)
{
	GroundedEntryState = GroundEntryState;
}

void UStarveCharacterAnimInstance::I_SetOverlayOverrideState(int overlayOverrideState)
{
	this->OverlayOverrideState = overlayOverrideState;
}

void UStarveCharacterAnimInstance::I_JumpedDelayFinish()
{
	//UKismetSystemLibrary::PrintString(this, TEXT("I_JumpedDelayFinish"), true, false, FLinearColor::Blue, 5.f);
	Jumped = false;
}

void UStarveCharacterAnimInstance::UpdateLayerValues()
{
	/*1.通过获取Mask_AimOffset曲线值来获取AimOffset权重值*/
	Enable_AimOffset = FMath::Lerp(1.f, 0.f, GetCurveValue(FName("Mask_AimOffset")));

	/*2.获得基础姿势权重*/
	BasePose_N = GetCurveValue(FName("Basepose_N"));
	BasePose_CLF = GetCurveValue(FName("Basepose_CLF"));

	/*3.获取身体各部位的Additive Amount*/
	Spine_Add = GetCurveValue(FName("Layering_Spine_Add"));
	Head_Add = GetCurveValue(FName("Layering_Head_Add"));
	Arm_L_Add = GetCurveValue(FName("Layering_Arm_L_Add"));
	Arm_R_Add = GetCurveValue(FName("Layering_Arm_R_Add"));

	/*4.获得手部Override权重*/
	Hand_L = GetCurveValue(FName("Layering_Hand_L"));
	Hand_R = GetCurveValue(FName("Layering_Hand_R"));

	/*5.通过Enable_HandIK_L和Layering_Arm_L获得是否开启手部IK*/
	Enable_HandIK_L = FMath::Lerp(0.f, GetCurveValue(FName("Enable_HandIK_L")), GetCurveValue(FName("Layering_Arm_L")));
	Enable_HandIK_R = FMath::Lerp(0.f, GetCurveValue(FName("Enable_HandIK_R")), GetCurveValue(FName("Layering_Arm_R")));

	/*6.设置手部骨骼是在LocalSpace还是在MeshSpace中混合，除非LocalSpace的权值为1，不然MeshSpace的权值始终为1，即默认是在MeshSpace下混合手臂的*/
	Arm_L_LS = GetCurveValue(FName("Layering_Arm_L_LS"));
	Arm_R_LS = GetCurveValue(FName("Layering_Arm_R_LS"));
	Arm_L_MS = (float)(1 - FMath::FloorToInt(Arm_L_LS));
	Arm_R_MS = (float)(1- FMath::FloorToInt(Arm_R_LS));
}

void UStarveCharacterAnimInstance::UpdateFootIK()
{
	FVector FootOffset_L_Target(0.f,0.f,0.f);/*左脚偏移目标量*/
	FVector FootOffset_R_Target(0.f, 0.f, 0.f);/*右脚偏移目标量*/
	/*两个脚步锁定，左右脚*/
	SetFootLocking(FName("Enable_FootIK_L"), FName("FootLock_L"), FName("ik_foot_l"), FootLock_L_Alpha, FootLock_L_Location, FootLock_L_Rotation);
	SetFootLocking(FName("Enable_FootIK_R"), FName("FootLock_R"), FName("ik_foot_r"), FootLock_R_Alpha, FootLock_R_Location, FootLock_R_Rotation);
	
	//将空中和非空中分开，在非空中时进行IK处理
	if (MovementState == EStarve_MovementState::None || MovementState == EStarve_MovementState::Grounded || MovementState == EStarve_MovementState::Mantling) {
		SetFootOffsets(FName("Enable_FootIK_L"), FName("ik_foot_l"), FName("root"), FootOffset_L_Target, FootOffset_L_Location, FootOffset_L_Rotation);
		SetFootOffsets(FName("Enable_FootIK_R"), FName("ik_foot_r"), FName("root"), FootOffset_R_Target, FootOffset_R_Location, FootOffset_R_Rotation);
		SetPelvisIKOffset(FootOffset_L_Target, FootOffset_R_Target);
	}
	else if (MovementState == EStarve_MovementState::InAir) {
		SetPelvisIKOffset(FVector(0.f, 0.f, 0.f), FVector(0.f, 0.f, 0.f));
		ResetIKOffsets();
	}
}

void UStarveCharacterAnimInstance::SetFootLocking(FName EnableFootIKCurve, FName FootLockCuve, FName FootIKBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation)
{
	/*先进行是否进行脚步锁定的判断*/
	float curvevalue = GetCurveValue(EnableFootIKCurve);
	if (curvevalue > 0.f) {
		/*1、获得脚步锁定的曲线值*/
		curvevalue = GetCurveValue(FootLockCuve);

		/*2、只有当FootLock的曲线值等于1或者它小于当前的FootLock值时才更新FootLock的Alpha值，目的是使得FootLock只能混合退出，而不能混合进入*/
		if (curvevalue >= 0.99f || curvevalue < CurrentFootLockAlpha) {
			CurrentFootLockAlpha = curvevalue;
		}

		/*3、脚步锁定曲线值等于1，使用组件坐空间给FootLock的Location和Rotation赋值*/
		if (CurrentFootLockAlpha >= 0.99f) {
			/*获取组件坐标系下的Transform*/
			FTransform rtscomponent = GetOwningComponent()->GetSocketTransform(FootIKBone,ERelativeTransformSpace::RTS_Component);
			CurrentFootLockLocation = rtscomponent.GetLocation();
			CurrentFootLockRotation = rtscomponent.Rotator();
		}

		/*4、如果脚步锁定的值大于0，则当胶囊体移动时更新FootLockOffset以保持脚步固定*/
		if (CurrentFootLockAlpha > 0.f) {
			SetFootLockOffsets(CurrentFootLockLocation, CurrentFootLockRotation);
		}
	}
}

void UStarveCharacterAnimInstance::SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation)
{
	/*1.获取当前帧跟上一帧的Rotation差量,确定脚保持在地面上应该要旋转多少度*/
	UCharacterMovementComponent* charactermovecomp = CharacterRef->GetCharacterMovement();
	FRotator rotationdifferent(0.f);
	if (charactermovecomp->IsMovingOnGround()) {
		rotationdifferent = charactermovecomp->GetLastUpdateRotation();
		rotationdifferent = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRef->GetActorRotation(), rotationdifferent);
	}

	/*2.获取当前帧Location与上一帧相比的差量,获得保证脚保持在原地的Location偏移量*/
	//Velocity * UGameplayStatics::GetWorldDeltaSeconds(this)是表示速度方向一帧前进的距离
	FVector locationdifferent = GetOwningComponent()->GetComponentRotation().UnrotateVector(Velocity * UGameplayStatics::GetWorldDeltaSeconds(this));

	/*3.通过速度方向的不同给传进来的引用Location在旋转方向上进行赋值*/
	LocalLocation = UKismetMathLibrary::RotateAngleAxis(LocalLocation - locationdifferent, rotationdifferent.Yaw, FVector(0.f, 0.f, -1.f));
	
	/*4.给LocalRotation进行赋值*/
	LocalRotation = UKismetMathLibrary::NormalizedDeltaRotator(LocalRotation, rotationdifferent);
}

void UStarveCharacterAnimInstance::SetFootOffsets(FName EnableFootIKCurve, FName IK_FootBone, FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset)
{
	float curvevalue = GetCurveValue(EnableFootIKCurve);
	if (curvevalue > 0.f) {
		
		USkeletalMeshComponent* owncomp = GetOwningComponent();
		/*取脚步位置的XY值，取root骨骼的Z值表示双脚的位置*/
		FVector linestart = owncomp->GetSocketLocation(IK_FootBone);
		FVector lineend = owncomp->GetSocketLocation(RootBone);

		FVector ikfootfloorlocation = FVector(linestart.X, linestart.Y, lineend.Z);

		/*进行射线检测*/
		linestart = ikfootfloorlocation + FVector(0.f, 0.f, IKTraceDistanceAboveFoot);
		lineend = ikfootfloorlocation - FVector(0.f, 0.f, IKTraceDistanceBlowFoot);
		EDrawDebugTrace::Type debugtrace = GetDebugTraceType(EDrawDebugTrace::ForOneFrame);
		FHitResult hitresult;/*保存碰撞信息*/

		//FVector impactpoint;/*碰撞点的位置*/
		//FVector impactnormal;/*碰撞点的法线*/
		//上面两个变量使用linestart和lineend代替，重复利用变量，减少内存
		
		FRotator targetrotationoffset(0.f);/*目标骨骼的目标旋转角度*/

		/*1.从脚部位置向下追踪以找到几何体。如果曲面是可行走的，保存“碰撞位置”和“法线”*/
		bool bhit = UKismetSystemLibrary::LineTraceSingle(this, linestart, lineend, TraceTypeQuery1,false, {}, 
			debugtrace,hitresult, true);

		if (CharacterRef->GetCharacterMovement()->IsWalkable(hitresult)) {
			linestart = hitresult.ImpactPoint;
			lineend = hitresult.ImpactNormal;
			/*CurrentLocationTarget赋值，通过碰撞点与预期点位置的偏差进行赋值*/
			CurrentLocationTarget = (lineend * FootHeight + linestart) - (ikfootfloorlocation + FVector(0.f, 0.f, 1.f) * FootHeight);
			/*反正切获得Rotation角度*/
			float rolldegree = UKismetMathLibrary::DegAtan2(lineend.Y, lineend.Z);
			float pitchdegree = -1.f * UKismetMathLibrary::DegAtan2(lineend.X, lineend.Z);
			targetrotationoffset = FRotator(pitchdegree, 0.f, rolldegree);
		}
	
		/*2.将“当前位置偏移”插值到新的目标值。根据新目标是高于还是低于当前目标，以不同的速度进行插值。防止脚步瞬移*/
		curvevalue = CurrentLocationOffset.Z > CurrentLocationTarget.Z ? 30.f : 15.f;
		CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset, CurrentLocationTarget, DeltaTimeX, curvevalue);

		/*3.将当前旋转偏移赋值到目标旋转*/
		CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset, targetrotationoffset, DeltaTimeX, 30.f);
	}
	else {
		/*没有IK修正，将偏移设为0*/
		CurrentLocationOffset = FVector(0.f, 0.f, 0.f);
		CurrentRotationOffset = FRotator(0.f, 0.f, 0.f);
	}
}

void UStarveCharacterAnimInstance::SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	/*对左右脚Enable_FootIK的曲线值取平均值作为PelvisAlpha*/
	PelvisAlpha = (GetCurveValue(FName("Enable_FootIK_L")) + GetCurveValue(FName("Enable_FootIK_R"))) / 2;
	if (PelvisAlpha > 0.f) {
		/*1.根据左右脚偏移的高度来选取合适的PelvisOffset，选取较低的那个*/
		FVector pelvistarget = FootOffset_L_Target.Z < FootOffset_R_Target.Z ? FootOffset_L_Target : FootOffset_R_Target;
		
		/*2.比较pelvistarget跟PelvisOffset的Z值来决定不同的插值速度*/
		float interpspeed = pelvistarget.Z > PelvisOffset.Z ? 10.f : 15.f;
		PelvisOffset = FMath::VInterpTo(PelvisOffset, pelvistarget, DeltaTimeX, interpspeed);
	}
	else
	{
		PelvisOffset = FVector(0.f, 0.f, 0.f);
	}
}

void UStarveCharacterAnimInstance::ResetIKOffsets()
{
	/*Location 偏移归零*/
	FootOffset_L_Location = FMath::VInterpTo(FootOffset_L_Location, FVector(0.f, 0.f, 0.f), DeltaTimeX, 15.f);
	FootOffset_R_Location = FMath::VInterpTo(FootOffset_R_Location, FVector(0.f, 0.f, 0.f), DeltaTimeX, 15.f);
	/*Rotation 偏移归零*/
	FootOffset_L_Rotation = FMath::RInterpTo(FootOffset_L_Rotation, FRotator(0.f, 0.f, 0.f), DeltaTimeX, 15.f);
	FootOffset_R_Rotation = FMath::RInterpTo(FootOffset_R_Rotation, FRotator(0.f, 0.f, 0.f), DeltaTimeX, 15.f);
}

void UStarveCharacterAnimInstance::DynamicTransitionCheck()
{
	/*左脚*/
	//获得两个插槽间的距离
	FDynamicMontageParams Parameters(lefttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.8f);

	float leftdistance = GetDistanceBetweenTwoSockets(GetOwningComponent(), FName("ik_foot_l"), ERelativeTransformSpace::RTS_Component, FName("VB foot_target_l"), ERelativeTransformSpace::RTS_Component, false, 0.f, 0.f, 0.f, 0.f);
	if (leftdistance > 8.0f) {
		PlayDynamicTransition(0.1f, Parameters);
	}

	/*右脚*/
	float rightdistance = GetDistanceBetweenTwoSockets(GetOwningComponent(), FName("ik_foot_r"), ERelativeTransformSpace::RTS_Component, FName("VB foot_target_r"), ERelativeTransformSpace::RTS_Component, false, 0.f, 0.f, 0.f, 0.f);
	if (rightdistance > 8.0f) {
		Parameters.Animation = righttransitionanimtion;
		PlayDynamicTransition(0.1f, Parameters);
	}
}

void UStarveCharacterAnimInstance::PlayTransition(FDynamicMontageParams Parameters)
{
	PlaySlotAnimationAsDynamicMontage(Parameters.Animation, FName("Grounded_Slot"), Parameters.BlendInTime, Parameters.BlendOutTime, Parameters.PlayRate, 1, 0.f, Parameters.StartTime);
}

void UStarveCharacterAnimInstance::PlayDynamicTransition(float ReTriggerDelay, FDynamicMontageParams Parameters)
{
	if (bCanPlayDynamicTransition)
	{
		bCanPlayDynamicTransition = false;

		// Play Dynamic Additive Transition Animation
		PlayTransition(Parameters);

		//UWorld* World = GetWorld();
		//check(World);
		//World->GetTimerManager().SetTimer(PlayDynamicTransitionTimer, this,
		//	&UStarveCharacterAnimInstance::PlayDynamicTransitionDelay,
		//	ReTriggerDelay, false);

		//使用Delay节点
		// 创建一个LatentInfo, 用不到Linkage直接传0(不能是-1)， UUID随机生成，指定延迟后要执行的函数ExecutionFunction，ExecutionFunction的归属者this
		const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("PlayDynamicTransitionDelay"), this);
		UKismetSystemLibrary::Delay(this, 0.1f, LatentInfo);
	}
}

void UStarveCharacterAnimInstance::PlayDynamicTransitionDelay()
{
	bCanPlayDynamicTransition = true;
}

float UStarveCharacterAnimInstance::GetDistanceBetweenTwoSockets(const USkeletalMeshComponent* Component, const FName SocketOrBoneNameA, ERelativeTransformSpace SocketSpaceA, const FName SocketOrBoneNameB, ERelativeTransformSpace SocketSpaceB, bool bRemapRange, float InRangeMin, float InRangeMax, float OutRangeMin, float OutRangeMax)
{
	if (Component && SocketOrBoneNameA != NAME_None && SocketOrBoneNameB != NAME_None)
	{
		FTransform SocketTransformA = Component->GetSocketTransform(SocketOrBoneNameA, SocketSpaceA);
		FTransform SocketTransformB = Component->GetSocketTransform(SocketOrBoneNameB, SocketSpaceB);

		float Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();

		if (bRemapRange)
		{
			return FMath::GetMappedRangeValueClamped(FVector2D(InRangeMin, InRangeMax), FVector2D(OutRangeMin, OutRangeMax), Distance);
		}
		else
		{
			return Distance;
		}
	}

	return 0.f;
}

void UStarveCharacterAnimInstance::AnimNotify_N_Stop_L(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(stopldowntransitionanimtion, 0.2f, 0.2f, 1.5f, 0.4f));
}

void UStarveCharacterAnimInstance::AnimNotify_N_Stop_R(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(stoprdowntransitionanimtion, 0.2f, 0.2f, 1.5f, 0.4f));
}

void UStarveCharacterAnimInstance::AnimNotify_StopTransition(UAnimNotify* Notify)
{
	StopSlotAnimation(0.2f, FName("Grounded_Slot"));
	StopSlotAnimation(0.2f, FName("N_Turn_Rotate"));
	StopSlotAnimation(0.2f, FName("CLF_Turn_Rotate"));
}

void UStarveCharacterAnimInstance::UpdateInAirValues()
{
	/*1.获得下落方向的速度*/
	FallSpeed = Velocity.Z;

	/*2.计算落地的混合度*/
	LandPredicion = CalculateLandPrediction();

	/*3.计算空中偏移量，并使当前偏移转至目标偏移*/
	FLeanAmount target = CalculateInAirLeanAmount();
	LeanAmount = InterpLeanAmount(LeanAmount, target, InAirLeanInterpSpeed, DeltaTimeX);
}

float UStarveCharacterAnimInstance::CalculateLandPrediction()
{
	if (FallSpeed < -200.f) {
		//进行胶囊体检测
		UCapsuleComponent* capsule = CharacterRef->GetCapsuleComponent();
		FVector start = capsule->GetComponentLocation();

		FVector unsafenormal = FVector(Velocity.X, Velocity.Y, FMath::Clamp(Velocity.Z, -4000.f, -200.f)).GetUnsafeNormal();
		
		FVector end = start + unsafenormal * FMath::GetMappedRangeValueClamped(FVector2D(0.f, -4000.f), FVector2D(50.f, 2000.f), Velocity.Z);

		FHitResult hitresult;

		UKismetSystemLibrary::CapsuleTraceSingleByProfile(this,start,end,capsule->GetScaledCapsuleRadius(),
			capsule->GetScaledCapsuleHalfHeight(), FName("StarveCharacter"), false, {},EDrawDebugTrace::ForOneFrame,
			hitresult,true);

		if (CharacterRef->GetCharacterMovement()->IsWalkable(hitresult) && hitresult.bBlockingHit) {
			float curvevalue = LandPredictionCurve->GetFloatValue(hitresult.Time);
			return FMath::Lerp<float>(curvevalue, 0.f, GetCurveValue(FName("Mask_LandPrediction")));
		}
	}

	return 0.f;
}

FLeanAmount UStarveCharacterAnimInstance::CalculateInAirLeanAmount()
{
	FVector unrotatevector = (CharacterRef->GetActorRotation().UnrotateVector(Velocity)) / 350.f;

	FVector2D vector2d = FVector2D(unrotatevector.Y, unrotatevector.X) * LeanInAirCurve->GetFloatValue(FallSpeed);

	return FLeanAmount(vector2d.X, vector2d.Y);
}

void UStarveCharacterAnimInstance::AnimNotify_LandToIdle(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.1f, 0.2f, 1.4f, 0.f));
}

void UStarveCharacterAnimInstance::AnimNotify_To_CLF_Stop(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.2f));
}

void UStarveCharacterAnimInstance::AnimNotify_To_N_QuickStop(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(righttransitionanimtion, 0.1f, 0.2f, 1.75f, 0.3f));
}

void UStarveCharacterAnimInstance::AnimNotify_RollToIdle(UAnimNotify* Notify)
{
	PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.2f));
}


void UStarveCharacterAnimInstance::AnimNotify_Reset_GroundedEntryState(UAnimNotify* Notify)
{
	GroundedEntryState = EGroundedEntryState::None;
}

void UStarveCharacterAnimInstance::AnimNotify_BowRelaxedToReady(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(righttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_BowReadyToRelaxed(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(righttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_RifleRelaxedToReady(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(righttransitionanimtion, 0.2f, 0.2f, 1.75f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_RifleReadyToRelaxed(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(righttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_Pistol_1H_RelaxedToReady(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.75f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_Pistol_1H_ReadyToRelaxed(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_Pistol_2H_RelaxedToReady(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.75f, 0.3f));
	}
}

void UStarveCharacterAnimInstance::AnimNotify_Pistol_2H_ReadyToRelaxed(UAnimNotify* Notify)
{
	if (CanOverlayTransition()) {
		PlayTransition(FDynamicMontageParams(lefttransitionanimtion, 0.2f, 0.2f, 1.5f, 0.3f));
	}
}

bool UStarveCharacterAnimInstance::CanOverlayTransition()
{
	return Stance == EStarve_Stance::Standing && !bShouldMove;
}

void UStarveCharacterAnimInstance::UpdateRagdollValues()
{
	FVector Ragdollvelocity = GetOwningComponent()->GetPhysicsLinearVelocity(FName("root"));
	FlailRate = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1000.f), FVector2D(0.f, 1.f), Ragdollvelocity.Size());
}

EDrawDebugTrace::Type UStarveCharacterAnimInstance::GetDebugTraceType(EDrawDebugTrace::Type ShowTraceType)
{
	if (Cast<IStarve_ControllerInterface>(UGameplayStatics::GetPlayerController(this, 0))->I_ShowTraces()) {
		return ShowTraceType;
	}
	return EDrawDebugTrace::None;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/StarveCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Interfaces/Starve_CharacterInterface.h"

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

}

void UStarveCharacterAnimInstance::NativeInitializeAnimation()
{
	APawn* pawn = TryGetPawnOwner();
	if (IsValid(pawn)) {
		this->CharacterRef = Cast<ACharacter>(pawn);
	}
}

void UStarveCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	this->DeltaTimeX = DeltaSeconds;
	if (DeltaTimeX != 0.f) {
		if (IsValid(CharacterRef)) {
			UpdateCharacterInfo();
			UpdateAimingValues();

			switch (MovementState)
			{
				case EStarve_MovementState::Grounded:{
					bShouldMove = ShouldMoveCheck();
					EDoWhtileReturn dwr = DoWhile(bShouldMove, bDoOnce);
					switch (dwr){
						//角色一直处于Move状态
						case EDoWhtileReturn::WhileTrue:{
							UpdateMovementValues();
							UpdateRotationValues();
							break;
						}

						//角色一直处于非Move状态
						case EDoWhtileReturn::WhileFalse: {
							/*第一步，是否可以原地转头*/
							if (CanRotateInPlace()) {

							}


							/*第二步，是否能够原地转向*/
							if (CanTurnInPlace()) {
								TurnInPlaceCheck();
							}

							/*第三步，是否动态过渡*/
							if (CanDynamicTransition()) {

							}
							break;
						}

						//角色从非move转到Move
						case EDoWhtileReturn::ChangeToTrue:
							break;

						//角色从move转到非Move
						case EDoWhtileReturn::ChangeToFalse:
							break;
					}
					bDoOnce = bShouldMove;
					break;
				}
				//case EStarve_MovementState::InAir:
				//	break;
				//case EStarve_MovementState::Ragdoll:
				//	break;
				/*default:
					break;*/
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
	/*第一步*/
	//利用差值过渡的方式获得并设置FVelocityBlend
	FVelocityBlend vb = CalculateVelocityBlend();
	VelocityBlend = InterpVelocityBlend(VelocityBlend, vb, VelocityBlendInterpSpeed, DeltaTimeX);

	/*第二步*/
	//获得DiagonalScaleAmount，用于根骨骼位置修正
	DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	/*第三步*/
	/*利用加速度获取偏移量，来实现速度变换的平滑过渡*/
	//获得局部坐标系下的单位加速度向量
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	//LeanAmount平滑过渡
	FLeanAmount targetlean = FLeanAmount(RelativeAccelerationAmount.Y, RelativeAccelerationAmount.X);
	LeanAmount = InterpLeanAmount(LeanAmount, targetlean, GroundedLeanInterpSpeed, DeltaTimeX);

	/*第四步*/
	//进行一些数据的更新计算
	WalkRunBlend = CalculateWalkRunBlend();
	StrideBlend = CalculateStrideBlend();
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();

}

void UStarveCharacterAnimInstance::UpdateRotationValues()
{
	/*第一步*/
	//计算运动方向
	MovementDirection = CalculateMovementDirection();

	/*第二步*/
	//设置YawOffset。这些值会影响动画图中的“YawOffset”曲线，
	//并用于偏移角色旋转以获得更自然的移动。曲线允许对偏移在每个移动方向上的行为进行精细控制。
	FRotator velocityrotator = Velocity.ToOrientationRotator();
	float deltarotatoryaw = UKismetMathLibrary::NormalizedDeltaRotator(velocityrotator, CharacterRef->GetControlRotation()).Yaw;
	FVector yawoffsetfb = YawOffsetFB->GetVectorValue(deltarotatoryaw);
	FVector yawoffsetlr = YawOffsetLR->GetVectorValue(deltarotatoryaw);
	FYaw = yawoffsetfb.X;
	BYaw = yawoffsetfb.Y;
	LYaw = yawoffsetlr.X;
	RYaw = yawoffsetlr.Y;
}

bool UStarveCharacterAnimInstance::ShouldMoveCheck()
{
	//前一个是输入判断，后一个是当你没有进行输入，但角色还处于减速状态
	return (bIsMoving && bHasMovementInput) || Speed > 150.f;
}

EDoWhtileReturn UStarveCharacterAnimInstance::DoWhile(bool bSMove, bool bLastShouldMove)
{
	EDoWhtileReturn dwr;
	if (bSMove) {
		dwr = (bLastShouldMove == bShouldMove) ? EDoWhtileReturn::WhileTrue : EDoWhtileReturn::ChangeToTrue;
	}
	else {
		dwr = (bLastShouldMove == bShouldMove) ? EDoWhtileReturn::WhileFalse : EDoWhtileReturn::ChangeToFalse;
	}
	return dwr;
}

FVelocityBlend UStarveCharacterAnimInstance::CalculateVelocityBlend()
{
	FVector norvelocity = UKismetMathLibrary::Normal(Velocity, 0.1f);
	FRotator characterrotation = CharacterRef->GetActorRotation();
	//不旋转向量，表示将人物的模型坐标用世界坐标表示出来，可以理解为将速度的处理放在自己模型的空间
	FVector l_rela_velo_dir = UKismetMathLibrary::LessLess_VectorRotator(norvelocity, characterrotation);
	
	//下面一步是为了判断当前速度方向在四向方向上的比例（因为是在地面上，所以忽略上下），方便后面六向混合
	float sum = FMath::Abs(l_rela_velo_dir.X) + FMath::Abs(l_rela_velo_dir.Y) + FMath::Abs(l_rela_velo_dir.Z);
	FVector rela_velo = l_rela_velo_dir / sum;

	FVelocityBlend vb;
	vb.F = FMath::Clamp(rela_velo.X, 0.f, 1.f);
	vb.B = FMath::Abs(FMath::Clamp(rela_velo.X, -1.f, 0.f));
	vb.L = FMath::Abs(FMath::Clamp(rela_velo.Y, -1.f, 0.f));
	vb.R = FMath::Clamp(rela_velo.Y, 0.f, 1.f);

	return vb;
}

FVelocityBlend UStarveCharacterAnimInstance::InterpVelocityBlend(FVelocityBlend Current, FVelocityBlend Target, float InterpSpeed, float DeltaTime)
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
	////点乘(UE中是将 | 当做点乘的符号,可以直接写成 A | B ）
	//float dot = FVector::DotProduct(Acceleration, Velocity);
	//if (dot > 0) {
	//	float maxacce = CharacterRef->GetCharacterMovement()->GetMaxAcceleration();
	//	//将加速度的最大值限制在maxacce
	//	FVector clamp_acce = Acceleration.GetClampedToMaxSize(maxacce);
	//	//返回一个局部坐标的加速度向量，限制了最大值
	//	return CharacterRef->GetActorRotation().UnrotateVector(clamp_acce / maxacce);
	//}
	//else {
	//  //点乘为负代表角色应该在减速，我们获取这个制动速度
	//	float maxacce = CharacterRef->GetCharacterMovement()->GetMaxBrakingDeceleration();
	//	//将加速度的最大值限制在maxacce
	//	FVector clamp_acce = Acceleration.GetClampedToMaxSize(maxacce);
	//	//返回一个局部坐标的加速度向量，限制了最大值
	//	return CharacterRef->GetActorRotation().UnrotateVector(clamp_acce / maxacce);
	//}

	//上面代码优化
	float dot = FVector::DotProduct(Acceleration, Velocity);
	
	UCharacterMovementComponent* cmc = CharacterRef->GetCharacterMovement();
	float maxacce = dot >0 ? cmc->GetMaxAcceleration(): cmc->GetMaxBrakingDeceleration();
	//将加速度的最大值限制在maxacce
	FVector clamp_acce = Acceleration.GetClampedToMaxSize(maxacce);
	//返回一个局部坐标的加速度向量，限制了最大值
	return CharacterRef->GetActorRotation().UnrotateVector(clamp_acce / maxacce);
}


FLeanAmount UStarveCharacterAnimInstance::InterpLeanAmount(FLeanAmount Current, FLeanAmount Target, float InterpSpeed, float DeltaTime)
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
	float stride_blend_nw = StrideBlend_N_Walk->GetFloatValue(Speed);
	float stride_blend_nr = StrideBlend_N_Run->GetFloatValue(Speed);
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f);
	//站立状态下walk到run的过渡值
	float w_t_r = UKismetMathLibrary::Lerp(stride_blend_nw, stride_blend_nr, alpha);

	float stride_blend_cw = StrideBlend_C_Walk->GetFloatValue(Speed);
	//站立到crouch的混合
	float alpha2 = GetCurveValue(FName("BasePose_CLF"));
	float s_t_c = UKismetMathLibrary::Lerp(w_t_r, stride_blend_cw, alpha2);

	return s_t_c;
}

float UStarveCharacterAnimInstance::GetAnimCurveClamp(FName CurveName, float Bias, float ClampMin, float ClampMax)
{
	float curvevalue = this->GetCurveValue(CurveName);
	float value = FMath::Clamp<float>(curvevalue + Bias, ClampMin, ClampMax);
	return value;
}

float UStarveCharacterAnimInstance::CalculateStandingPlayRate()
{	
	//计算当前速度与动画速度的比值，并进行差值
	float sw = Speed / AnimatedWalkSpeed;
	float sr = Speed / AnimatedRunSpeed;
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.f, 0.f, 1.f);
	float w2r = UKismetMathLibrary::Lerp(sw, sr, alpha);

	float ss = Speed / AnimatedSprintSpeed;
	float alpha2 = GetAnimCurveClamp(FName("Weight_Gait"), -2.f, 0.f, 1.f);
	float w2r2s = UKismetMathLibrary::Lerp(w2r, ss, alpha2);
	
	//缩放步距大小，受人物本身步距大小和人物世界缩放影响
	float worldscale = GetOwningComponent()->GetComponentScale().Z;
	float stridescale = (w2r2s / StrideBlend) / worldscale;
	float re = FMath::Clamp<float>(stridescale, 0.f, 3.f);
	return re;

}

float UStarveCharacterAnimInstance::CalculateCrouchingPlayRate()
{
	float cp = Speed / AnimatedCrouchSpeed;

	//下蹲缩放步距大小，受人物本身步距大小和人物世界缩放影响
	float stridescale = (cp / StrideBlend) / (GetOwningComponent()->K2_GetComponentScale().Z);

	return FMath::Clamp<float>(stridescale, 0.f, 2.f);
}

EMovementDirecction UStarveCharacterAnimInstance::CalculateMovementDirection()
{
	EMovementDirecction md = EMovementDirecction::Forward;

	switch (Gait)
	{
		case EStarve_Gait::Walking:
		case EStarve_Gait::Running: {
			switch (RotationMode)
			{
				case EStarve_RotationMode::VelocityDirection: {
					md = EMovementDirecction::Forward;
					break;
				}
				case EStarve_RotationMode::LookingDirection:
				case EStarve_RotationMode::Aiming: {
					FRotator velocityrotation = Velocity.ToOrientationRotator();
					//计算旋转差量
					float angle = UKismetMathLibrary::NormalizedDeltaRotator(velocityrotation, AimingRatation).Yaw;
					md = CalculateQuadrant(MovementDirection, 70.f, -70.f, 110.f, -110.f, 5.f, angle);
					break;
				}
			}
			break;
		}
		case EStarve_Gait::Sprinting: {
			md = EMovementDirecction::Forward;
			break;
		}
	}

	return md;
}


EMovementDirecction UStarveCharacterAnimInstance::CalculateQuadrant(EMovementDirecction CurrentMD, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angle)
{
	EMovementDirecction md;
	//前或后
	bool fob = CurrentMD != EMovementDirecction::Forward || CurrentMD != EMovementDirecction::Backward;
	//是否在前边 FL~FR 范围内
	bool b1 = AngleInRange(Angle, FL_Threshold, FR_Threshold, Buffer, fob);
	if (b1) {
		md = EMovementDirecction::Forward;
	}
	else {
		//左或右
		bool lor = CurrentMD != EMovementDirecction::Left || CurrentMD != EMovementDirecction::Right;
		//是否在右边 FR~BR 范围内
		bool b2 = AngleInRange(Angle, FR_Threshold, BR_Threshold, Buffer, lor);
		if (b2) {
			md = EMovementDirecction::Right;
		}
		else {
			//是否在左边 BL~FL 范围内，不在前三者代表后面
			bool b3 = AngleInRange(Angle, BL_Threshold, FL_Threshold, Buffer, lor);
			md = b3 ? EMovementDirecction::Left : EMovementDirecction::Backward;
		}
	}

	//上面的主要目的是将前后左右360分成四份进行判断
	return md;
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
	return RotationMode == EStarve_RotationMode::LookingDirection && ViewMode == EStarve_ViewMode::ThirdPerson && GetCurveValue(FName("Enable_Transition")) > 0.99;
}

bool UStarveCharacterAnimInstance::CanDynamicTransition()
{
	return GetCurveValue(FName("Enable_Transition")) == 1.f;
}

void UStarveCharacterAnimInstance::TurnInPlaceCheck()
{
	/*1、*/
	if (FMath::Abs(AimingAngle.X) > TurnCheckMinAngle && AimYawRate < AimYawRateLimit) {
		ElapsedDelayTime += DeltaTimeX;
		//通过映射角度获得延迟的时间
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

	/*2、计算人物Aimin状态下的 Angle 以及中间过渡的 SmoothedAngle*/
	FRotator char_r = CharacterRef->GetActorRotation();
	FRotator delta_r = UKismetMathLibrary::NormalizedDeltaRotator(AimingRatation, char_r);
	AimingAngle.X = delta_r.Yaw;
	AimingAngle.Y = delta_r.Pitch;

	FRotator delta_sr = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, char_r);
	SmoothedAimingAngle.X = delta_sr.Yaw;
	SmoothedAimingAngle.Y = delta_sr.Pitch;

	/*3、*/


}

void UStarveCharacterAnimInstance::TurnInPlace(const FRotator& TargetRotation, float PlayRateScale, float StartTime, bool bOverrideCurrent)
{
	/*1、计算旋转角度*/
	float turnangle = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRef->GetActorRotation()).Yaw;

	/*2、根据判断获得正确的 FTurnInPlace_Asset（决定播放哪个旋转的动画以及其它的一些信息，是在AnimationBlueprint中给定的）*/
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

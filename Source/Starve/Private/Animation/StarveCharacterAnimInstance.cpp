// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/StarveCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"

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

	static ConstructorHelpers::FObjectFinder<UCurveFloat> cf3(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/DiagonalScaleAmount.DiagonalScaleAmount'"));
	if (cf3.Succeeded()) {
		StrideBlend_N_Run = cf3.Object;
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

			switch (MovementState)
			{
			case EStarve_MovementState::Grounded:
				bShouldMove = ShouldMoveCheck();
				EDoWhtileReturn dwr = DoWhile(bShouldMove, bDoOnce);
				switch (dwr)
				{
					//角色一直处于Move状态
				case EDoWhtileReturn::WhileTrue:
					UpdateMovementValues();
					UpdateRotationValues();
					break;
					//角色一直处于非Move状态
				case EDoWhtileReturn::WhileFalse:
					break;
					//角色从非move转到Move
				case EDoWhtileReturn::ChangeToTrue:
					break;
					//角色从move转到非Move
				case EDoWhtileReturn::ChangeToFalse:
					break;
				}
				bDoOnce = bShouldMove;
				break;
			//case EStarve_MovementState::InAir:
			//	break;
			//case EStarve_MovementState::Ragdoll:
			//	break;
			//default:
			//	break;
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
		this->ActualGait = starve_cs.ActualGait;
		this->ActualStance = starve_cs.ActualStance;
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
	WalkRunBlend = CalculateWalkRunBlend();
	StrideBlend = CalculateStrideBlend();
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UStarveCharacterAnimInstance::UpdateRotationValues()
{

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
		dwr = bLastShouldMove == bShouldMove ? EDoWhtileReturn::WhileTrue : EDoWhtileReturn::ChangeToTrue;
	}
	else {
		dwr = bLastShouldMove == bShouldMove ? EDoWhtileReturn::WhileFalse : EDoWhtileReturn::ChangeToFalse;
	}
	return dwr;
}

FVelocityBlend UStarveCharacterAnimInstance::CalculateVelocityBlend()
{
	FVector norvelocity = Velocity.GetSafeNormal(0.1);
	FRotator characterrotation = CharacterRef->GetActorRotation();
	//不旋转向量，表示将人物的模型坐标用世界坐标表示出来，可以理解为将速度的处理放在自己模型的空间
	FVector l_rela_velo_dir = UKismetMathLibrary::LessLess_VectorRotator(norvelocity, characterrotation);
	
	//下面一步是为了判断当前速度方向在四向方向上的比例（因为是在地面上，所以忽略上下），方便后面六向混合
	float sum = FMath::Abs(l_rela_velo_dir.X) + FMath::Abs(l_rela_velo_dir.Y) + FMath::Abs(l_rela_velo_dir.Z);
	FVector rela_velo = l_rela_velo_dir / sum;

	FVelocityBlend vb;
	vb.F = FMath::Clamp(rela_velo.X, 0.f, 1.f);
	vb.B = FMath::Clamp(FMath::Abs(rela_velo.X), -1.f, 0.f);
	vb.L = FMath::Clamp(FMath::Abs(rela_velo.Y), -1.f, 0.f);
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
	switch (ActualGait)
	{
	case EStarve_Gait::Walking:
		return 0.f;
		break;
	default:
		return 1.f;
		break;
	}
}

float UStarveCharacterAnimInstance::CalculateStrideBlend()
{
	float stride_blend_nw = StrideBlend_N_Walk->GetFloatValue(Speed);
	float stride_blend_nr = StrideBlend_N_Run->GetFloatValue(Speed);
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.f, 0.f, 1.f);
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
	curvevalue = FMath::Clamp<float>(curvevalue + Bias, ClampMin, ClampMax);
	return curvevalue;
}

float UStarveCharacterAnimInstance::CalculateStandingPlayRate()
{	
	//计算当前速度与动画速度的比值，并进行差值
	float wp = Speed / AnimatedWalkSpeed;
	float rp = Speed / AnimatedRunSpeed;
	float alpha = GetAnimCurveClamp(FName("Weight_Gait"), -1.f, 0.f, 1.f);
	float w2r = UKismetMathLibrary::Lerp(wp, rp, alpha);

	float sp = Speed / AnimatedSprintSpeed;
	float alpha2 = GetAnimCurveClamp(FName("Weight_Gait"), -2.f, 0.f, 1.f);
	float w2r2s = UKismetMathLibrary::Lerp(w2r, sp, alpha2);
	
	//缩放步距大小，受人物本身步距大小和人物世界缩放影响
	float stridescale = (w2r2s / StrideBlend) / (GetOwningComponent()->K2_GetComponentScale().Z);

	return FMath::Clamp<float>(stridescale, 0.f, 3.f);

}

float UStarveCharacterAnimInstance::CalculateCrouchingPlayRate()
{
	float cp = Speed / AnimatedCrouchSpeed;

	//下蹲缩放步距大小，受人物本身步距大小和人物世界缩放影响
	float stridescale = (cp / StrideBlend) / (GetOwningComponent()->K2_GetComponentScale().Z);

	return FMath::Clamp<float>(stridescale, 0.f, 2.f);
}

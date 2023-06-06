// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CalculateRotationAmount_AM.h"
#include "AnimationModifier.h"

void UCalculateRotationAmount_AM::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	//判断曲线存不存在
	if (UAnimationBlueprintLibrary::DoesCurveExist(AnimationSequence, CurveName, ERawCurveTrackTypes::RCT_Float)) {
		UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, CurveName, false);
	}
	//添加曲线
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, CurveName, ERawCurveTrackTypes::RCT_Float, false);
	//获取动画的帧数
	int num = 0;
	UAnimationBlueprintLibrary::GetNumFrames(AnimationSequence, num);

	float ratescale = 0;
	UAnimationBlueprintLibrary::GetRateScale(AnimationSequence, ratescale);

	int selectint = ratescale < 0 ? -1 : 1;

	float time;
	FTransform nextpose;
	FTransform currentpose;
	float value;


	for (int i = 0; i <= num; i++) {
		//获取每一帧对应的时间
		UAnimationBlueprintLibrary::GetTimeAtFrame(AnimationSequence, i, time);

		//获取下一帧对应的pose
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, i + selectint, false, nextpose);
		//获取这一帧对应的pose
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimationSequence, RootBoneName, i, false, currentpose);

		//对应曲线的值
		value = (nextpose.Rotator().Yaw - currentpose.Rotator().Yaw) * FMath::Abs(selectint);

		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, CurveName, time, value);
	}
}

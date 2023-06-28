// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/OverlayOverride_ANS.h"
#include "Interfaces/Starve_AnimationInterface.h"

FString UOverlayOverride_ANS::GetNotifyName_Implementation() const
{
	FString num = FString::FromInt(OverlayOverrideState);
	return FString("OverlayOverride: ").Append(num);
}

void UOverlayOverride_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	IStarve_AnimationInterface* animinfa = Cast<IStarve_AnimationInterface>(MeshComp->GetAnimInstance());
	if (animinfa != nullptr) {
		animinfa->I_SetOverlayOverrideState(OverlayOverrideState);
	}
}

void UOverlayOverride_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IStarve_AnimationInterface* animinfa = Cast<IStarve_AnimationInterface>(MeshComp->GetAnimInstance());
	if (animinfa != nullptr) {
		animinfa->I_SetOverlayOverrideState(0);
	}
}

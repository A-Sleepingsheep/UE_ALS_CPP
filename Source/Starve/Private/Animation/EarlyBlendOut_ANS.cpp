// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/EarlyBlendOut_ANS.h"
#include "Animation/AnimInstance.h"


#include "Interfaces/Starve_CharacterInterface.h"


FString UEarlyBlendOut_ANS::GetNotifyName() const
{
	return FString("EarlyBlendOut");
}

void UEarlyBlendOut_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AnimInstance = MeshComp->GetAnimInstance();
	OwingActor = MeshComp->GetOwner();
}

void UEarlyBlendOut_ANS::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (AnimInstance != nullptr && OwingActor != nullptr) {
		IStarve_CharacterInterface* characterinterface = Cast<IStarve_CharacterInterface>(OwingActor);
		if (characterinterface != nullptr) {
			/*1.*/
			if (bCheckMovementState) {
				if (characterinterface->I_GetCurrentState().MovementState == MovementStateEquals) {
					AnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
				}
			}

			/*2.*/
			if (bCheckStance) {
				if (characterinterface->I_GetCurrentState().ActualStance == StanceEquals) {
					AnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
				}
			}

			/*3.*/
			if (bCheckMovementInput) {
				if (characterinterface->I_GetEssentialValues().bHasMovementInput) {
					AnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
				}
			}
		}
	}
}

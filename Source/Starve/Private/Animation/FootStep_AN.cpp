// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FootStep_AN.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"



void UFootStep_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UAnimInstance* anim = MeshComp->GetAnimInstance();
	if (MeshComp && anim) {
		if (IsValid(Sound)) {
			float curvevalue = 1.f - anim->GetCurveValue(FName("Mask_FootstepSound"));
			float volume = bOverrideMaskCurve ? VolumeMultiplier : VolumeMultiplier * curvevalue;
			UAudioComponent* audio = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, AttachPointName, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true, volume, PitchMultiplier);
			
			if (IsValid(audio)) {
				audio->SetIntParameter(FName("FootstepType"), (int32)FootStepType);
			}
		
		}


	}
}

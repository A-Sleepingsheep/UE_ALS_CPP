// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "FootStep_AN.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UFootStep_AN : public UAnimNotify
{
	GENERATED_BODY()


public:	
	UPROPERTY(EditAnywhere,Category = FootStep)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere,Category = FootStep)
	FName AttachPointName = FName("root");

	UPROPERTY(EditAnywhere,Category = FootStep)
	EFootstepType FootStepType = EFootstepType::Step;

	UPROPERTY(EditAnywhere,Category = FootStep)
	float VolumeMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere,Category = FootStep)
	float PitchMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere,Category = FootStep)
	bool bOverrideMaskCurve;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

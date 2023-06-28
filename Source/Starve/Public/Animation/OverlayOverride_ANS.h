// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "OverlayOverride_ANS.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UOverlayOverride_ANS : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	int OverlayOverrideState;

	FString GetNotifyName_Implementation() const;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;


	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};

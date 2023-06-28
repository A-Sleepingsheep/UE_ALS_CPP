// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CameraShake_AN.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UCameraShake_AN : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = CameraShake)
	TSubclassOf<class UCameraShakeBase> ShakeClass;

	UPROPERTY(EditAnywhere,Category = CameraShake)
	float Scale = 1.f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Enums/Starve_LocomotionEnum.h"

#include "GroundedEntryAction_AN.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UGroundedEntryAction_AN : public UAnimNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EGroundedEntryState GroundedEntryState;

	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

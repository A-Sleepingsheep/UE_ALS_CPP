// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "EarlyBlendOut_ANS.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UEarlyBlendOut_ANS : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimMontage* AnimMontage;

	/*混出时间*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float BlendOutTime = 0.25f;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCheckMovementState;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EStarve_MovementState MovementStateEquals;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCheckStance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EStarve_Stance StanceEquals;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bCheckMovementInput;

	class UAnimInstance* AnimInstance;

	AActor* OwingActor;

	FString GetNotifyName() const;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};

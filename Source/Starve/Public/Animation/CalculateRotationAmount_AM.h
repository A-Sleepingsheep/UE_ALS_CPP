// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "CalculateRotationAmount_AM.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UCalculateRotationAmount_AM : public UAnimationModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName CurveName = FName("RotationAmount");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FName RootBoneName = FName("root");


	//计算动画的旋转偏移量
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/StarveCharacterBase.h"
#include "StarveCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API AStarveCharacter : public AStarveCharacterBase
{
	GENERATED_BODY()
	
public:
	AStarveCharacter();

#pragma region CameraSystemInterface

	virtual FVector Get_FP_CameraTarget() override;

	/*��õ����˳�������Ļ���ê��λ��*/
	UFUNCTION(BlueprintCallable)
	virtual FTransform Get_TP_PivotTarget() override;

	/*��õ����˳����Ҽ�������λ��*/
	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) override;

#pragma endregion
};

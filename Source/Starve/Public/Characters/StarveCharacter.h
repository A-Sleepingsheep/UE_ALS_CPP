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

	/*攀爬点的实际开始偏移量*/
	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_2m_Default;

#pragma region CameraSystemInterface

	virtual FVector Get_FP_CameraTarget() override;

	/*获得第三人称下人物的基础锚点位置*/
	UFUNCTION(BlueprintCallable)
	virtual FTransform Get_TP_PivotTarget() override;

	/*获得第三人称左右肩的摄像机位置*/
	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) override;

	virtual FMantle_Asset GetMantleAsset(EMantleType MantleType) override;

	virtual void MantleStart(float& MantleHeight, FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType& RefMantleType) override;

	virtual void MantleEnd() override;
#pragma endregion
};

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

	/*Mantle_Assets*/
	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_2m_Default = FMantle_Asset(FVector(0.f,65.f,200.f),125.f,1.2f,0.6f,200.f,1.2f,0.f);

	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_1m_Default = FMantle_Asset(FVector(0.f, 65.f, 100.f), 50.f, 1.f, 0.5f, 100.f, 1.f, 0.f);

	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_1m_LH = FMantle_Asset(FVector(0.f, 65.f, 100.f), 50.f, 1.f, 0.5f, 100.f, 1.f, 0.f);;

	/*待修改*/
	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_1m_2H = FMantle_Asset(FVector(0.f, 65.f, 100.f), 50.f, 1.f, 0.5f, 100.f, 1.f, 0.f);;

	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_1m_RH = FMantle_Asset(FVector(0.f, 65.f, 100.f), 50.f, 1.f, 0.5f, 100.f, 1.f, 0.f);;
	
	UPROPERTY(Category = MantleSystem, EditAnywhere, BlueprintReadWrite)
		FMantle_Asset Mantle_1m_Box = FMantle_Asset(FVector(0.f, 65.f, 100.f), 50.f, 1.f, 0.5f, 100.f, 1.f, 0.f);;

	/*默认的翻滚蒙太奇*/
	UPROPERTY(Category = "Roll", EditAnywhere, BlueprintReadWrite)
		UAnimMontage* LandRollDefault;


#pragma region CameraSystemInterface

	virtual FVector Get_FP_CameraTarget() override;

	/*获得第三人称下人物的基础锚点位置*/
	UFUNCTION(BlueprintCallable)
	virtual FTransform Get_TP_PivotTarget() override;

	/*获得第三人称左右肩的摄像机位置*/
	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) override;

	virtual FMantle_Asset GetMantleAsset(EMantleType MantleType) override;

	virtual void MantleStart(float MantleHeight,const FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType RefMantleType) override;

	virtual void MantleEnd() override;

	virtual UAnimMontage* GetRollAnimation() override;
#pragma endregion
};

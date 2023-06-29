// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/Starve_ControllerInterface.h"

#include "StarvePC.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API AStarvePC : public APlayerController,public IStarve_ControllerInterface
{
	GENERATED_BODY()

public:
	AStarvePC();

	/*重写Onpossess函数*/
	virtual void OnPossess(APawn* aPawn) override;

	/**/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	ACharacter* DebugFocusCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	TArray<ACharacter*> AvailableDebugCharacters;

	/*是否进入Debug视角*/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool bDebugView;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowHUD;

	/*是否开启PlayerCameraManager的绘制*/
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool bShowTraces;


	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool bShowDebugShapes;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowLayerColors;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool Slomo;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Debug)
	bool ShowCharacterInfo;






public:
	/****************Controller Interfaces *********************/
	/*是否开启 PlayerCameraManager 的Character 与 Camera 之间球体检测绘制*/
	virtual bool I_ShowCameraManagerTraces() override;

	/*是否开启CameraManager的关于Pivot轴点位置相关的绘制*/
	virtual bool I_ShowDebugShapes() override;

	/*进入Debug视角*/
	virtual bool I_DebugView() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Starve_ControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStarve_ControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARVE_API IStarve_ControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*是否绘制Character跟Camera之间进行球体检测的线条显示*/
	virtual bool I_ShowCameraManagerTraces() = 0;

	/*是否绘制CameraManager的Debug Shapes*/
	virtual bool I_ShowDebugShapes() = 0;

	/*是否进入Debug视角*/
	virtual bool I_DebugView() = 0;
};

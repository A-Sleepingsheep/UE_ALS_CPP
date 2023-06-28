// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCameraInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 跟摄像机相关的接口
 * 在人物中实现
 */
class STARVE_API ICameraInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	* 获得摄像机的参数
	* @param TP_FOV 第三人称摄像机的FOV
	* @param FP_FOV 第一人称摄像机的FOV
	* @return 是否是右肩的位置，true表示右肩，false表示左肩
	*/
	virtual bool Get_CameraParameters(float& TP_FOV, float& FP_FOV) = 0 ;

	/* 获得第一人称视角的摄像机位置 */
	virtual FVector Get_FP_CameraTarget() = 0;

	/* 获得第三人称轴点的位置 */
	virtual FTransform Get_TP_PivotTarget() = 0;

	/* 
	* 主要是第三人称模式下进行摄像机与角色之间进行球体检测的信息
	* 传引用的目的是为了有多个返回值，返回的float 是球体检测的半径*/
	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) = 0;
};

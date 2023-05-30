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
 * ���������صĽӿ�
 * ��������ʵ��
 */
class STARVE_API ICameraInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	* ���������Ĳ���
	* @param TP_FOV �����˳��������FOV
	* @param FP_FOV ��һ�˳��������FOV
	* @return 
	*/
	virtual bool Get_CameraParameters(float& TP_FOV, float& FP_FOV) = 0 ;

	/* ��õ�һ�˳��ӽǵ������λ��*/
	virtual FVector Get_FP_CameraTarget() = 0;

	/*��õ����˳�������λ��*/
	virtual FTransform Get_TP_PivotTarget() = 0;


	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) = 0;
};

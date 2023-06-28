// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Starve_PlayerCameraManager.generated.h"

class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class STARVE_API AStarve_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AStarve_PlayerCameraManager();

	static FName CameraMeshName;

private:
	/*相机模型，这里主要是使用骨骼的曲线，进而达到使用曲线控制角色玩家的视角*/
	UPROPERTY(Category = CameraMesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CameraMesh;
	
	/*玩家控制的角色*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn;

	/*相机目标Rotation,每帧更新*/
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator TargetCameraRotation;

	/*平滑过渡到TargetPivot的中间过渡 Transform*/
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform SmoothedTargetPivot;

	/*PivotLocation（蓝色球体位置）：SmoothedTargetPivot + 世界坐标系下的偏移*/
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector PivotLocation;

	/*每帧更新的摄像机目标位置*/
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TargetCameraLocation;

public:
	/*获得相机模型的Mesh*/
	FORCEINLINE USkeletalMeshComponent* GetCameraMesh() const { return CameraMesh; }

	/**
	* 当PlayerController调用 OnPossess 时，会调用该函数对ControlledPawn进行赋值
	* 同时对摄像机模型的动画蓝图进行PlayerController和Pawn的赋值
	**/
	virtual void OnPlayerControllerPossess(APawn* Pawn);


protected:
	/*每帧调用，更新ViewTarget*/
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	/*自定义更新ViewTarget，在UpdateViewTargetInternal中调用*/
	virtual void CustomCameraBehavior(float DeltaTime, FMinimalViewInfo& POV);

	/*获得AnimInstance的曲线信息*/
	float GetCameraBehaviorCurveValue(FName CurveName);

	/*在本地坐标系下进行Location的过渡差值计算,这样计算就与轴无关，这样可以最大程度的控制差值的过渡*/
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds);

public:
	/*********************************************************************/
	//Debug模式
	/*Debug模式下固定视角的Rotation*/
	UPROPERTY(Category = Starve_Debug, EditAnywhere, BlueprintReadWrite)
	FRotator DebugViewRotation;

	/*Debug模式下固定视角的Location Offset*/
	UPROPERTY(Category = Starve_Debug, EditAnywhere, BlueprintReadWrite)
	FVector DebugViewOffset;

	/*Debug模式下Character与Camera之间进行球体检测时的DrawDebugType*/
	EDrawDebugTrace::Type GetDebugTraceType(EDrawDebugTrace::Type DrawDebugTrace);

	/************************************************************************************/


};
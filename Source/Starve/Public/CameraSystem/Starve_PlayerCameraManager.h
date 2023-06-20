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

private:
	UPROPERTY(Category = CameraMesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CameraBehavior;
	
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn;

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator TargetCameraRotation;

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator DebugViewRotation;/*Debug模式固定视角的旋转*/

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector DebugViewOffset;/*Debug模式固定视角的Offset*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform SmoothedTargetPivot;/*平缓目标轴点*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector PivotLocation;/*轴点位置*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TargetCameraLocation;/*摄像机目标位置*/


public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return CameraBehavior; }

	virtual void OnPossess(APawn* Pawn);

	virtual void UpdateCamera(float DeltaTime) override;

	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	void CustomCameraBehavior(float DeltaTime, FMinimalViewInfo& OutResult);

	EDrawDebugTrace::Type GetDebugTraceType(EDrawDebugTrace::Type DrawDebugTrace);

	/*获得AnimInstance的曲线信息*/
	float GetCameraBehaviorParam(FName CurveName);

	/*分别精确计算位置X,Y，Z的差值过渡值*/
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds);
};
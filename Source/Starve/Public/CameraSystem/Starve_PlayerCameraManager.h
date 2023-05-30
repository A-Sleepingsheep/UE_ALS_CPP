// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
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
	FRotator DebugViewRotation;/*Debugģʽ�̶��ӽǵ���ת*/

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector DebugViewOffset;/*Debugģʽ�̶��ӽǵ�Offset*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform SmoothedTargetPivot;/*ƽ��Ŀ�����*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector PivotLocation;/*���λ��*/

	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TargetCameraLocation;/*�����Ŀ��λ��*/


public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return CameraBehavior; }

	virtual void OnPossess(APawn* Pawn);

	virtual void UpdateCamera(float DeltaTime) override;

	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	void CustomCameraBehavior(float DeltaTime, FMinimalViewInfo& OutResult);


	/*���AnimInstance��������Ϣ*/
	float GetCameraBehaviorParam(FName CurveName);

	/*�ֱ�ȷ����λ��X,Y��Z�Ĳ�ֵ����ֵ*/
	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds);

};
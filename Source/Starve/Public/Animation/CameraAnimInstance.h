// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Enums/Starve_LocomotionEnum.h"

#include "CameraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STARVE_API UCameraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	UCameraAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Category = CameraRef, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	UPROPERTY(Category = CameraRef, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APawn* ControlledPawn;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Gait Gait;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementState MovementState;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_RotationMode RotationMode;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementAction MovementAction;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_ViewMode ViewMode;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Stance Stance;

	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bRightShould;
	
	UPROPERTY(Category = CharacterInfo, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebugView;


public:
	/*Update，更新角色相关的信息*/
	void UpdateCharacterInfo();

	FORCEINLINE APlayerController* GetPlayerController() const { return this->PlayerController; }
	FORCEINLINE APawn* GetControlledPawn() const { return this->ControlledPawn; }

	FORCEINLINE void SetPlayerController(APlayerController* PC) { this->PlayerController = PC; };
	FORCEINLINE void SetControlledPawn(APawn* Pawn) { this->ControlledPawn = Pawn; };
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CameraAnimInstance.h"

#include "Interfaces/Starve_CharacterInterface.h"
#include "Interfaces/CameraInterface.h"
#include "Gameplay/StarvePC.h"

UCameraAnimInstance::UCameraAnimInstance() {
	
}

void UCameraAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
}

void UCameraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateCharacterInfo();
}

void UCameraAnimInstance::UpdateCharacterInfo()
{
	IStarve_CharacterInterface* ci = Cast<IStarve_CharacterInterface>(ControlledPawn);
	if (ci != nullptr) {
		FStarveCharacterState info = ci->I_GetCurrentState();
		MovementState = info.MovementState;
		MovementAction = info.MovementAction;
		RotationMode = info.RotationMode;
		Gait = info.ActualGait;
		Stance = info.ActualStance;
		ViewMode = info.ViewMode;
	}

	ICameraInterface* cami = Cast<ICameraInterface>(ControlledPawn);
	if (cami != nullptr) {
		float f1, f2;
		bRightShould = cami->Get_CameraParameters(f1, f2);
	}

	AStarvePC* coni = Cast<AStarvePC>(PlayerController);
	if (coni != nullptr) {
		bDebugView = coni->DebugView;
	}
}

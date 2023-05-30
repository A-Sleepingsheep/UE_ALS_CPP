// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/StarveCharacterAnimInstance.h"
#include "GameFramework/Character.h"

#include "Interfaces/Starve_CharacterInterface.h"
#include "Structs/Starve_LocomotionStructs.h"

UStarveCharacterAnimInstance::UStarveCharacterAnimInstance() {

}

void UStarveCharacterAnimInstance::NativeInitializeAnimation()
{
	APawn* pawn = TryGetPawnOwner();
	if (IsValid(pawn)) {
		this->CharacterRef = Cast<ACharacter>(pawn);
	}
}

void UStarveCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	this->DeltaTime = DeltaSeconds;
	if (DeltaTime != 0.f) {

	}
}


void UStarveCharacterAnimInstance::UpdateCharacterInfo()
{
	IStarve_CharacterInterface* sci = Cast<IStarve_CharacterInterface>(CharacterRef);
	if (sci != nullptr) {
		FEssentialValues e_v = sci->I_GetEssentialValues();
		this->Velocity = e_v.Velocity;
		this->Acceleration = e_v.Acceleration;
		this->MovementInput = e_v.MovementInput;
		this->bIsMoving = e_v.bIsMoving;
		this->bHasMovementInput = e_v.bHasMovementInput;
		this->Speed = e_v.Speed;
		this->MovementInputAmount = e_v.MovementInputAmount;
		this->AimingRatation = e_v.AimingRatation;
		this->AimYawRate = e_v.AimYawRate;

		FStarveCharacterState starve_cs =sci->I_GetCurrentState();
		this->MovementState = starve_cs.MovementState;
		this->PrevMovementState = starve_cs.PrevMovementState;
		this->MovementAction = starve_cs.MovementAction;
		this->RotationMode = starve_cs.RotationMode;
		this->ActualGait = starve_cs.ActualGait;
		this->ActualStance = starve_cs.ActualStance;
		this->ViewMode = starve_cs.ViewMode;
		this->OverlayState = starve_cs.OverlayState;
	}
}

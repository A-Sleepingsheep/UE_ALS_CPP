// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CameraShake_AN.h"
#include "Camera/CameraShake.h"


void UCameraShake_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* actor = MeshComp->GetOwner();
	if (IsValid(actor)) {
		APawn* pawn = Cast<APawn>(actor);
		if (IsValid(pawn)) {
			APlayerController* controller = Cast<APlayerController>(pawn->GetController());
			if (IsValid(controller)) {
				controller->ClientStartCameraShake(ShakeClass, Scale);
			}
		}
	}
}

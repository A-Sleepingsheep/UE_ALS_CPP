// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/GroundedEntryAction_AN.h"
#include "Interfaces/Starve_AnimationInterface.h"

FString UGroundedEntryAction_AN::GetNotifyName_Implementation() const
{
	FString EnumName = StaticEnum<EGroundedEntryState>()->GetNameStringByValue(static_cast<int64>(GroundedEntryState));
	return FString("GroundedEntryState:").Append(EnumName);
}

void UGroundedEntryAction_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IStarve_AnimationInterface* animinfa = Cast<IStarve_AnimationInterface>(MeshComp->GetAnimInstance());
	if (animinfa != nullptr) {
		animinfa->I_SetGroundedEntryState(GroundedEntryState);
	}
}

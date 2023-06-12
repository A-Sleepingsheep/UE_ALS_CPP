// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MovementAction_ANS.h"
#include "Kismet/KismetStringLibrary.h"

#include "Interfaces/Starve_CharacterInterface.h"


FString UMovementAction_ANS::GetNotifyName_Implementation() const
{
	FString PreName = "MovementAction:";

	FString EnumName = StaticEnum<EStarve_MovementAction>()->GetNameStringByValue(static_cast<int64>(MovementAction));

	return UKismetStringLibrary::Concat_StrStr(PreName, EnumName);
}

void UMovementAction_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	IStarve_CharacterInterface* characterinterface = Cast<IStarve_CharacterInterface>(MeshComp->GetOwner());
	if (characterinterface != nullptr) {
		characterinterface->I_SetMovementAction(MovementAction);
	}
}

void UMovementAction_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IStarve_CharacterInterface* characterinterface = Cast<IStarve_CharacterInterface>(MeshComp->GetOwner());
	if (characterinterface != nullptr) {
		FStarveCharacterState characterstates = characterinterface->I_GetCurrentState();
		if (characterstates.MovementAction == MovementAction) {
			characterinterface->I_SetMovementAction(EStarve_MovementAction::None);
		}
	}
}

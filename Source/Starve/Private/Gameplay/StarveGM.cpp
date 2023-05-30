// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarveGM.h"

AStarveGM::AStarveGM() {
	static ConstructorHelpers::FClassFinder<APlayerController> PCClass(TEXT("Class'/Script/Starve.StarvePC'"));
	if (PCClass.Class != NULL) {
		this->PlayerControllerClass = PCClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> CharacterClass(TEXT("Class'/Script/Starve.StarveCharacter'"));
	if (CharacterClass.Class != NULL) {
		this->DefaultPawnClass = CharacterClass.Class;
	}

}

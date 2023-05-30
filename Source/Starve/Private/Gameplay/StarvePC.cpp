// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarvePC.h"

#include "CameraSystem/Starve_PlayerCameraManager.h"

AStarvePC::AStarvePC() {
	static ConstructorHelpers::FClassFinder<APlayerCameraManager> PCMFinder(TEXT("Class'/Script/Starve.Starve_PlayerCameraManager'"));
	if (PCMFinder.Class != NULL) {
		this->PlayerCameraManagerClass = PCMFinder.Class;
	}


}


void AStarvePC::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	AStarve_PlayerCameraManager* SPCM = Cast<AStarve_PlayerCameraManager>(PlayerCameraManager);
	if (SPCM != NULL) {
		SPCM->OnPossess(aPawn);
	}
}

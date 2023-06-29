// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarvePC.h"
//#include "Kismet/GameplayStatics.h"

#include "CameraSystem/Starve_PlayerCameraManager.h"
#include "Interfaces/Starve_InputInterface.h"

AStarvePC::AStarvePC() {
	this->PlayerCameraManagerClass = AStarve_PlayerCameraManager::StaticClass();
}


void AStarvePC::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	AStarve_PlayerCameraManager* temp = Cast<AStarve_PlayerCameraManager>(PlayerCameraManager);
	if (temp != nullptr) {
		temp->OnPlayerControllerPossess(aPawn);
	}
}

bool AStarvePC::I_ShowDebugShapes()
{
	return bShowDebugShapes;
}

bool AStarvePC::I_DebugView()
{
	return bDebugView;
}

bool AStarvePC::I_ShowCameraManagerTraces()
{
	return bShowTraces;
}

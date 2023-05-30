// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/StarveCharacter.h"

AStarveCharacter::AStarveCharacter() {
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeleMesh(TEXT("SkeletalMesh'/Game/MyALS_CPP/CharacterAssets/Mesh/StarveMan.StarveMan'"));
	if (SkeleMesh.Succeeded()) {
		this->GetMesh()->SetSkeletalMesh(SkeleMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharAniInsFinder(TEXT("AnimBlueprint'/Game/MyALS_CPP/CharacterAssets/StarveCharacter_ABP.StarveCharacter_ABP_C'"));
	if (CharAniInsFinder.Class != NULL) {
		this->GetMesh()->AnimClass = CharAniInsFinder.Class;
	}
}

#pragma region CameraInterfaceDefinition

FVector AStarveCharacter::Get_FP_CameraTarget()
{
	return GetMesh()->GetSocketLocation(TEXT("FP_Camera"));
}

FTransform AStarveCharacter::Get_TP_PivotTarget()
{
	FVector headLocation = GetMesh()->GetSocketLocation(TEXT("head"));
	FVector rootLocation = GetMesh()->GetSocketLocation(TEXT("root"));
	FVector pivotLocation = (headLocation + rootLocation) / 2;
	FRotator actorRotation = GetActorRotation();
	return FTransform(actorRotation, pivotLocation);
}

float AStarveCharacter::Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel)
{
	if (bRightShoulder) {
		TraceOrigin = GetMesh()->GetSocketLocation(TEXT("TP_CameraTrace_R"));
	}
	else {
		TraceOrigin = GetMesh()->GetSocketLocation(TEXT("TP_CameraTrace_L"));
	}
	TraceChannel = ETraceTypeQuery::TraceTypeQuery2;
	return 15.0f;
}

#pragma endregion
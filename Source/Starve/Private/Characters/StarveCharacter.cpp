// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/StarveCharacter.h"
#include "Curves/CurveVector.h"


AStarveCharacter::AStarveCharacter() {
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeleMesh(TEXT("SkeletalMesh'/Game/MyALS_CPP/CharacterAssets/Mesh/StarveMan.StarveMan'"));
	if (SkeleMesh.Succeeded()) {
		this->GetMesh()->SetSkeletalMesh(SkeleMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharAniInsFinder(TEXT("AnimBlueprint'/Game/MyALS_CPP/CharacterAssets/StarveCharacter_ABP.StarveCharacter_ABP_C'"));
	if (CharAniInsFinder.Class != NULL) {
		this->GetMesh()->AnimClass = CharAniInsFinder.Class;
	}

	//攀爬资源初始赋值，先在C++中这么使用，后面进行改进
	//1m攀爬曲线
	static ConstructorHelpers::FObjectFinder<UCurveVector> cvf1(TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/MantleCurve/Mantle_1m.Mantle_1m'"));
	if (cvf1.Succeeded()) {
		Mantle_1m_Default.PositionCorrectionCurve = cvf1.Object;

	}
	//2m攀爬曲线
	static ConstructorHelpers::FObjectFinder<UCurveVector> cvf2(TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/MantleCurve/Mantle_2m.Mantle_2m'"));
	if (cvf2.Succeeded()) {
		Mantle_2m_Default.PositionCorrectionCurve = cvf2.Object;
	}

	//mantle_2m_default
	static ConstructorHelpers::FObjectFinder<UAnimMontage> am_mantle_2m_default(TEXT("AnimMontage'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Actions/Starve_N_Mantle_2m_Montage_Default.Starve_N_Mantle_2m_Montage_Default'"));
	if (am_mantle_2m_default.Succeeded()) {
		Mantle_2m_Default.AnimMontage = am_mantle_2m_default.Object;
	}

	//mantle_2m_default
	static ConstructorHelpers::FObjectFinder<UAnimMontage> am_mantle_1m_default(TEXT("AnimMontage'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Actions/Starve_N_Mantle_1m_Montage_Default.Starve_N_Mantle_1m_Montage_Default'"));
	if (am_mantle_1m_default.Succeeded()) {
		Mantle_1m_Default.AnimMontage = am_mantle_1m_default.Object;
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


FMantle_Asset AStarveCharacter::GetMantleAsset(EMantleType mantleType)
{
	if (mantleType == EMantleType::HighMantle || mantleType == EMantleType::FallingCatch) {
		return Mantle_2m_Default;
	}
	else if(mantleType==EMantleType::LowMantle) {
		return Mantle_1m_Default;

		/*switch (OverlayState)
		{
			case EStarve_OverlayState::Default:
			case EStarve_OverlayState::Masculine:
			case EStarve_OverlayState::Feminine: {
				return 
			}
			case EStarve_OverlayState::Injured: {
				return 
			}

			case EStarve_OverlayState::HandsTied: {
				return 
			}
			case EStarve_OverlayState::Rifle:
			case EStarve_OverlayState::Pistol_1H:
			case EStarve_OverlayState::Postol_2H: {
				return 
			}
			case EStarve_OverlayState::Bow: {

				break;
			}
			case EStarve_OverlayState::Touch: {

				break;
			}
			case EStarve_OverlayState::Binoculars: {

				break;
			}
			case EStarve_OverlayState::Box: {

				break;
			}
			case EStarve_OverlayState::Barrel: {

				break;
			}
			default:
				break;
		}*/

	}

	return Mantle_2m_Default;

}


void AStarveCharacter::MantleStart(float MantleHeight,const FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType RefMantleType)
{
	Super::MantleStart(MantleHeight, MantleLedgeWS, RefMantleType);

}

void AStarveCharacter::MantleEnd()
{
	Super::MantleEnd();
}

#pragma endregion
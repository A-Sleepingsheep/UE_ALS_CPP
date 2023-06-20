// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/StarveCharacter.h"
#include "Curves/CurveVector.h"


AStarveCharacter::AStarveCharacter() {
	HeldObjectRoot = CreateDefaultSubobject<USceneComponent>(FName("HeldObjectRoot"));
	HeldObjectRoot->SetupAttachment(GetMesh());

	VisualMeshes = CreateDefaultSubobject<USceneComponent>(FName("VisualMeshes"));
	VisualMeshes->SetupAttachment(GetMesh());

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	StaticMesh->SetupAttachment(HeldObjectRoot);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(HeldObjectRoot);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("BodyMesh"));
	BodyMesh->SetupAttachment(VisualMeshes);
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> bodyanimfinder(TEXT("AnimBlueprint'/Game/AdvancedLocomotionV4/Props/Meshes/Bow_AnimBP.Bow_AnimBP_C'"));
	if (bodyanimfinder.Succeeded()) {
		SkeletalMesh->SetAnimInstanceClass(bodyanimfinder.Class);
		BodyMesh->SetAnimInstanceClass(bodyanimfinder.Class);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeleMeshFinder(TEXT("SkeletalMesh'/Game/MyALS_CPP/CharacterAssets/Mesh/StarveMan.StarveMan'"));
	if (SkeleMeshFinder.Succeeded()) {
		this->GetMesh()->SetSkeletalMesh(SkeleMeshFinder.Object);
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

	//LandRollDefault
	static ConstructorHelpers::FObjectFinder<UAnimMontage> landrolldefault(TEXT("AnimMontage'/Game/MyALS_CPP/CharacterAssets/AnimationExamples/Actions/Starve_N_LandRoll_F_Montage_Default.Starve_N_LandRoll_F_Montage_Default'"));
	if (landrolldefault.Succeeded()) {
		LandRollDefault = landrolldefault.Object;
	}
}

void AStarveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHeldObjectAnimations();
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
		switch (OverlayState)
		{
			case EStarve_OverlayState::Default:
			case EStarve_OverlayState::Masculine:
			case EStarve_OverlayState::Feminine: {
				return Mantle_1m_Default;
			}
			case EStarve_OverlayState::Injured: {
				return Mantle_1m_LH;
			}

			case EStarve_OverlayState::HandsTied: {
				return Mantle_1m_2H;
			}
			case EStarve_OverlayState::Rifle:
			case EStarve_OverlayState::Pistol_1H:
			case EStarve_OverlayState::Postol_2H: {
				return Mantle_1m_RH;
			}
			case EStarve_OverlayState::Bow: {
				return Mantle_1m_LH;
			}
			case EStarve_OverlayState::Touch: {
				return Mantle_1m_LH;
			}
			case EStarve_OverlayState::Binoculars: {
				return Mantle_1m_RH;
			}
			case EStarve_OverlayState::Box: {
				return Mantle_1m_Box;
			}
			case EStarve_OverlayState::Barrel: {
				return Mantle_1m_LH;
			}
		}
	}

	return Mantle_2m_Default;

}


void AStarveCharacter::MantleStart(float MantleHeight,const FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType RefMantleType)
{
	Super::MantleStart(MantleHeight, MantleLedgeWS, RefMantleType);
	switch (MantleType)
	{
		case EMantleType::HighMantle:
		case EMantleType::FallingCatch:
			ClearHeldObject();
			break;
	}
}

void AStarveCharacter::MantleEnd()
{
	Super::MantleEnd();
	UpdateHeldObject();
}

UAnimMontage* AStarveCharacter::GetRollAnimation()
{
	switch (OverlayState)
	{
		case EStarve_OverlayState::Default: {
			return LandRollDefault;
		}
	}
	return LandRollDefault;
}

void AStarveCharacter::UpdateHeldObject()
{
	switch (OverlayState)
	{
		case EStarve_OverlayState::Default:
		case EStarve_OverlayState::Masculine:
		case EStarve_OverlayState::Feminine:
		case EStarve_OverlayState::Injured:
		case EStarve_OverlayState::HandsTied: {
			ClearHeldObject();
			break;
		}
		case EStarve_OverlayState::Rifle: {
			USkeletalMesh* skeletalmesh = LoadObject<USkeletalMesh>(this, TEXT("SkeletalMesh'/Game/AdvancedLocomotionV4/Props/Meshes/M4A1.M4A1'"));
			AttachToHand(nullptr,skeletalmesh,nullptr);
			break;
		}
		case EStarve_OverlayState::Pistol_1H: {
			USkeletalMesh* skeletalmesh = LoadObject<USkeletalMesh>(this, TEXT("SkeletalMesh'/Game/AdvancedLocomotionV4/Props/Meshes/M9.M9'"));
			AttachToHand(nullptr, skeletalmesh, nullptr);
			break;
		}
		case EStarve_OverlayState::Postol_2H: {
			USkeletalMesh* skeletalmesh = LoadObject<USkeletalMesh>(this, TEXT("SkeletalMesh'/Game/AdvancedLocomotionV4/Props/Meshes/M9.M9'"));
			AttachToHand(nullptr, skeletalmesh, nullptr);
			break;
		}
		case EStarve_OverlayState::Bow: {
			USkeletalMesh* skeletalmesh = LoadObject<USkeletalMesh>(this, TEXT("SkeletalMesh'/Game/AdvancedLocomotionV4/Props/Meshes/Bow.Bow'"));
			UObject* animinstance = LoadObject<UObject>(this, TEXT("AnimBlueprint'/Game/AdvancedLocomotionV4/Props/Meshes/Bow_AnimBP.Bow_AnimBP_C'"));
			AttachToHand(nullptr, skeletalmesh, animinstance,true);
			break;
		}
		case EStarve_OverlayState::Touch:{
			UStaticMesh* staticmesh = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/AdvancedLocomotionV4/Props/Meshes/Torch.Torch'"));
			AttachToHand(staticmesh, nullptr, nullptr,true);
			break;
		}
		case EStarve_OverlayState::Binoculars: {
			UStaticMesh* staticmesh = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/AdvancedLocomotionV4/Props/Meshes/Binoculars.Binoculars'"));
			AttachToHand(staticmesh, nullptr, nullptr);
			break;
		}
		case EStarve_OverlayState::Box: {
			UStaticMesh* staticmesh = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/AdvancedLocomotionV4/Props/Meshes/Box.Box'"));
			AttachToHand(staticmesh, nullptr, nullptr);
			break;
		}
		case EStarve_OverlayState::Barrel: {
			UStaticMesh* staticmesh = LoadObject<UStaticMesh>(this, TEXT("StaticMesh'/Game/AdvancedLocomotionV4/Props/Meshes/Barrel.Barrel'"));
			AttachToHand(staticmesh, nullptr, nullptr, true);
			break;
		}
	}
}

void AStarveCharacter::ClearHeldObject()
{
	StaticMesh->SetStaticMesh(nullptr);
	SkeletalMesh->SetSkeletalMesh(nullptr);
	SkeletalMesh->SetAnimInstanceClass(nullptr);
}

void AStarveCharacter::AttachToHand(UStaticMesh* NewStaticMesh, USkeletalMesh* NewSkeletalMesh, UObject* NewAnimClass, bool bLeftHand, FVector Offset)
{
	/*1.先清除手中的物品*/
	ClearHeldObject();

	/*2.设置静态网格体*/
	if (IsValid(NewStaticMesh)) {
		StaticMesh->SetStaticMesh(NewStaticMesh);
	}

	/*设置骨骼网格体以及对应的动画*/
	if (IsValid(NewSkeletalMesh)) {
		SkeletalMesh->SetSkeletalMesh(NewSkeletalMesh);
		if (IsValid(NewAnimClass)) {
			SkeletalMesh->SetAnimInstanceClass(NewAnimClass->GetClass());
		}
	}

	/*添加物品组件*/
	FName socketname = bLeftHand ? FName("VB LHS_ik_hand_gun") : FName("VB RHS_ik_hand_gun");
	HeldObjectRoot->K2_AttachToComponent(GetMesh(), socketname, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	HeldObjectRoot->SetRelativeLocation(Offset);
}

void AStarveCharacter::OnOverlayStateChanged(EStarve_OverlayState NewOverlayState)
{
	Super::OnOverlayStateChanged(NewOverlayState);
	UpdateHeldObject();
}

#pragma endregion
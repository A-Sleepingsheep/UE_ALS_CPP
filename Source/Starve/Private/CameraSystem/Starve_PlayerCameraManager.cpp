// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraSystem/Starve_PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Animation/CameraAnimInstance.h"
#include "Interfaces/CameraInterface.h"


AStarve_PlayerCameraManager::AStarve_PlayerCameraManager() {

	//�����������������Mesh
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CameraBehavior"));
	CameraBehavior->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CameraMesh(TEXT("SkeletalMesh'/Game/MyALS_CPP/Blueprints/CameraSystem/Starve_Camera.Starve_Camera'"));
	if (CameraMesh.Object != NULL) {
		CameraBehavior->SetSkeletalMesh(CameraMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceFinder(TEXT("AnimBlueprint'/Game/MyALS_CPP/Blueprints/CameraSystem/Starve_Camera_ABP.Starve_Camera_ABP_C'"));
	if (AnimInstanceFinder.Succeeded()) {
		this->GetMesh()->AnimClass = AnimInstanceFinder.Class;
	}

	DebugViewRotation = FRotator(0.f, -5.f, 180.f);
	DebugViewOffset = FVector(350.f, 0.f, 50.f);
}

void AStarve_PlayerCameraManager::OnPossess(APawn* Pawn)
{
	this->ControlledPawn = Pawn;

	//UE_LOG(LogTemp, Warning, TEXT("On AStarve_PlayerCameraManager Possess"));

	UCameraAnimInstance* CameraAnimInstance = Cast<UCameraAnimInstance>(this->GetMesh()->GetAnimInstance());
	if (CameraAnimInstance != nullptr) {

		//UE_LOG(LogTemp, Warning, TEXT("On CameraAnimInstance Assign"));

		CameraAnimInstance->SetPlayerController(GetOwningPlayerController());
		CameraAnimInstance->SetControlledPawn(ControlledPawn);
	}
}

void AStarve_PlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);


}

void AStarve_PlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	if (OutVT.Target->ActorHasTag("Starve_Character")) {
		//UKismetSystemLibrary::PrintString(this, "CalcCamera", true, false, FLinearColor::Blue, 0.f);

		CustomCameraBehavior(DeltaTime, OutVT.POV);
	}
	else {
		//UKismetSystemLibrary::PrintString(this, "CalcCamera2", true, false, FLinearColor::Blue, 2.f);
		Super::CalcCamera(DeltaTime, OutVT.POV);
	}
}

void AStarve_PlayerCameraManager::CustomCameraBehavior(float DeltaTime, FMinimalViewInfo& OutResult)
{
	
	ICameraInterface* CameraInterface = Cast<ICameraInterface>(ControlledPawn);
	if (CameraInterface != nullptr) {
		/*��һ��*/
		//���ͨ��CameraInterface�����Ϣ
		FTransform Pivot_Target;
		FVector FP_Target;
		float FP_FOV, TP_FOV = 0.f;
		Pivot_Target = CameraInterface->Get_TP_PivotTarget();
		FP_Target = CameraInterface->Get_FP_CameraTarget();
		CameraInterface->Get_CameraParameters(TP_FOV, FP_FOV);


		/*�ڶ���*/
		//���������Rotation��PlayerController��Rotation����ͬ�������ò�ֵӪ����ӳٵĸо�
		FRotator PCRotation = GetOwningPlayerController()->GetControlRotation();
		FRotator CameraRotation = GetCameraRotation();
		float Alpha = GetCameraBehaviorParam(FName("RotationLagSpeed"));
		FRotator InterpRotation = FMath::RInterpTo(CameraRotation, PCRotation, DeltaTime, Alpha);
		//Debug
		TargetCameraRotation = UKismetMathLibrary::RLerp(InterpRotation, DebugViewRotation, GetCameraBehaviorParam(FName("Override_Debug")), true);

		/*������*/
		//��ô�������ӽǵ������������(Pivot_Target)��ÿ֡����ֵ
		FVector LagVector;
		LagVector.X = GetCameraBehaviorParam(FName("PivotLagSpeed_X"));
		LagVector.Y = GetCameraBehaviorParam(FName("PivotLagSpeed_Y"));
		LagVector.Z = GetCameraBehaviorParam(FName("PivotLagSpeed_Z"));
		FVector CalculVector = CalculateAxisIndependentLag(SmoothedTargetPivot.GetLocation(), Pivot_Target.GetLocation(), TargetCameraRotation, LagVector);
		SmoothedTargetPivot = FTransform(Pivot_Target.GetRotation(), CalculVector);

		/*���Ĳ�*/
		//���������ת�������Ӹ�SmoothSmoothedTargetPivot��Location
		FVector PivotAddLocationX = SmoothedTargetPivot.GetRotation().GetForwardVector() * GetCameraBehaviorParam(FName("PivotOffset_X"));
		FVector PivotAddLocationY = SmoothedTargetPivot.GetRotation().GetRightVector() * GetCameraBehaviorParam(FName("PivotOffset_Y"));
		FVector PivotAddLocationZ = SmoothedTargetPivot.GetRotation().GetUpVector() * GetCameraBehaviorParam(FName("PivotOffset_Z"));

		PivotLocation = PivotAddLocationX + PivotAddLocationY + PivotAddLocationZ + SmoothedTargetPivot.GetLocation();

		/*���岽*/
		//������һ����õ�λ�ã����������������λ��������������������λ��
		FVector CameraAddLocationX = TargetCameraRotation.Vector() * GetCameraBehaviorParam(FName("CameraOffset_X"));
		FVector CameraAddLocationY = FRotationMatrix(TargetCameraRotation).GetScaledAxis(EAxis::Y) * GetCameraBehaviorParam(FName("CameraOffset_Y"));
		FVector CameraAddLocationZ = FRotationMatrix(TargetCameraRotation).GetScaledAxis(EAxis::Z) * GetCameraBehaviorParam(FName("CameraOffset_Z"));

		FVector CameraAfterLocation = CameraAddLocationX + CameraAddLocationY + CameraAddLocationZ + PivotLocation;
		//Debug
		FVector DebugLocation = Pivot_Target.GetLocation() + DebugViewOffset;
		float DebugAlpha = GetCameraBehaviorParam(FName("Override_Debug"));

		TargetCameraLocation = UKismetMathLibrary::VLerp(CameraAfterLocation, DebugLocation, DebugAlpha);

		/*������*/
		//����������,��������·���Ƿ����ڵ�
		FVector TraceOrigin;
		ETraceTypeQuery TraceChannel;
		float Radius = CameraInterface->Get_TP_TraceParams(TraceOrigin, TraceChannel);
		FHitResult OutHit;
		if (UKismetSystemLibrary::SphereTraceSingle(this, TraceOrigin, TargetCameraLocation, 
													Radius, TraceChannel,
													false, {},
													EDrawDebugTrace::ForOneFrame, OutHit, true,
													FLinearColor::Yellow,FLinearColor::Black,5.0f)) 
		{
			//��ײ�� - ������ �������������ǰŲ���٣��Ǹ�������TraceEndһ����˵��������� End
			if (OutHit.bBlockingHit && !OutHit.bStartPenetrating) {
				FVector SubVector = OutHit.Location - OutHit.TraceEnd;	
				TargetCameraLocation += SubVector;
			}
		}

		//UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),

		/*���߲�*/
		//����Debug����
		//UKismetSystemLibrary::PrintString(this, "DrawDebug", true, false, FLinearColor::Blue, 0.f);
		//�����ﴦ��õ�Pivot_Target
		UKismetSystemLibrary::DrawDebugSphere(this, Pivot_Target.GetLocation(), 16.f, 8,FLinearColor::Green,0.f,0.5f);
		//ƽ������ʱ��SmoothedTargetPivot
		UKismetSystemLibrary::DrawDebugSphere(this, SmoothedTargetPivot.GetLocation(), 16.f, 8, FLinearColor(1.f,0.166667f,0.f,1.f),0.f, 0.5f);
		//ƽ������ʱ���յ�PivotLocationλ��
		UKismetSystemLibrary::DrawDebugSphere(this, PivotLocation, 16.f, 8, FLinearColor::Blue, 0.f, 0.5f);
		//����SmoothedTargetPivot��Pivot_Target֮�������
		UKismetSystemLibrary::DrawDebugLine(this, SmoothedTargetPivot.GetLocation(), Pivot_Target.GetLocation(), FLinearColor(1, 0.166667, 0, 1), 0.f, 1.f);
		//����
		UKismetSystemLibrary::DrawDebugLine(this, PivotLocation, SmoothedTargetPivot.GetLocation(), FLinearColor::Blue, 0.f, 1.f);

		/*�ڰ˲�*/
		//�������ֵ
		//��õ����˳����һ�˳Ƶı仯
		FTransform TP_CameraTransform(TargetCameraRotation, TargetCameraLocation);
		FTransform FP_CameraTransform(TargetCameraRotation, FP_Target);
		FTransform T_F_Transform = UKismetMathLibrary::TLerp(TP_CameraTransform, FP_CameraTransform, GetCameraBehaviorParam(FName("Weight_FirstPerson")));

		//Debug
		FTransform Debug_CameraTransform(DebugViewRotation, TargetCameraLocation);
		FTransform Debug_Transform = UKismetMathLibrary::TLerp(T_F_Transform, Debug_CameraTransform, GetCameraBehaviorParam(FName("Override_Debug")));

		//FOV
		float L_FOV = UKismetMathLibrary::Lerp(TP_FOV, TP_FOV, GetCameraBehaviorParam(FName("Weight_FirstPerson")));
		
		OutResult.Location = Debug_Transform.GetLocation();
		OutResult.Rotation = Debug_Transform.Rotator();
		OutResult.FOV = L_FOV;

	}
}


float AStarve_PlayerCameraManager::GetCameraBehaviorParam(FName CurveName)
{
	UAnimInstance* AnimInstance = CameraBehavior->GetAnimInstance();
	if(AnimInstance != nullptr) {
		return AnimInstance->GetCurveValue(CurveName);
	}
	return 0.0f;
}

FVector AStarve_PlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds)
{
	FRotator CameraRotationYaw = FRotator(0, CameraRotation.Yaw, 0);

	//������ռ�ת���ɱ��ؿռ�,���ص���Ȼ������������ϵ�µı�ʾ
	FVector LocalCurrentLocation = UKismetMathLibrary::LessLess_VectorRotator(CurrentLocation, CameraRotationYaw);
	FVector LocalTargetLocation = UKismetMathLibrary::LessLess_VectorRotator(TargetLocation, CameraRotationYaw);

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float X = FMath::FInterpTo(LocalCurrentLocation.X, LocalTargetLocation.X, DeltaTime, LagSpeeds.X);
	float Y = FMath::FInterpTo(LocalCurrentLocation.Y, LocalTargetLocation.Y, DeltaTime, LagSpeeds.Y);
	float Z = FMath::FInterpTo(LocalCurrentLocation.Z, LocalTargetLocation.Z, DeltaTime, LagSpeeds.Z);
	FVector LocalReturnVector = FVector(X, Y, Z);

	FVector ReturnVector = UKismetMathLibrary::GreaterGreater_VectorRotator(LocalReturnVector, CameraRotationYaw);
	return ReturnVector;
}
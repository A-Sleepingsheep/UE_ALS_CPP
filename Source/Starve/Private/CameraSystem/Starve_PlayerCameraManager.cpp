// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraSystem/Starve_PlayerCameraManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Animation/CameraAnimInstance.h"
#include "Interfaces/CameraInterface.h"
#include "Gameplay/StarvePC.h"

FName AStarve_PlayerCameraManager::CameraMeshName(TEXT("CameraMesh"));

AStarve_PlayerCameraManager::AStarve_PlayerCameraManager() {

	//创建摄像机管理对象的Mesh
	CameraMesh = CreateDefaultSubobject<USkeletalMeshComponent>(CameraMeshName);
	CameraMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceFinder(TEXT("AnimBlueprint'/Game/MyALS_CPP/Blueprints/CameraSystem/Starve_Camera_ABP.Starve_Camera_ABP_C'"));
	if (AnimInstanceFinder.Succeeded()) {
		this->GetCameraMesh()->AnimClass = AnimInstanceFinder.Class;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CameraMeshFinder(TEXT("SkeletalMesh'/Game/MyALS_CPP/Blueprints/CameraSystem/Starve_Camera.Starve_Camera'"));
	if (CameraMeshFinder.Object != NULL) {
		CameraMesh->SetSkeletalMesh(CameraMeshFinder.Object);
	}
	
	DebugViewRotation = FRotator(0.f, -5.f, 180.f);
	DebugViewOffset = FVector(350.f, 0.f, 50.f);
}

void AStarve_PlayerCameraManager::OnPlayerControllerPossess(APawn* Pawn)
{
	ControlledPawn = Pawn;

	UCameraAnimInstance* CameraAnimInstance = Cast<UCameraAnimInstance>(this->GetCameraMesh()->GetAnimInstance());
	if (CameraAnimInstance != nullptr) {
		CameraAnimInstance->SetPlayerController(GetOwningPlayerController());
		CameraAnimInstance->SetControlledPawn(ControlledPawn);
	}
}


void AStarve_PlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	/*在更新ViewTarget的时候，如果角色有对应的标签，则调用自己的Update函数*/
	if (OutVT.Target->ActorHasTag("Starve_Character")) {
		CustomCameraBehavior(DeltaTime, OutVT.POV);
	}
	else {
		Super::UpdateViewTargetInternal(OutVT, DeltaTime);
	}
}

void AStarve_PlayerCameraManager::CustomCameraBehavior(float DeltaTime, FMinimalViewInfo& POV)
{
	
	ICameraInterface* CameraInterface = Cast<ICameraInterface>(ControlledPawn);
	if (CameraInterface != nullptr) {
		/*1.获得通过CameraInterface获得相对应的信息，有些值移到第8步了*/
		FTransform Pivot_Target = CameraInterface->Get_TP_PivotTarget();

		/*2.将当前Camera的Rotation跟PlayerController的Rotation进行差值过渡*/
		FRotator PCRotation = GetOwningPlayerController()->GetControlRotation();
		FRotator CameraRotation = GetCameraRotation();
		float Alpha = GetCameraBehaviorCurveValue(FName("RotationLagSpeed"));
		CameraRotation = FMath::RInterpTo(CameraRotation, PCRotation, DeltaTime, Alpha);
		//增加Debug模式下的过渡
		Alpha = GetCameraBehaviorCurveValue(FName("Override_Debug"));
		TargetCameraRotation = UKismetMathLibrary::RLerp(CameraRotation, DebugViewRotation, Alpha, true);

		/*3.利用CalculateAxisIndependentLag和 Pivot_Target(绿色球体) 来计算 SmoothedTargetPivot(橙色球体) 的Location*/
		FVector TempVector;
		TempVector.X = GetCameraBehaviorCurveValue(FName("PivotLagSpeed_X"));
		TempVector.Y = GetCameraBehaviorCurveValue(FName("PivotLagSpeed_Y"));
		TempVector.Z = GetCameraBehaviorCurveValue(FName("PivotLagSpeed_Z"));
		TempVector = CalculateAxisIndependentLag(SmoothedTargetPivot.GetLocation(), Pivot_Target.GetLocation(), TargetCameraRotation, TempVector);
		SmoothedTargetPivot = FTransform(Pivot_Target.GetRotation(), TempVector);

		/*4.SmoothedTargetPivot + 世界坐标系下轴点的偏移 = PivotLocation(蓝色球体)*/
		FVector LocationOffset_X = SmoothedTargetPivot.GetRotation().GetForwardVector() * GetCameraBehaviorCurveValue(FName("PivotOffset_X"));
		FVector LocationOffset_Y = SmoothedTargetPivot.GetRotation().GetRightVector() * GetCameraBehaviorCurveValue(FName("PivotOffset_Y"));
		FVector LocationOffset_Z = SmoothedTargetPivot.GetRotation().GetUpVector() * GetCameraBehaviorCurveValue(FName("PivotOffset_Z"));
		//PivotLocation赋值
		PivotLocation = LocationOffset_X + LocationOffset_Y + LocationOffset_Z + SmoothedTargetPivot.GetLocation();

		/*5.PivotLocation + 世界坐标系下Camera的偏移 = TargetCameraLocation*/
		LocationOffset_X = TargetCameraRotation.Vector() * GetCameraBehaviorCurveValue(FName("CameraOffset_X"));
		LocationOffset_Y = FRotationMatrix(TargetCameraRotation).GetScaledAxis(EAxis::Y) * GetCameraBehaviorCurveValue(FName("CameraOffset_Y"));
		LocationOffset_Z = FRotationMatrix(TargetCameraRotation).GetScaledAxis(EAxis::Z) * GetCameraBehaviorCurveValue(FName("CameraOffset_Z"));
		//TargetCameraLocation赋值
		TargetCameraLocation = LocationOffset_X + LocationOffset_Y + LocationOffset_Z + PivotLocation;
		//在加上Debug模式下的，Alpha值就是上面获得的对应曲线Override_Debug的值
		if (Alpha > 0.5f) {
			TargetCameraLocation = UKismetMathLibrary::VLerp(TargetCameraLocation, Pivot_Target.GetLocation() + DebugViewOffset, Alpha);
		}

		/*6.根据Character提供的位置与Camera位置之间进行球体检测，如果之间有遮挡，则进行像弹簧臂一样的伸缩*/
		//Trave的Start点是TempVector，因为是通过引用的方式，所以在Get_TP_TraceParams后会有值
		// 用上面的Alpha值代替Radius，减小内存消耗
		ETraceTypeQuery TraceChannel;
		Alpha = CameraInterface->Get_TP_TraceParams(TempVector, TraceChannel);
		EDrawDebugTrace::Type drawdebugtrace = GetDebugTraceType(EDrawDebugTrace::ForOneFrame);
		FHitResult OutHit;
		if (UKismetSystemLibrary::SphereTraceSingle(this, TempVector, TargetCameraLocation, 
													Alpha, TraceChannel,
													false, {},
													drawdebugtrace, OutHit, true)) 
		{
			//碰撞点 - 结束点 代表着摄像机往前挪多少，是个负数，TraceEnd一般来说就是填入的 End
			if (OutHit.bBlockingHit && !OutHit.bStartPenetrating) {
				TempVector = OutHit.Location - OutHit.TraceEnd;
				TargetCameraLocation += TempVector;
			}
		}

		/*7.Debug模式下绘制球体*/
		if (Cast<IStarve_ControllerInterface>(GetOwningPlayerController())->I_ShowDebugShapes()) {
			//绘制 Pivot_Target（绿色球体）
			UKismetSystemLibrary::DrawDebugSphere(this, Pivot_Target.GetLocation(), 16.f, 8, FLinearColor::Green, 0.f, 0.5f);
			//绘制 SmoothedTargetPivot（橙色球体）
			UKismetSystemLibrary::DrawDebugSphere(this, SmoothedTargetPivot.GetLocation(), 16.f, 8, FLinearColor(1.f, 0.166667f, 0.f, 1.f), 0.f, 0.5f);
			//绘制 PivotLocation (蓝色球体）
			UKismetSystemLibrary::DrawDebugSphere(this, PivotLocation, 16.f, 8, FLinearColor(0, 0.666667f, 1.f, 1.f), 0.f, 0.5f);
			//绘制 SmoothedTargetPivot 到 Pivot_Target 之间的线条
			UKismetSystemLibrary::DrawDebugLine(this, SmoothedTargetPivot.GetLocation(), Pivot_Target.GetLocation(), FLinearColor(1, 0.166667, 0, 1), 0.f, 1.f);
			//绘制 SmoothedTargetPivot 到 PivotLocation 之间的线条
			UKismetSystemLibrary::DrawDebugLine(this, PivotLocation, SmoothedTargetPivot.GetLocation(), FLinearColor(0, 0.666667f, 1.f, 1.f), 0.f, 1.f);
		}

		/*8.真正设置ViewTarget的值*/
		// FP_CameraTarget:第一人称的Camera Location，使用前面的TempVector节省内存
		TempVector = CameraInterface->Get_FP_CameraTarget();

		float TP_FOV = 0.f;
		CameraInterface->Get_CameraParameters(TP_FOV, Alpha); //这里用Alpha当做FP_FOV

		//分别获得第三人称与第一人称的Transform
		// TP_CameraTransform 使用 Pivot_Target（重复利用，节省内存)
		Pivot_Target = FTransform(TargetCameraRotation, TargetCameraLocation); //第三人称Transform
		FTransform Temp_Transform(TargetCameraRotation, TempVector); //第一人称Transform

		//第一人称跟第三人称根据 Weight_FirstPerson(该值只有0和1两种情况)获得不同的值
		Pivot_Target = UKismetMathLibrary::TLerp(Pivot_Target, Temp_Transform, GetCameraBehaviorCurveValue(FName("Weight_FirstPerson")));
		
		//Debug模式下的Transform
		Temp_Transform = FTransform(DebugViewRotation, TargetCameraLocation);
		Pivot_Target = UKismetMathLibrary::TLerp(Pivot_Target, Temp_Transform, GetCameraBehaviorCurveValue(FName("Override_Debug")));

		//FOV
		Alpha = UKismetMathLibrary::Lerp(TP_FOV, Alpha, GetCameraBehaviorCurveValue(FName("Weight_FirstPerson")));
		
		POV.Location = Pivot_Target.GetLocation();
		POV.Rotation = Pivot_Target.Rotator();
		POV.FOV = Alpha;

	}
}


EDrawDebugTrace::Type AStarve_PlayerCameraManager::GetDebugTraceType(EDrawDebugTrace::Type DrawDebugTrace)
{
	if (Cast<IStarve_ControllerInterface>(GetOwningPlayerController())->I_ShowCameraManagerTraces()) {
		return DrawDebugTrace;
	}
	else {
		return EDrawDebugTrace::None;
	}
}

float AStarve_PlayerCameraManager::GetCameraBehaviorCurveValue(FName CurveName)
{
	UAnimInstance* AnimInstance = CameraMesh->GetAnimInstance();
	if(AnimInstance != nullptr) {
		return AnimInstance->GetCurveValue(CurveName);
	}
	return 0.0f;
}


FVector AStarve_PlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds)
{
	FRotator CameraRotationYaw = FRotator(0, CameraRotation.Yaw, 0);

	//将世界坐标系转换成本地坐标系，在本地坐标系下进行差值
	FVector LocalCurrentLocation = CameraRotationYaw.UnrotateVector(CurrentLocation);
	FVector LocalTargetLocation = CameraRotationYaw.UnrotateVector(TargetLocation);

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float X = FMath::FInterpTo(LocalCurrentLocation.X, LocalTargetLocation.X, DeltaTime, LagSpeeds.X);
	float Y = FMath::FInterpTo(LocalCurrentLocation.Y, LocalTargetLocation.Y, DeltaTime, LagSpeeds.Y);
	float Z = FMath::FInterpTo(LocalCurrentLocation.Z, LocalTargetLocation.Z, DeltaTime, LagSpeeds.Z);
	LocalCurrentLocation = FVector(X, Y, Z);
	//最后返回世界坐标系下Location
	return CameraRotationYaw.RotateVector(LocalCurrentLocation);
}


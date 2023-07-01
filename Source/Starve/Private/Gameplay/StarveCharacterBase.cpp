// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarveCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"

#include "Components/InteractiveComponent.h"
#include "Libraries/Starve_MacroLibrary.h"
#include "Interfaces/Starve_AnimationInterface.h"
#include "Animation/AnimMontage.h"
#include "Interfaces/Starve_ControllerInterface.h"

#pragma region Override
// Sets default values
AStarveCharacterBase::AStarveCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRightShoulder = true;

	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);
	//设置碰撞预设标签为StarveCharacter
	GetCapsuleComponent()->SetCollisionProfileName(FName("StarveCharacter"));

	//添加角色标签
	Tags.Add(FName("Starve_Character"));

	//设置网格体的碰撞预设
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);


	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bRunPhysicsWithNoController = true; //运行物理时无控制器
	GetCharacterMovement()->MaxAcceleration = 1500.0f;   //最大加速度
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f; //制动摩擦系数
	GetCharacterMovement()->CrouchedHalfHeight = 60.0f;  //蹲伏胶囊体半高
	GetCharacterMovement()->MinAnalogWalkSpeed = 25.0f;  //最小模拟步行速度
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;  //蹲伏时可走下平台
	GetCharacterMovement()->PerchRadiusThreshold = 20.0f;  //到达半径阈值
	GetCharacterMovement()->PerchAdditionalHeight = 0.0f;  //到达额外高度
	GetCharacterMovement()->LedgeCheckThreshold = 0.0f;  //平台检测阈值
	GetCharacterMovement()->AirControl = 0.15f;			//空气控制

	//可蹲伏、可飞行等设置需要额外注意
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;  //能蹲伏
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanFly = true;  //可飞行

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 420.f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeleMesh(TEXT("SkeletalMesh'/Game/MyALS_CPP/CharacterAssets/Mesh/Proxy.Proxy'"));
	if (SkeleMesh.Succeeded()) {
		this->GetMesh()->SetSkeletalMesh(SkeleMesh.Object);
		this->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -92), FRotator(0, -90, 0));
	}


	//MovementSettings的初始化
	CurrentMovementSettings.WalkSpeed = 165.f;
	CurrentMovementSettings.RunSpeed = 350.f;
	CurrentMovementSettings.SprintSpeed = 600.f;

	//Timeliness组件
	MantleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MantleTimeline"));

	/*Mantle时间轴所用的曲线赋值*/
	static ConstructorHelpers::FObjectFinder<UCurveFloat> cf2(TEXT("CurveFloat'/Game/MyALS_CPP/Data/Curves/MantleCurve/Mantle_Timeline.Mantle_Timeline'"));
	if (cf2.Succeeded()) {
		MantleTimelineCurve = cf2.Object;
	}

}

// Called when the game starts or when spawned
void AStarveCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	
	//Timeline的事件委托绑定
	FOnTimelineFloat TimelineUpdated;
	FOnTimelineEvent TimelineFinished;
	TimelineUpdated.BindUFunction(this, TEXT("MantleUpdate"));
	TimelineFinished.BindUFunction(this, TEXT("MantleEnd"));

	MantleTimeline->AddInterpFloat(MantleTimelineCurve, TimelineUpdated);
	MantleTimeline->SetTimelineFinishedFunc(TimelineFinished);
	MantleTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	MantleTimeline->SetLooping(false);
	MantleTimeline->SetTimelineLength(5.f);

	OnBeginPlay();
}

// Called every frame
void AStarveCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*1、获得主要的EssentialInformation，基本运动信息*/
	SetEssentialValues();

	if (MovementState == EStarve_MovementState::Grounded) {
		UpdateCharacterMovement(); //更新角色在地面上的移动信息
		UpdateGroundedRotation();//更新在地面上的旋转信息
	}
	else if(MovementState == EStarve_MovementState::InAir){
		UpdateInAirRotation();
		if (bHasMovementInput) {
			//在空中有输入的话进行检测，看有没有可攀爬点
			MantleCheck(FallingTraceSettings, EDrawDebugTrace::ForOneFrame);
		}
	}
	else if (MovementState == EStarve_MovementState::Ragdoll) {
		RagdollUpdate();
	}

	/*第二步，保存当前帧的某些值给下一帧使用*/
	CacheValus();

	/*3.绘制Debug线条*/
	DrawDebugShapes();
}

// Called to bind functionality to input
void AStarveCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	//前进后退
	PlayerInputComponent->BindAxis("MoveForward", this, &AStarveCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStarveCharacterBase::MoveRight);
	//视角
	PlayerInputComponent->BindAxis("Turn", this, &AStarveCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AStarveCharacterBase::LookUp);
	//跳跃，松开事件绑定的是ACharacter的
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AStarveCharacterBase::JumpAction);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//冲刺
	PlayerInputComponent->BindAction("SprintAction", IE_Pressed, this, &AStarveCharacterBase::SprintAction);
	PlayerInputComponent->BindAction("SprintAction", IE_Released, this, &AStarveCharacterBase::StopSprintAction);

	//步行
	PlayerInputComponent->BindAction("WalkAction", IE_Pressed, this, &AStarveCharacterBase::WalkAction);

	//站立蹲伏切换
	PlayerInputComponent->BindAction("StanceAction", IE_Pressed, this, &AStarveCharacterBase::StanceAction);

	//瞄准
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &AStarveCharacterBase::AimPressedAction);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &AStarveCharacterBase::AimReleasedAction);

	//一三人称视角切换事件
	PlayerInputComponent->BindAction("CameraAction", IE_Pressed, this, &AStarveCharacterBase::CameraPressedAction);

	/*布娃娃开启*/
	PlayerInputComponent->BindAction("RagdollAction", IE_Pressed, this, &AStarveCharacterBase::RagdollPressedAction);
}

void AStarveCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	//调用自己的实现
	OnCharacterMovementModeChanged(GetCharacterMovement()->MovementMode);
}
#pragma endregion

const FVector AStarveCharacterBase::GetControllerDirection(EAxis::Type InAxis)
{
	FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	return FRotationMatrix(YawRotation).GetScaledAxis(InAxis);
}


FVector2D AStarveCharacterBase::FixDiagonalGamepadValus(float InX, float InY)
{
	float tempx = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 0.6f), FVector2D(1.f, 1.2f), FMath::Abs(InX));
	float tempy = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 0.6f), FVector2D(1.f, 1.2f), FMath::Abs(InY));

	tempx = FMath::Clamp<float>(InY * tempx, -1.f, 1.f);
	tempy = FMath::Clamp<float>(InX * tempy, -1.f, 1.f);

	return FVector2D(tempy, tempx);
}

void AStarveCharacterBase::DrawDebugShapes()
{
	if (Cast<IStarve_ControllerInterface>(UGameplayStatics::GetPlayerController(this, 0))->I_ShowDebugShapes()) {
		FVector start = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector end;
		FVector velocity = GetVelocity();
		FLinearColor color;
		if (velocity.Equals(FVector(0, 0, 0), 0.0001f)) {
			end = LastVelocityRotation.Vector();
			color = FLinearColor(0.25f, 0, 0.25f, 1);
		}
		else {
			end = velocity;
			color = FLinearColor(1, 0, 1, 1);
		}
		end = start + end.GetUnsafeNormal() * FMath::GetMappedRangeValueClamped(FVector2D(0, GetCharacterMovement()->MaxWalkSpeed), FVector2D(50.f, 75.f), velocity.Size());
		//绘制Velocity方向的箭头
		UKismetSystemLibrary::DrawDebugArrow(this, start, end, 60.f, color, 0, 5.f);

		start = start + FVector(0, 0, 3.5f);
		velocity = GetCharacterMovement()->GetCurrentAcceleration();
		if (velocity.Equals(FVector(0, 0, 0), 0.0001f)) {
			end = LastMovementInputRotation.Vector();
			color = FLinearColor(0.25f, 0.125f, 0, 1);
		}
		else {
			end = velocity;
			color = FLinearColor(0.5f, 0.25f, 0, 1);
		}
		end = start + end.GetUnsafeNormal() * FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(50.f, 75.f), velocity.Size()/GetCharacterMovement()->GetMaxAcceleration());
		//绘制MovementInput箭头
		UKismetSystemLibrary::DrawDebugArrow(this, start, end, 50.f, color, 0, 3.f);

		start = start + FVector(0, 0, 3.5f);
		velocity = TargetRotation.Vector().GetUnsafeNormal();
		end = start + velocity * 50.f;
		color = FLinearColor(0, 0.333333f, 1, 1);
		//绘制TargetRotation箭头
		UKismetSystemLibrary::DrawDebugArrow(this, start, end, 50.f, color, 0, 3.f);

		start = GetMesh()->GetSocketLocation(FName("FP_Camera"));
		end = GetControlRotation().Vector().GetUnsafeNormal();
		color = FLinearColor(0, 0.5f, 1, 1);
		//绘制第一人称视角锥形
		UKismetSystemLibrary::DrawDebugConeInDegrees(this, start, end, 100.f, 30.f, 30.f, 8,color, 0, 0.5f);

		//绘制Character胶囊体
		start = GetActorLocation();
		color = FLinearColor(0, 0, 0, 1);
		UKismetSystemLibrary::DrawDebugCapsule(this, start, GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), GetActorRotation(), color, 0, 0.3f);
	}
}

void AStarveCharacterBase::PlayerMovementInput(bool IsForward)
{
	//只有在地面和空中可以以WASD方式移动
	if (MovementState == EStarve_MovementState::Grounded || MovementState == EStarve_MovementState::InAir) {
		FVector2D fix_value = FixDiagonalGamepadValus(InputComponent->GetAxisValue(FName("MoveForward")), InputComponent->GetAxisValue(FName("MoveRight")));
		float scale_value;
		FVector direction;
		if (IsForward) {
			scale_value = fix_value.X;
			direction = GetControllerDirection(EAxis::X);
		}
		else {
			scale_value = fix_value.Y;
			direction = GetControllerDirection(EAxis::Y);
		}
		AddMovementInput(direction, scale_value);
	}
}

void AStarveCharacterBase::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0) {
		PlayerMovementInput(true);
	}
}

void AStarveCharacterBase::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0) {
		PlayerMovementInput(false);
	}
}

void AStarveCharacterBase::JumpAction()
{
	//前提:没有在播放蒙太奇
	if (MovementAction == EStarve_MovementAction::None) {
		//根据MovementState进行不同的操作
		switch (MovementState)
		{
			case EStarve_MovementState::None:
			case EStarve_MovementState::Grounded:
			case EStarve_MovementState::InAir: {
				if (MovementState == EStarve_MovementState::Grounded) {
					if (bHasMovementInput) {
						//Grounded并且有输入，进行攀爬检测，如果为真，则退出进入攀爬
						if(MantleCheck(GroundedTraceSettings, EDrawDebugTrace::ForDuration)) return;
					}
					switch (Stance)
					{
						//站立时跳跃
						case EStarve_Stance::Standing: {
							Jump();
							break;
						}
						//Crouch时取消蹲伏
						case EStarve_Stance::Crouching: {
							UnCrouch();
							break;
						}
					}
				}
				else if (MovementState == EStarve_MovementState::InAir) {
					MantleCheck(FallingTraceSettings, EDrawDebugTrace::ForDuration);
				}
				break;
			}
			case EStarve_MovementState::Ragdoll: {
				RagdollEnd();
				break;
			}
		}
	}
}


void AStarveCharacterBase::SprintAction()
{
	DesiredGait = EStarve_Gait::Sprinting;
}


void AStarveCharacterBase::StopSprintAction()
{
	DesiredGait = EStarve_Gait::Running;
}


void AStarveCharacterBase::WalkAction()
{
	switch (DesiredGait)
	{
		case EStarve_Gait::Walking: {
			DesiredGait = EStarve_Gait::Running;
			break;
		}
		case EStarve_Gait::Running:{
			DesiredGait = EStarve_Gait::Walking;
			break;
		}
	}
}


void AStarveCharacterBase::OnCharacterMovementModeChanged(EMovementMode CurrentMovementMode)
{
	//根据MovementMode设置状态，这里主要涉及两种状态
	switch (CurrentMovementMode)
	{
		case MOVE_Walking:
		case MOVE_NavWalking: {
			I_SetMovementState(EStarve_MovementState::Grounded);
			break;
		}
		case MOVE_Falling: {
			I_SetMovementState(EStarve_MovementState::InAir);
			break;
		}
	}
}

void AStarveCharacterBase::Turn(float Value)
{
	AddControllerYawInput(Value * LookRightRate);
}

void AStarveCharacterBase::LookUp(float Value)
{
	AddControllerPitchInput(Value * LookUpRate);
}

#pragma region TickFunctionDefinition
void AStarveCharacterBase::SetEssentialValues()
{
	/* 一、计算当前加速度 */
	Acceleration = CalculateAcceleration();

	/* 二、计算速度以及速度判断角色是否移动，同时计算速度的指向*/
	FVector temp_vector = GetVelocity();
	Speed = FVector(temp_vector.X, temp_vector.Y, 0).Size(); //只需要计算XY平面上的速度
	bIsMoving = Speed > 1.f ? true : false;
	if (bIsMoving) {
		LastVelocityRotation = temp_vector.ToOrientationRotator();//该值主要作用是即使角色没有移动，也可以用它来判断角色最后的运动朝向
	}

	/*三、判断是否有键盘输入,输入的值在0~1之间，并且更新最后的移动输入朝向*/
	temp_vector = GetCharacterMovement()->GetCurrentAcceleration();
	MovementInputAmount = temp_vector.Size() / GetCharacterMovement()->GetMaxAcceleration();
	bHasMovementInput = MovementInputAmount > 0.f ? true : false;
	if (bHasMovementInput) {
		LastMovementInputRotation = temp_vector.ToOrientationRotator();
	}

	/*四、Controller的Rotation在Yaw值上的增加速率，是从左向右的*/
	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / UGameplayStatics::GetWorldDeltaSeconds(this));
}

FVector AStarveCharacterBase::CalculateAcceleration()
{
	
	/*（这一时刻的速度 - 前一时刻的速度）/ 时间 = 加速度 */
	return (this->GetVelocity() - PreviousVelocity)/ UGameplayStatics::GetWorldDeltaSeconds(this);
}

void AStarveCharacterBase::CacheValus()
{
	/*保存这一帧的速度与ControllerYaw方向上的大小去给下一帧用*/
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}
#pragma endregion


#pragma region CameraInterfaceDefinition
FVector AStarveCharacterBase::Get_FP_CameraTarget()
{
	return GetMesh()->GetSocketLocation(TEXT("FP_Camera"));
}

FTransform AStarveCharacterBase::Get_TP_PivotTarget()
{
	return GetActorTransform();
}

float AStarveCharacterBase::Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel)
{
	TraceOrigin = GetActorLocation();
	TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
	return 10.0f;
}

bool AStarveCharacterBase::Get_CameraParameters(float& TP_FOV, float& FP_FOV)
{
	TP_FOV = ThirdPerson_FOV;
	FP_FOV = FirstPerson_FOV;
	return bRightShoulder;
}
#pragma	endregion

#pragma region CharacterInterfaceDefinition
FStarveCharacterState AStarveCharacterBase::AStarveCharacterBase::I_GetCurrentState()
{
	FStarveCharacterState starve_character_state;

	starve_character_state.PawnMovementMode = GetCharacterMovement()->MovementMode;
	starve_character_state.MovementState = this->MovementState;
	starve_character_state.PrevMovementState = this->PrevMovementState;
	starve_character_state.MovementAction = this->MovementAction;
	starve_character_state.RotationMode = this->RotationMode;
	starve_character_state.ActualGait = this->Gait;
	starve_character_state.ActualStance = this->Stance;
	starve_character_state.ViewMode = this->ViewMode;
	starve_character_state.OverlayState = this->OverlayState;

	return starve_character_state;
}

FEssentialValues AStarveCharacterBase::I_GetEssentialValues() {
	FEssentialValues essential_values;

	essential_values.Velocity = GetVelocity();
	essential_values.Acceleration = Acceleration;
	essential_values.MovementInput = GetCharacterMovement()->GetCurrentAcceleration();
	essential_values.bIsMoving = this->bIsMoving;
	essential_values.bHasMovementInput = this->bHasMovementInput;
	essential_values.Speed = this->Speed;
	essential_values.MovementInputAmount = this->MovementInputAmount;
	essential_values.AimingRatation = GetControlRotation();
	essential_values.AimYawRate = this->AimYawRate;

	return essential_values;
}

void AStarveCharacterBase::I_SetMovementState(EStarve_MovementState NewMovementState)
{
	if (NewMovementState != MovementState) {
		OnMovementStateChanged(NewMovementState);
	}
}

void AStarveCharacterBase::I_SetMovementAction(EStarve_MovementAction NewMovementAction) {
	if (NewMovementAction != MovementAction) {
		OnMovementActionChanged(NewMovementAction);
	}
}

void AStarveCharacterBase::I_SetRotationMode(EStarve_RotationMode NewRotationMode)
{
	if (NewRotationMode != RotationMode) {
		OnRotationModeChanged(NewRotationMode);
	}
}

void AStarveCharacterBase::I_SetGait(EStarve_Gait NewGait) {
	if (NewGait != Gait) {
		OnGaitChanged(NewGait);
	}
}

void AStarveCharacterBase::I_SetViewMode(EStarve_ViewMode NewViewMode) {
	if (NewViewMode != ViewMode) {
		OnViewModeChanged(NewViewMode);
	}
}

void AStarveCharacterBase::I_SetOverlayState(EStarve_OverlayState NewOverlayState) {
	if (NewOverlayState != OverlayState) {
		OnOverlayStateChanged(NewOverlayState);
	}
}
#pragma endregion

#pragma region CharacterMovementChanged
void AStarveCharacterBase::OnMovementStateChanged(EStarve_MovementState NewMovementState)
{
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_MovementState>(NewMovementState, this->MovementState, this->PrevMovementState);
	switch (MovementState)
	{
		case EStarve_MovementState::InAir: {
			switch (MovementAction)
			{
				case EStarve_MovementAction::None: {
					InAirRotation = GetActorRotation();
					if (Stance == EStarve_Stance::Crouching) {
						UnCrouch();
					}
					break;
				}
				case EStarve_MovementAction::Rolling: {
					RagdollStart();
					break;
				}
			}
			break;
		}
		case EStarve_MovementState::Ragdoll: {
			//开启布娃娃系统的时候停止攀爬，防止摄像机镜头出现问题
			if (PrevMovementState == EStarve_MovementState::Mantling) {
				MantleTimeline->Stop();
			}
			break;
		}
	}
}

void AStarveCharacterBase::OnMovementActionChanged(EStarve_MovementAction NewMovementAction)
{
	EStarve_MovementAction PrevMovementAction = EStarve_MovementAction::None;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_MovementAction>(NewMovementAction, this->MovementAction, PrevMovementAction);

	if (MovementAction == EStarve_MovementAction::Rolling) {
		Crouch();
	}

	if (PrevMovementAction == EStarve_MovementAction::Rolling) {
		switch (DesiredStance)
		{
			case EStarve_Stance::Standing:
				UnCrouch();
				break;

			case EStarve_Stance::Crouching:
				Crouch();
				break;
		}
	}
}

void AStarveCharacterBase::OnRotationModeChanged(EStarve_RotationMode NewRotationMode)
{
	this->RotationMode = NewRotationMode;
	
	//如果RotationMode是Velocity模式并且现在的ViewMode是第一人称，则强制使用第三人称
	if (RotationMode == EStarve_RotationMode::VelocityDirection && ViewMode == EStarve_ViewMode::FirstPerson) {
		I_SetViewMode(EStarve_ViewMode::ThirdPerson);
	}
}

void AStarveCharacterBase::OnGaitChanged(EStarve_Gait NewGait)
{/*
	EStarve_Gait PrevGait;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_Gait>(NewGait, this->Gait, PrevGait);*/
	Gait = NewGait;

}

void AStarveCharacterBase::OnViewModeChanged(EStarve_ViewMode NewViewMode)
{
	this->ViewMode = NewViewMode;
	
	switch (ViewMode)
	{
		case EStarve_ViewMode::ThirdPerson:
			//第三人称下角色的RotationMode根据期望值而定
			if (RotationMode == EStarve_RotationMode::VelocityDirection|| RotationMode == EStarve_RotationMode::LookingDirection) {
				I_SetRotationMode(DesiredRotationMode);
				break;
			}
		case EStarve_ViewMode::FirstPerson: {
			//第一人称下角色的RotationMode只能是Looking模式
			if (RotationMode == EStarve_RotationMode::VelocityDirection) {
				I_SetRotationMode(EStarve_RotationMode::LookingDirection);
				break;
			}
		}
	}
}

void AStarveCharacterBase::OnOverlayStateChanged(EStarve_OverlayState NewOverlayState)
{
	this->OverlayState = NewOverlayState;
}
#pragma endregion

#pragma region BeginPlayFunctions
void AStarveCharacterBase::OnBeginPlay()
{
	/*第一步，确保SkeletonMesh的Tick是打开的，保证AnimBP可以获得最新的数据*/
	GetMesh()->AddTickPrerequisiteActor(this);

	/*第二步，获取并保存动画实例*/
	UAnimInstance* ai = GetMesh()->GetAnimInstance();
	if (IsValid(ai)) {
		this->MainAnimInstance = ai;
	}

	/*第三步，获取DataTable并设置初始的MovementSettings_State为Normal下的数据*/
	SetMovementModel();

	/*第四步，初始化角色状态*/
	OnGaitChanged(this->DesiredGait);
	OnRotationModeChanged(this->DesiredRotationMode);
	OnViewModeChanged(ViewMode);
	OnOverlayStateChanged(OverlayState);
	switch (DesiredStance)
	{
		case EStarve_Stance::Standing: {
			UnCrouch();
			break;
		}
		case EStarve_Stance::Crouching: {
			Crouch();
			break;
		}
	}

	/*第五步，使用角色的开始Rotation初始化一些Rotation值*/
	FRotator rotator = GetActorRotation();
	TargetRotation = rotator;
	LastVelocityRotation = rotator;
	LastMovementInputRotation = rotator;
}


void AStarveCharacterBase::SetMovementModel()
{
	//如果蓝图没有赋值，则使用默认的
	if (MovementModel_DT == nullptr) {
		MovementModel_DT = LoadObject<UDataTable>(NULL, TEXT("DataTable'/Game/MyALS_CPP/Data/DataTable/Starve_MovementModel_DT.Starve_MovementModel_DT'"));
	}
	//查找Normal行对应的MovementData
	FMovementSettings_State* ms_s = MovementModel_DT->FindRow<FMovementSettings_State>(FName("Normal"),"MovementSettings_State Normal");
	if (ms_s != NULL) {
		MovementData = *ms_s;
	}
}

void AStarveCharacterBase::UpdateCharacterMovement()
{
	/*第一步，计算允许的Gait，因为期望的Gait跟能达到的Gait是由差异的*/
	EStarve_Gait allowgait = GetAllowGait();

	/*第二步，根据允许的Gait获得真正的Gait，真正的Gait主要还是根据角色当前的Speed来决定的*/
	EStarve_Gait actualgait = GetActualGait(allowgait);
	if (Gait != actualgait) {
		I_SetGait(actualgait);
	}

	/*根据allowgait更新角色运动相关的设置，包括行走速度，加速度等相关数据*/
	UpdateDynamicMovementSettings(allowgait);

}

EStarve_Gait AStarveCharacterBase::GetAllowGait()
{
	//根据不同的Stance更新允许的Gait
	if (Stance == EStarve_Stance::Standing) {
		if (RotationMode == EStarve_RotationMode::VelocityDirection || RotationMode == EStarve_RotationMode::LookingDirection) {
			switch (DesiredGait)
			{
				case EStarve_Gait::Walking: {
					//站立 + 旋转(Look或Velovity) + 期望是Walking = Walking
					return EStarve_Gait::Walking;
				}
				case EStarve_Gait::Running: {
					//站立 + 旋转(Look或Velovity) + 期望是Running = Walking
					return EStarve_Gait::Running;
				}
				case EStarve_Gait::Sprinting: {
					//站立 + 旋转(Look或Velovity) + 期望是Sprinting = Walking
					return CanSprint() ? EStarve_Gait::Sprinting : EStarve_Gait::Running;
				}
			}
		}
		else if(RotationMode == EStarve_RotationMode::Aiming){
			//在瞄准模式下，角色允许的Gait最大只能是Running
			return (DesiredGait == EStarve_Gait::Walking) ? EStarve_Gait::Walking : EStarve_Gait::Running;
		}
	}
	else if(Stance == EStarve_Stance::Crouching){
		//蹲伏状态下，允许的Gait最大也只能是Running
		return (DesiredGait == EStarve_Gait::Walking) ? EStarve_Gait::Walking : EStarve_Gait::Running;
	}

	return EStarve_Gait::Running;
}

bool AStarveCharacterBase::CanSprint()
{
	if (bHasMovementInput) {
		//有运动输入
		switch (RotationMode)
		{
			case EStarve_RotationMode::VelocityDirection:
				return MovementInputAmount > 0.9 ? true : false; //Velocity模式下输入大于0.9就可以冲刺
			case EStarve_RotationMode::LookingDirection: {
				//Looking模式下，在有运动输入的情况下，还需要判断Controller朝向跟角色运动朝向的Yaw差值是否小于50°，即你只能在某个夹角范围内才可以进行冲刺
				FRotator rotationx = GetCharacterMovement()->GetCurrentAcceleration().ToOrientationRotator();
				rotationx = UKismetMathLibrary::NormalizedDeltaRotator(rotationx, GetControlRotation());
				return (MovementInputAmount > 0.9 && FMath::Abs(rotationx.Yaw) < 50.f) ? true : false;
			}
			case EStarve_RotationMode::Aiming:
				return false; //有运动输入，但是在瞄准状态下不允许冲刺
		}
	}
	return false;
}

EStarve_Gait AStarveCharacterBase::GetActualGait(EStarve_Gait AllowedGait)
{
	//加10.f是因为浮点数不是精确的，加上10对判断影响不大
	if (Speed >= CurrentMovementSettings.RunSpeed + 10.f) {
		switch (AllowedGait) {
			case EStarve_Gait::Walking:
			case EStarve_Gait::Running: {
				return EStarve_Gait::Running;
			}
			case EStarve_Gait::Sprinting: {
				return EStarve_Gait::Sprinting;
			}
		}
	}
	else {
		return (Speed >= CurrentMovementSettings.WalkSpeed + 10.f) ? EStarve_Gait::Running : EStarve_Gait::Walking;
	}

	return AllowedGait;
}


void AStarveCharacterBase::UpdateDynamicMovementSettings(EStarve_Gait AllowedGait)
{
	CurrentMovementSettings = GetTargetMovementSettings();

	/*对上面拿到的数据进行操作*/
	/*先对角色的移动速度进行赋值*/
	float speed = 0.f;
	switch (AllowedGait) {
		case EStarve_Gait::Walking: {
			speed = CurrentMovementSettings.WalkSpeed;
			break;
		}
		case EStarve_Gait::Running: {
			speed = CurrentMovementSettings.RunSpeed;
			break;
		}
		case EStarve_Gait::Sprinting: {
			speed = CurrentMovementSettings.SprintSpeed;
			break;
		}
	}
	GetCharacterMovement()->MaxWalkSpeed = speed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = speed;

	/*通过曲线上的信息给角色一些运动信息赋值*/
	speed = GetMappedSpeed();
	FVector curvevector = CurrentMovementSettings.MovementCurve->GetVectorValue(speed);
	GetCharacterMovement()->MaxAcceleration = curvevector.X;//加速度
	GetCharacterMovement()->BrakingDecelerationWalking = curvevector.Y;//制动减速
	GetCharacterMovement()->GroundFriction = curvevector.Z;//地面摩擦力

}


FMovementSettings AStarveCharacterBase::GetTargetMovementSettings()
{
	switch (RotationMode) {
		case EStarve_RotationMode::VelocityDirection: {
			switch (Stance)
			{
				case EStarve_Stance::Standing:
					return MovementData.VelocityDirection.Standing;
				case EStarve_Stance::Crouching:
					return MovementData.VelocityDirection.Crouching;
			}
		}
		case EStarve_RotationMode::LookingDirection: {
			return (Stance == EStarve_Stance::Standing) ? MovementData.LookingDirection.Standing : MovementData.LookingDirection.Crouching;
		}
		case EStarve_RotationMode::Aiming: {
			return (Stance == EStarve_Stance::Standing) ? MovementData.Aiming.Standing : MovementData.Aiming.Crouching;
		}
	}
	//默认值
	return MovementData.LookingDirection.Standing;
}


float AStarveCharacterBase::GetMappedSpeed()
{
	//主要是根据Speed将角色的Speed分成三个阶段，0~1表示walk，1~2表示run，2~3表示Sprint，就是对应的Gait，主要是从MovementSettings曲线中取对应的值
	if (Speed > CurrentMovementSettings.RunSpeed) {
		return FMath::GetMappedRangeValueClamped(FVector2D(CurrentMovementSettings.RunSpeed, CurrentMovementSettings.SprintSpeed), FVector2D(2, 3), Speed);
	}

	if (Speed > CurrentMovementSettings.WalkSpeed) {
		return FMath::GetMappedRangeValueClamped(FVector2D(CurrentMovementSettings.WalkSpeed, CurrentMovementSettings.RunSpeed), FVector2D(1, 2), Speed);
	}

	return FMath::GetMappedRangeValueClamped(FVector2D(0, CurrentMovementSettings.WalkSpeed), FVector2D(0, 1), Speed);
}
#pragma endregion

//RatationSystem
void AStarveCharacterBase::UpdateGroundedRotation()
{
	//MovementAction用于判断有没有处于蒙太奇状态下
	if (MovementAction == EStarve_MovementAction::None) {
		float rotationrate;
		if (CanUpdateMovingRotation()) {
			rotationrate = CalculateGroundedRotationRate();
			//需要进行每帧更新旋转
			switch (RotationMode)
			{
				case EStarve_RotationMode::VelocityDirection: {
					SmoothCharacterRotation(FRotator(0.f, LastVelocityRotation.Yaw, 0.f), 800.f, rotationrate);
					return;
				}
				case EStarve_RotationMode::LookingDirection: {
					//冲刺状态下的yaw跟走路奔跑有所不同
					float ratationyaw = (Gait == EStarve_Gait::Sprinting) ? LastVelocityRotation.Yaw : GetControlRotation().Yaw + GetAnimCurveValue(FName("YawOffset"));
					SmoothCharacterRotation(FRotator(0, ratationyaw, 0), 500.f, rotationrate);
					return;
				}
				case EStarve_RotationMode::Aiming: {
					SmoothCharacterRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f), 1000.f, 20.f);
					return;
				}
			}
		}
		else {
			//这种情况是角色没有移动，但是Camera视角朝向变了，就需要应用原地转向动画
			//如果是第一人陈或者第三人称但是在Aiming状态的话需要对Rotation进行限制
			if (ViewMode == EStarve_ViewMode::FirstPerson || (ViewMode ==EStarve_ViewMode::ThirdPerson && RotationMode == EStarve_RotationMode::Aiming)) {
				LimitRotation(-100.f, 100.f, 20.f);
			}
			rotationrate = GetAnimCurveValue(FName("RotationAmount"));
			if (FMath::Abs(rotationrate) > 0.001f) {
				//应用TurnInPlace动画中的RotationAmount曲线，它定义了每帧应该旋转多少，同时使用动画的帧数应用到游戏帧数中
				//(1.f / 30.f)是动画的帧数有关，我们的动画是1秒30帧
				rotationrate = rotationrate * (UGameplayStatics::GetWorldDeltaSeconds(this) / (1.f / 30.f));
				AddActorWorldRotation(FRotator(0.f, rotationrate, 0.f));
				TargetRotation = GetActorRotation();
			}
		}
	}
	else if (MovementAction == EStarve_MovementAction::Rolling) {
		if (bHasMovementInput) {
			SmoothCharacterRotation(FRotator(0.f, LastMovementInputRotation.Yaw, 0.f), 0.f, 2.f);
		}
	}
}


bool AStarveCharacterBase::CanUpdateMovingRotation()
{
	//HasAnyRootMotion()是否有根运动
	return ((bIsMoving && bHasMovementInput) || Speed > 150.f) && !HasAnyRootMotion();
}

void AStarveCharacterBase::SmoothCharacterRotation(const FRotator& Target, float TargetInterpSpeed, float ActorInterpSpeed)
{
	//常量差值获得目标旋转
	float delta = UGameplayStatics::GetWorldDeltaSeconds(this);
	//恒定速率插值TargetRotation
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, Target, delta, TargetInterpSpeed);
	//设置角色转向目标旋转
	FRotator newrotator = FMath::RInterpTo(GetActorRotation(), TargetRotation, delta, ActorInterpSpeed);
	SetActorRotation(newrotator);
}

float AStarveCharacterBase::CalculateGroundedRotationRate()
{
	float curvevaule = CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed());
	float maprangeclamp = FMath::GetMappedRangeValueClamped(FVector2D(0, 300), FVector2D(1, 3), AimYawRate);
	return curvevaule * maprangeclamp;
}

float AStarveCharacterBase::GetAnimCurveValue(FName CurveName)
{
	if (IsValid(MainAnimInstance)) {
		return MainAnimInstance->GetCurveValue(CurveName);
	}
	return 0.f;
}

void AStarveCharacterBase::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed)
{
	//根据Controller和Character的旋转Yaw差量
	FRotator control_r = GetControlRotation();
	float rotation_yaw = UKismetMathLibrary::NormalizedDeltaRotator(control_r, GetActorRotation()).Yaw;
	//范围在范围内不进行限制，在范围外进行限制
	if (!UKismetMathLibrary::InRange_FloatFloat(rotation_yaw, AimYawMin, AimYawMax, true, true)) {
		rotation_yaw = rotation_yaw > 0.f ? control_r.Yaw + AimYawMin : control_r.Yaw + AimYawMax;
		SmoothCharacterRotation(FRotator(0, rotation_yaw, 0), 0.f, InterpSpeed);
	}
}

void AStarveCharacterBase::OnJumped_Implementation()
{
	//父类的OnJumped事件只是一个空函数,可以不用调用父类的实现
	//Super::OnJumped_Implementation();

	//设置起跳时角色的初始空中Rotation
	InAirRotation = Speed > 100.f ? LastVelocityRotation : GetActorRotation();

	//使用接口通知动画蓝图
	IStarve_AnimationInterface* ai = Cast<IStarve_AnimationInterface>(MainAnimInstance);
	if (ai != nullptr) {
		ai->I_Jumped();
	}
}


void AStarveCharacterBase::UpdateInAirRotation()
{
	if (RotationMode == EStarve_RotationMode::LookingDirection || RotationMode == EStarve_RotationMode::VelocityDirection) {
		SmoothCharacterRotation(FRotator(0.f, InAirRotation.Yaw, 0.f), 0.f, 5.f);
	}
	else if(RotationMode==EStarve_RotationMode::Aiming) {
		SmoothCharacterRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f), 0.f, 15.f);
		InAirRotation = GetActorRotation();
	}
}


bool AStarveCharacterBase::MantleCheck(FMantle_TraceSettings TraceSettings, EDrawDebugTrace::Type DebugTrace)
{
	/*1、根据运动方向向前后或左右将进行射线检测找到角色不能行走的Location*/
	FVector direction = GetPlayerMovementInput(); 	//获得角色输入的方向
	FVector basecapsulelocation = GetCapsuleBaseLocation(2.f); //获得角色Capsule底部的Location，有2cm的偏移微调

	/*下面的start点以及height值的原因，这样最后的表现效果就是进行检测的胶囊体检测最底部的高度就为MinLedgeHeight*/
	/*最顶部高度就是MaxLedgeHeight*/
	/*也就是允许攀爬最小高度和最大高度，低于或高于这个高度直接跳跃就行了，+1.f主要是加一点偏移量，避免边界条件*/
	FVector start = basecapsulelocation + (-30.f * direction) + FVector(0.f, 0.f, (TraceSettings.MaxLedgeHeight + TraceSettings.MinLedgeHeight) / 2);
	FVector end = start + direction * TraceSettings.ReachDistance;
	float height = (TraceSettings.MaxLedgeHeight - TraceSettings.MinLedgeHeight) / 2 + 1.f;

	EDrawDebugTrace::Type debugtrace = GetTraceDebugType(DebugTrace);
	FHitResult hitresult;
	//第一次胶囊体检测检测的开始点和结束点在运动方向前后或左右先检测前面垂直的墙面，角色不能走是避免斜面的情况
	UKismetSystemLibrary::CapsuleTraceSingle(this, start, end, TraceSettings.ForwardTraceRadius, height,
		ETraceTypeQuery::TraceTypeQuery1, false, {}, debugtrace, hitresult, true,
		FLinearColor::Black, FLinearColor::Blue, 1.0f);

	FVector InitialTrace_ImpactPoint(0.f);/*实际碰撞到的点位置*/
	FVector InitialTrace_Normal(0.f);/*碰撞点处的法线*/

	//GetCharacterMovement()->IsWalkable(hitresult)判断碰撞检测返回的结果角色能否行走，判断这个条件主要是针对斜面
	if (!GetCharacterMovement()->IsWalkable(hitresult) && hitresult.bBlockingHit && !hitresult.bStartPenetrating) {
		//不能进行行走但是有碰撞
		InitialTrace_ImpactPoint = hitresult.ImpactPoint;
		InitialTrace_Normal = hitresult.ImpactNormal;
	}
	else
	{
		//上面条件不满足返回false，代表着没有碰撞到物体，不能进行攀爬
		return false;
	}

	/*2、如果上一步有碰撞，再进行一个Sphere检测，主要是返回检测到的物体最高点的高度*/
	end = FVector(InitialTrace_ImpactPoint.X, InitialTrace_ImpactPoint.Y, basecapsulelocation.Z) + (-15.f * InitialTrace_Normal);
	start = end + FVector(0.f, 0.f, TraceSettings.MaxLedgeHeight + TraceSettings.DownwardTracrRadius + 1.f);
	
	/*这次球形检测的主要目的是获得在墙面上的站立基础点，也就是角色脚步的大致位置*/
	UKismetSystemLibrary::SphereTraceSingle(this, start, end, TraceSettings.DownwardTracrRadius, ETraceTypeQuery::TraceTypeQuery1,
		false, {}, debugtrace, hitresult, true, FLinearColor::Yellow, FLinearColor::Red, 1.f);

	FVector DownTraceLocation(0.f);/*存放球形检测碰撞到的Location*/
	UPrimitiveComponent* HitComponent = nullptr;/*存放碰撞到的组件*/

	if (GetCharacterMovement()->IsWalkable(hitresult) && hitresult.bBlockingHit) {
		DownTraceLocation = FVector(hitresult.Location.X, hitresult.Location.Y, hitresult.ImpactPoint.Z);
		HitComponent = hitresult.GetComponent();
	}
	else {
		return false;
	}

	/*3、通过CapsuleHasRoomCheck检测上一步碰撞到的点是否可以容纳角色胶囊体，如果可以获得攀爬后角色的目标位置以及攀爬高度*/
	FTransform TargetTransform;/*到达点的变换*/

	FVector capsulelocationfrombase = GetCapsuleLocationFromBase(DownTraceLocation, 2.f);
	if (CapsuleHasRoomCheck(GetCapsuleComponent(), capsulelocationfrombase, 0.f, 0.f, debugtrace)) {
		/*乘以(-1.f, -1.f, 0)是因为targetrotation会影响角色攀爬后的面朝向，InitialTrace_Normal的方向和角色的朝向在X和Y方向是相反的，角色最终面朝向的Z朝向由其自身决定，所以Z分量为0*/
		FRotator targetrotation = (InitialTrace_Normal * FVector(-1.f, -1.f, 0)).ToOrientationRotator();
		TargetTransform = FTransform(targetrotation, capsulelocationfrombase, FVector(1.f, 1.f, 1.f));
		//攀爬高度
		height = (capsulelocationfrombase - GetActorLocation()).Z;
	}
	else {
		return false;
	}

	/*4、判断攀爬的类型*/
	if (MovementState == EStarve_MovementState::InAir) {
		MantleType = EMantleType::FallingCatch; /*在空中时属于FallingCatch*/
	}
	else {
		MantleType = height > 125.f ? EMantleType::HighMantle : EMantleType::LowMantle;
	}

	/*5.如果上面一切顺利，那么开始攀爬*/
	/*传递的Component的Transform是世界空间坐标的*/
	//FStarve_ComponentAndTransform MantleLedgeWS(TargetTransform, HitComponent);
	MantleStart(height, FStarve_ComponentAndTransform(TargetTransform, HitComponent), MantleType);
	/*最后return true表示攀爬成功*/
	return true;
}

FVector AStarveCharacterBase::GetCapsuleBaseLocation(float ZOffset)
{
	UCapsuleComponent* capsule = this->GetCapsuleComponent();
	return capsule->GetComponentLocation() - capsule->GetUpVector() * (capsule->GetScaledCapsuleHalfHeight() + ZOffset);
}

FVector AStarveCharacterBase::GetPlayerMovementInput()
{
	float forward = InputComponent->GetAxisValue(FName("MoveForward"));
	float right = InputComponent->GetAxisValue(FName("MoveRight"));

	FVector forward_d = GetControllerDirection(EAxis::X);
	FVector right_d = GetControllerDirection(EAxis::Y);
	//A.GetSafeNormal(0.0001f)获得A向量的单位向量，参数是可接受的浮动范围
	return (forward_d * forward + right_d * right).GetSafeNormal(0.0001f);
}

FVector AStarveCharacterBase::GetCapsuleLocationFromBase(const FVector& BaseLocation, float ZOffset)
{
	return BaseLocation + FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + ZOffset);
}

bool AStarveCharacterBase::CapsuleHasRoomCheck(UCapsuleComponent* Capsule, const FVector& TargetLocation, float HeihtOffset, float RadiusOffset, EDrawDebugTrace::Type DegugType)
{
	//获取不带半球高度的胶囊体缩放半高
	float radius = -1 * RadiusOffset + Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere() + HeihtOffset;
	FVector start = TargetLocation + FVector(0.f, 0.f, radius);
	FVector end = TargetLocation - FVector(0.f, 0.f, radius);
	radius = Capsule->GetScaledCapsuleRadius() + RadiusOffset;
	FHitResult hitresult;

	UKismetSystemLibrary::SphereTraceSingleByProfile(this, start, end, radius,
		FName("Starve_Character"), false, {}, DegugType, hitresult, true, FLinearColor(0.130706f,0.896269f,0.144582f,1), FLinearColor(0.932733f,0.29136f,1,1), 1.f);
	
	//如果没有碰撞到物体就代表可以该地方可以容纳角色
	return !(hitresult.bBlockingHit || hitresult.bStartPenetrating);
}

EDrawDebugTrace::Type AStarveCharacterBase::GetTraceDebugType(EDrawDebugTrace::Type ShowTraceType)
{
	if (Cast<IStarve_ControllerInterface>(UGameplayStatics::GetPlayerController(this, 0))->I_ShowTraces()) {
		return ShowTraceType;
	}
	return EDrawDebugTrace::None;
}

void AStarveCharacterBase::MantleStart(float MantleHeight,const FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType RefMantleType)
{
	/*1、通过对应的MantleType获取对应的MantleAsset,并计算对应的MantleParams的参数*/
	FMantle_Asset MantleAsset = GetMantleAsset(MantleType);
	/*根据攀爬高度获得动画的播放速率和动画开始起点*/
	//该float值是播放速度
	float playrate = FMath::GetMappedRangeValueClamped(FVector2D(MantleAsset.LowHeight, MantleAsset.HighHeight), FVector2D(MantleAsset.LowPlayRate, MantleAsset.HighPlayRate), MantleHeight);
	//该float值是动画开始播放起点
	float startposition = FMath::GetMappedRangeValueClamped(FVector2D(MantleAsset.LowHeight, MantleAsset.HighHeight), FVector2D(MantleAsset.LowStartPosition, MantleAsset.HighStartPosition), MantleHeight);

	MantleParams.AnimMontage = MantleAsset.AnimMontage;
	MantleParams.PositionCorrectionCurve = MantleAsset.PositionCorrectionCurve;
	MantleParams.StartingPosition = startposition;
	MantleParams.PlayRate = playrate;
	MantleParams.StartingOffset = MantleAsset.StartingOffset;
	
	/*2、获得攀爬点在局部坐标系的位置，目的在于如果物体在攀爬过程中发生了世界空间的移动，可以用这两个量反算出移动后的位置*/
	MantleLedgeLS = ComponentWorldToLocal(MantleLedgeWS);

	/*3、将攀爬物体在世界空间下的Transform赋值给MantleTarget，并计算MantleTarget相对于角色Transform的偏移*/
	MantleTarget = MantleLedgeWS.Transform;
	MantleActualStartOffset = TransformSub(GetActorTransform(), MantleTarget);

	/*4、根据上面的结果计算播放攀爬动画时角色要进行的偏移量。目的主要是如果直接进行过渡，一般会出现穿模问题，我们希望角色攀爬的运动是一条弧线*/
	FVector animatedlocation = MantleTarget.Rotator().Vector();
	animatedlocation = animatedlocation * MantleParams.StartingOffset.Y;/*进行Y轴缩放时因为Character类的前方是Y轴*/
	animatedlocation = MantleTarget.GetLocation() - FVector(animatedlocation.X, animatedlocation.Y, MantleParams.StartingOffset.Z);
	FTransform animtedtransform = FTransform(MantleTarget.Rotator(), animatedlocation);
	MantleAnimatedStartOffset = TransformSub(animtedtransform, MantleTarget);/*这个Transform是动画的偏移量*/

	/*5、清除角色移动模式并将移动状态设置为Mantle*/
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	I_SetMovementState(EStarve_MovementState::Mantling);

	/*6、配置Mantle Timeline，使TimelineLength = MaxTime - MantleParams.StartingPosition，设置播放速率。然后开始播放时间线，在TimeLine中每帧更新角色的Transform*/
	/*主要目的是为了动画与曲线同步*/
	//这里使用了上面的两个float值来获得曲线对应的MinTime和MaxTime,优化后的代码（较少内存）
	MantleParams.PositionCorrectionCurve->GetTimeRange(playrate, startposition);
	MantleTimeline->SetTimelineLength(startposition - MantleParams.StartingPosition);
	MantleTimeline->SetPlayRate(MantleParams.PlayRate);
	MantleTimeline->PlayFromStart();

	/*7、如果蒙太奇有效，则播放蒙太奇*/
	if (IsValid(MantleParams.AnimMontage) && IsValid(MainAnimInstance)) {
		MainAnimInstance->Montage_Play(MantleParams.AnimMontage,MantleParams.PlayRate,EMontagePlayReturnType::MontageLength,MantleParams.StartingPosition,false);
	}
}

FMantle_Asset AStarveCharacterBase::GetMantleAsset(EMantleType mantletype)
{
	FMantle_Asset returnmantleasset(NULL, NULL, FVector(0, 0, 0), 0, 0, 0, 0, 0, 0);
	UAnimMontage* AnimMontage;
	UCurveVector* PositionCorrectionCurve;
	FVector StartingOffset;
	float LowHeight;
	float LowPlayRate;
	float LowStartPosition;
	float HighHeight;
	float HighPlayRate;
	float HighStartPosition;
	//攀爬用的两条曲线
	UObject* cv1 = LoadObject<UObject>(this, TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/MantleCurve/Mantle_2m.Mantle_2m'"));
	UObject* cv2 = LoadObject<UObject>(this, TEXT("CurveVector'/Game/MyALS_CPP/Data/Curves/MantleCurve/Mantle_1m.Mantle_1m'"));

	switch (mantletype)
	{
		case EMantleType::HighMantle: {
			AnimMontage = NULL;
			PositionCorrectionCurve = Cast<UCurveVector>(cv1);
			StartingOffset = FVector(0.f, 65.f, 200.f);
			LowHeight = 50.f;
			LowPlayRate = 1.f;
			LowStartPosition = 0.5f;
			HighHeight = 100.f;
			HighPlayRate = 1.f;
			HighStartPosition = 0.f;
			break;
		}
		case EMantleType::LowMantle: {
			AnimMontage = NULL;
			PositionCorrectionCurve = Cast<UCurveVector>(cv2);

			StartingOffset = FVector(0.f, 65.f, 200.f);
			LowHeight = 125.f;
			LowPlayRate = 1.2f;
			LowStartPosition = 0.6f;
			HighHeight = 200.f;
			HighPlayRate = 1.2f;
			HighStartPosition = 0.f;
			break;
		}
		case EMantleType::FallingCatch: {
			AnimMontage = NULL;
			PositionCorrectionCurve = Cast<UCurveVector>(cv2);
			StartingOffset = FVector(0.f, 65.f, 200.f);
			LowHeight = 125.f;
			LowPlayRate = 1.2f;
			LowStartPosition = 0.6f;
			HighHeight = 200.f;
			HighPlayRate = 1.2f;
			HighStartPosition = 0.f;
			break;
		}
	}

	return returnmantleasset;
}

void AStarveCharacterBase::MantleUpdate(float BlendIn)
{
	/*1、根据攀爬物体的局部坐标系的Transform每帧更新物体的世界Transform，这样在攀爬移动物体的时候可以确保角色跟着物体移动*/
	MantleTarget = ComponentLocalToWorld(MantleLedgeLS).Transform;

	/*2、当前Timeline的播放位置+动画的偏移获得当前帧对应的曲线位置，然后从MentleParams的曲线中获得对应的值*/
	float curvetime = MantleTimeline->GetPlaybackPosition() + MantleParams.StartingPosition;
	FVector curvevalue = MantleParams.PositionCorrectionCurve->GetVectorValue(curvetime);
	//优化，下面直接使用CurveValue.x y z
	//float positionalpha = curvevalue.X; /**/
	//float Y_Correctionalpha = curvevalue.Y;/*该值是进行XY平面的TransformOffset差值的Alpha值*/
	//float Z_Correctionalpha = curvevalue.Z;/*该值是进行Z方向的TransformOffset差值的Alpha值*/

	/*3.通过各种差值混合获得当前角色的Transform*/
	FVector temp_location_1 = MantleAnimatedStartOffset.GetLocation();
	FVector temp_location_2 = MantleActualStartOffset.GetLocation();

	//使用 Y_Correctionalpha 混合获得水平方向以及旋转的偏移量,因为Z值用的是MantleActualStartOffset的Z值，所以它的值不变
	FTransform temp_transform(MantleAnimatedStartOffset.Rotator(), FVector(temp_location_1.X, temp_location_1.Y, temp_location_2.Z));
	FTransform transform_offset_xy_rotation = UKismetMathLibrary::TLerp(MantleActualStartOffset, temp_transform, curvevalue.Y);
	
	//使用 Z_Correctionalpha 混合获得垂直偏移量，Z值是MantleAnimatedStartOffset的Z值，XY值不变
	temp_transform = FTransform(MantleActualStartOffset.Rotator(), FVector(temp_location_2.X, temp_location_2.Y, temp_location_1.Z));
	FTransform transform_offset_z = UKismetMathLibrary::TLerp(MantleActualStartOffset, temp_transform, curvevalue.Z);
	
	temp_location_1 = transform_offset_xy_rotation.GetLocation();
	temp_location_2 = transform_offset_z.GetLocation();

	/*获得当前角色的Transform*/
	temp_transform = FTransform(transform_offset_xy_rotation.Rotator(), FVector(temp_location_1.X, temp_location_1.Y, temp_location_2.Z));
	temp_transform = TransformAdd(MantleTarget, temp_transform);
	
	/*该Lerp的A值相当于角色初始Transform，B值是攀爬的TargetTransform*/
	FTransform character_current_transform = UKismetMathLibrary::TLerp(temp_transform, MantleTarget, curvevalue.X);

	/*下面一步是防止攀爬目标低于自身发生抖动现象，主要是Falling Catch的时候*/
	temp_transform = TransformAdd(MantleTarget, MantleActualStartOffset);
	character_current_transform = UKismetMathLibrary::TLerp(temp_transform, character_current_transform, BlendIn);

	/*4、将 Actor 的位置和旋转设置为 character_current_transform*/
	SetActorLocationAndRotationUpdateTarget(character_current_transform.GetLocation(), character_current_transform.Rotator(), false, ETeleportType::None);

}

void AStarveCharacterBase::MantleEnd()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

bool AStarveCharacterBase::SetActorLocationAndRotationUpdateTarget(FVector NewLocation, FRotator NewRotator, bool bSweep, ETeleportType Teleport)
{
	TargetRotation = NewRotator;
	return SetActorLocationAndRotation(NewLocation, NewRotator, bSweep, nullptr, Teleport);
}

void AStarveCharacterBase::StanceAction()
{
	//没有在播放动画蒙太奇的情况下才能执行蹲伏操作
	if (MovementAction == EStarve_MovementAction::None) {
		//是否有多次输入
		if (MultiTapInput(0.3f)) {
			/*有多次按下就执行翻滚*/
			RollEvent();
			switch (Stance)
			{
				case EStarve_Stance::Standing: {
					DesiredStance = EStarve_Stance::Crouching;
					break;
				}
				case EStarve_Stance::Crouching: {
					DesiredStance = EStarve_Stance::Standing;
					break;
				}
			}
		}
		else {
			if (MovementState == EStarve_MovementState::Grounded) {
				switch (Stance)
				{
					case EStarve_Stance::Standing: {
						DesiredStance = EStarve_Stance::Crouching;
						Crouch();
						break;
					}
					case EStarve_Stance::Crouching: {
						DesiredStance = EStarve_Stance::Standing;
						UnCrouch();
						break;
					}
				}
			}
			else if (MovementState == EStarve_MovementState::InAir) {

			}

		}
	}
}

void AStarveCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EStarve_Stance::Crouching);
}

void AStarveCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	OnStanceChanged(EStarve_Stance::Standing);
}

void AStarveCharacterBase::OnStanceChanged(EStarve_Stance NewStance)
{
	Stance = NewStance;
}

void AStarveCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	//在着陆时设置角色的制动摩擦系数，这样可以放置角色落地时的滑步
	float f1 = bHasMovementInput ? 0.5f : 3.f;
	GetCharacterMovement()->BrakingFrictionFactor = f1;

	//0.5秒延迟后重新设置制动摩擦系数
	const FLatentActionInfo LandedActionInfo(0, FMath::Rand(), TEXT("LandedDelay"), this);
	UKismetSystemLibrary::RetriggerableDelay(this, 0.5f, LandedActionInfo);
}

void AStarveCharacterBase::LandedDelay()
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
}

void AStarveCharacterBase::RollEvent()
{
	//只是简单的播放翻滚蒙太奇
	if (IsValid(MainAnimInstance)) {
		MainAnimInstance->Montage_Play(GetRollAnimation(),1.35f);
	}
}

void AStarveCharacterBase::BreakfallEvent()
{
	if (IsValid(MainAnimInstance)) {
		MainAnimInstance->Montage_Play(GetRollAnimation(), 1.35f);
	}
}

UAnimMontage* AStarveCharacterBase::GetRollAnimation()
{
	return nullptr;
}

bool AStarveCharacterBase::MultiTapInput(float ResetTime)
{
	PressCount += 1;

	const FLatentActionInfo MultiInputInfo(0, FMath::Rand(), TEXT("MultiTapInputDelay"), this);
	UKismetSystemLibrary::RetriggerableDelay(this, ResetTime, MultiInputInfo);

	return PressCount < 2 ? false : true;
}

void AStarveCharacterBase::MultiTapInputDelay()
{
	PressCount = 0;
}

void AStarveCharacterBase::RagdollStart()
{
	/*1.设置运动模式*/
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	I_SetMovementState(EStarve_MovementState::Ragdoll);

	/*2.设置碰撞*/
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("Pelvis"), true, true);

	/*3.停止蒙太奇*/
	MainAnimInstance->Montage_Stop(0.2f);
}

void AStarveCharacterBase::RagdollEnd()
{
	/*1.保存姿势快照*/
	if (IsValid(MainAnimInstance)) {
		MainAnimInstance->SavePoseSnapshot(FName("RagdollPose"));
	}

	/*2.播放起身蒙太奇动画，并赋予人物速度*/
	if (bRagdollOnGround) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		MainAnimInstance->Montage_Play(GetGetUpAnimation(bRagdollFaceUp));
	}
	else {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	/*3.重新启用碰撞*/
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

void AStarveCharacterBase::AimPressedAction()
{
	I_SetRotationMode(EStarve_RotationMode::Aiming);
}

void AStarveCharacterBase::AimReleasedAction()
{
	switch (ViewMode)
	{
		case EStarve_ViewMode::ThirdPerson:
			I_SetRotationMode(DesiredRotationMode);
			break;
		case EStarve_ViewMode::FirstPerson:
			I_SetRotationMode(EStarve_RotationMode::LookingDirection);
			break;
	}
}

void AStarveCharacterBase::CameraPressedAction()
{
	switch (ViewMode)
	{
		case EStarve_ViewMode::ThirdPerson:
			I_SetViewMode(EStarve_ViewMode::FirstPerson);
			break;
		case EStarve_ViewMode::FirstPerson:
			I_SetViewMode(EStarve_ViewMode::ThirdPerson);
			break;
	}
}

void AStarveCharacterBase::RagdollPressedAction()
{
	switch (MovementState)
	{
		case EStarve_MovementState::None:
		case EStarve_MovementState::Grounded:
		case EStarve_MovementState::InAir:
		case EStarve_MovementState::Mantling:
			RagdollStart();
			break;
		case EStarve_MovementState::Ragdoll:
			RagdollEnd();
			break;
	}
}



UAnimMontage* AStarveCharacterBase::GetGetUpAnimation(bool RagdollFaceUp)
{
	return nullptr;
}

void AStarveCharacterBase::RagdollUpdate()
{
	/*1.LastRagdollVelocity赋值*/
	LastRagdollVelocity = GetMesh()->GetPhysicsLinearVelocity(FName("root"));

	/*2.缩放LastRagdollVelocity对物理动画的关节强度进行限制*/
	float inspring = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1000.f), FVector2D(0.f, 25000.f), LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(inspring, 0.f, 0.f, false);

	/*3.下落速度超多4000取消重力*/
	GetMesh()->SetEnableGravity(LastRagdollVelocity.Z > -4000.f);

	/*4.更新角色位置*/
	SetActorLocationDuringRagdoll();

}

void AStarveCharacterBase::SetActorLocationDuringRagdoll()
{
	/*1.获得目标位置*/
	FVector TargetRagdollLocation = GetMesh()->GetSocketLocation(FName("pelvis"));

	/*2.获取人物面朝向*/
	FRotator TargetRagdollRotation = GetMesh()->GetSocketRotation(FName("pelvis"));
	bRagdollFaceUp = TargetRagdollRotation.Roll < 0.f;
	float temp_float = bRagdollFaceUp ? TargetRagdollRotation.Yaw - 180.f : TargetRagdollRotation.Yaw;
	TargetRagdollRotation = FRotator(0.f, temp_float, 0.f);

	/*3.射线检测*/
	FVector end(TargetRagdollLocation.X, TargetRagdollLocation.Y, TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FHitResult hitresult;
	UKismetSystemLibrary::LineTraceSingle(this, TargetRagdollLocation, end, TraceTypeQuery1, false, {}, EDrawDebugTrace::None, hitresult, true);
	bRagdollOnGround = hitresult.bBlockingHit;
	if (bRagdollOnGround) {
		temp_float = TargetRagdollLocation.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FMath::Abs(hitresult.ImpactPoint.Z - hitresult.TraceStart.Z) + 2.f;
		TargetRagdollLocation = FVector(TargetRagdollLocation.X, TargetRagdollLocation.Y, temp_float);
	}
	SetActorLocationAndRotationUpdateTarget(TargetRagdollLocation,TargetRagdollRotation,false, ETeleportType::None);
}

FStarve_ComponentAndTransform AStarveCharacterBase::ComponentWorldToLocal(FStarve_ComponentAndTransform WorldSpaceComp)
{
	//获得世界坐标变换矩阵
	FTransform temp_transform = WorldSpaceComp.PrimitiveComponent->GetComponentToWorld();
	//获得局部坐标
	temp_transform = UKismetMathLibrary::InvertTransform(temp_transform);

	temp_transform = UKismetMathLibrary::ComposeTransforms(WorldSpaceComp.Transform, temp_transform);

	return FStarve_ComponentAndTransform(temp_transform, WorldSpaceComp.PrimitiveComponent);
}

FStarve_ComponentAndTransform AStarveCharacterBase::ComponentLocalToWorld(FStarve_ComponentAndTransform LocalSpaceComp)
{
	//获得世界坐标
	FTransform temp_transform = LocalSpaceComp.PrimitiveComponent->GetComponentToWorld();

	temp_transform = UKismetMathLibrary::ComposeTransforms(LocalSpaceComp.Transform, temp_transform);

	return FStarve_ComponentAndTransform(temp_transform, LocalSpaceComp.PrimitiveComponent);
}

FTransform AStarveCharacterBase::TransformSub(const FTransform& A, const FTransform& B)
{
	FVector location = A.GetLocation() - B.GetLocation();
	FRotator rotator = FRotator(A.Rotator().Pitch - B.Rotator().Pitch, A.Rotator().Yaw - B.Rotator().Yaw, A.Rotator().Roll - B.Rotator().Roll);
	FVector scale = A.GetScale3D() - B.GetScale3D();

	return FTransform(rotator, location, scale);
}

FTransform AStarveCharacterBase::TransformAdd(const FTransform& A, const FTransform& B)
{
	FVector location = A.GetLocation() + B.GetLocation();
	FRotator rotator = FRotator(A.Rotator().Pitch + B.Rotator().Pitch, A.Rotator().Yaw + B.Rotator().Yaw, A.Rotator().Roll + B.Rotator().Roll);
	FVector scale = A.GetScale3D() + B.GetScale3D();

	return FTransform(rotator, location, scale);
}

//bool AStarveCharacterBase::HoldInput(float WaitTime)
//{
//	const FLatentActionInfo HoldInputInfo(0, FMath::Rand(), TEXT("MultiTapInputDelay"), this);
//	UKismetSystemLibrary::RetriggerableDelay(this, WaitTime, HoldInputInfo);
//}


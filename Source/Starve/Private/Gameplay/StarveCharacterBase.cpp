// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarveCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"


#include "Components/InteractiveComponent.h"
#include "Libraries/Starve_MacroLibrary.h"

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


}

// Called when the game starts or when spawned
void AStarveCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	//
	OnBeginPlay();
}

// Called every frame
void AStarveCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*第一步*/
	SetEssentialValues(); //获得主要的角色信息
	if (MovementState == EStarve_MovementState::Grounded) {
		UpdateCharacterMovement(); //更新角色在地面上的移动信息
		UpdateGroundedRotation();//更新在地面上的旋转信息
	}

	CacheValus();

	//UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(Speed),true,false,FLinearColor::Blue,0.f);
	/*第二步，保存当前的某些信息*/


}

// Called to bind functionality to input
void AStarveCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	//前进后退
	PlayerInputComponent->BindAxis("MoveForward", this, &AStarveCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStarveCharacterBase::MoveRight);
	//视线
	PlayerInputComponent->BindAxis("Turn", this, &AStarveCharacterBase::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AStarveCharacterBase::LookUp);
	//跳跃
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AStarveCharacterBase::JumpAction);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//冲刺
	PlayerInputComponent->BindAction("SprintAction", IE_Pressed, this, &AStarveCharacterBase::SprintAction);
	PlayerInputComponent->BindAction("SprintAction", IE_Released, this, &AStarveCharacterBase::StopSprintAction);

	//步行
	PlayerInputComponent->BindAction("WalkAction", IE_Pressed, this, &AStarveCharacterBase::WalkAction);
}

void AStarveCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	//调用自己的实现
	OnCharacterMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}
#pragma endregion

#pragma region CharacterMoveDefinition
const FVector AStarveCharacterBase::GetControllerDirection(EAxis::Type InAxis)
{
	FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(InAxis);
	return Direction;
}

FVector2D AStarveCharacterBase::FixDiagonalGamepadValus(float InX, float InY)
{
	float tempx = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 0.6f), FVector2D(1.f, 1.2f), FMath::Abs(InX));
	float tempy = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 0.6f), FVector2D(1.f, 1.2f), FMath::Abs(InY));

	tempx = FMath::Clamp<float>(InY * tempx, -1.f, 1.f);
	tempy = FMath::Clamp<float>(InX * tempy, -1.f, 1.f);

	return FVector2D(tempy, tempx);
}

void AStarveCharacterBase::Starve_PlayerMovementInput(bool IsForward)
{
	//只有在地面和空中可以以WASD方式移动
	if (MovementState == EStarve_MovementState::Grounded || MovementState == EStarve_MovementState::InAir) {
		float forward = InputComponent->GetAxisValue(FName("MoveForward"));
		float right = InputComponent->GetAxisValue(FName("MoveRight"));
		FVector2D fix_value = FixDiagonalGamepadValus(forward, right);
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
		Starve_PlayerMovementInput(true);
	}
}

void AStarveCharacterBase::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0) {
		Starve_PlayerMovementInput(false);
	}
}

void AStarveCharacterBase::JumpAction()
{
	if (MovementAction == EStarve_MovementAction::None) {
		if (MovementState == EStarve_MovementState::None || MovementState == EStarve_MovementState::Grounded || MovementState == EStarve_MovementState::InAir) {
			if (MovementState == EStarve_MovementState::Grounded) {
				Jump();
			}else if(MovementState == EStarve_MovementState::InAir) {

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
		default:
			break;
	}
}


void AStarveCharacterBase::OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	EMovementMode mm = GetCharacterMovement()->MovementMode;
	IStarve_CharacterInterface* ic = Cast<IStarve_CharacterInterface>(this);
	if (ic != nullptr) {
		if (mm == EMovementMode::MOVE_Walking || mm == EMovementMode::MOVE_NavWalking) {
			ic->I_SetMovementState(EStarve_MovementState::Grounded);
		}
		else if (mm == EMovementMode::MOVE_Falling) {
			ic->I_SetMovementState(EStarve_MovementState::InAir);
		}
	}
}
#pragma endregion


#pragma region CameraViewDefinition
void AStarveCharacterBase::Turn(float Value)
{
	AddControllerYawInput(Value * LookRightRate);
}

void AStarveCharacterBase::LookUp(float Value)
{
	AddControllerPitchInput(Value * LookUpRate);
}

#pragma endregion

#pragma region TickFunctionDefinition
void AStarveCharacterBase::SetEssentialValues()
{
	/* 一、计算当前加速度 */
	Acceleration = CalculateAcceleration();

	/* 二、计算速度以及速度判断角色是否移动，同时计算速度的指向*/
	FVector velocity = GetVelocity();
	Speed = FVector(velocity.X, velocity.Y, 0).Size(); //只需要计算XY平面上的速度
	bIsMoving = Speed > 1.f ? true : false;
	LastVelocityRotation = bIsMoving ? velocity.ToOrientationRotator() : FRotator(0.f);

	/*三、判断是否有键盘输入，并根据判断更新键盘输入的某些变量*/
	FVector current_acceleration = GetCharacterMovement()->GetCurrentAcceleration();
	MovementInputAmount = current_acceleration.Size() / GetCharacterMovement()->GetMaxAcceleration();
	bHasMovementInput = MovementInputAmount > 0.f ? true : false;
	LastMovementInputRotation = bHasMovementInput ? current_acceleration.ToOrientationRotator() : FRotator(0.f);

	/*四、获得控制器Yaw方向的旋转速度*/
	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / GetWorld()->GetDeltaSeconds());
}

FVector AStarveCharacterBase::CalculateAcceleration()
{
	/*（这一时刻的速度 - 前一时刻的速度）/ 时间 = 加速度 */
	return (this->GetVelocity() - PreviousVelocity)/GetWorld()->GetDeltaSeconds();
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
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_MovementState>(NewMovementState, this->MovementState)) {
		OnMovementStateChanged(NewMovementState);
	}
}

void AStarveCharacterBase::I_SetMovementAction(EStarve_MovementAction NewMovementAction) {
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_MovementAction>(NewMovementAction, this->MovementAction)) {
		OnMovementActionChanged(NewMovementAction);
	}
}

void AStarveCharacterBase::I_SetRotationMode(EStarve_RotationMode NewRotationMode)
{
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_RotationMode>(NewRotationMode, this->RotationMode)) {
		OnRotationModeChanged(NewRotationMode);
	}
}

void AStarveCharacterBase::I_SetGait(EStarve_Gait NewGait) {
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_Gait>(NewGait, this->Gait)) {
		OnGaitChanged(NewGait);
	}
}

void AStarveCharacterBase::I_SetViewMode(EStarve_ViewMode NewViewMode) {
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_ViewMode>(NewViewMode, this->ViewMode)) {
		OnViewModeChanged(NewViewMode);
	}
}

void AStarveCharacterBase::I_SetOverlayState(EStarve_OverlayState NewOverlayState) {
	if (UStarve_MacroLibrary::ML_IsDifferent<EStarve_OverlayState>(NewOverlayState, this->OverlayState)) {
		OnOverlayStateChanged(NewOverlayState);
	}
}
#pragma endregion

#pragma region CharacterMovementChanged
void AStarveCharacterBase::OnMovementStateChanged(EStarve_MovementState NewMovementState)
{
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_MovementState>(NewMovementState, this->MovementState, this->PrevMovementState);

}

void AStarveCharacterBase::OnMovementActionChanged(EStarve_MovementAction NewMovementAction)
{
	EStarve_MovementAction PrevMovementAction;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_MovementAction>(NewMovementAction, this->MovementAction, PrevMovementAction);

}

void AStarveCharacterBase::OnRotationModeChanged(EStarve_RotationMode NewRotationMode)
{
	EStarve_RotationMode PrevRotationMode;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_RotationMode>(NewRotationMode, this->RotationMode, PrevRotationMode);
}

void AStarveCharacterBase::OnGaitChanged(EStarve_Gait NewGait)
{
	EStarve_Gait PrevGait;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_Gait>(NewGait, this->Gait, PrevGait);

}

void AStarveCharacterBase::OnViewModeChanged(EStarve_ViewMode NewViewMode)
{
	EStarve_ViewMode PrevViewMode;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_ViewMode>(NewViewMode, this->ViewMode, PrevViewMode);

}

void AStarveCharacterBase::OnOverlayStateChanged(EStarve_OverlayState NewOverlayState)
{
	EStarve_OverlayState PrevOverlayState;
	UStarve_MacroLibrary::ML_SetPreviousAndNewValue<EStarve_OverlayState>(NewOverlayState, this->OverlayState, PrevOverlayState);

}
#pragma endregion

#pragma region BeginPlayFunctions
void AStarveCharacterBase::OnBeginPlay()
{
	/*第一步*/
	GetMesh()->AddTickPrerequisiteActor(this);//添加先决条件的TickActor，确保ABP是有效的

	/*第二步*/
	//获取并保存动画实例
	UAnimInstance* ai = GetMesh()->GetAnimInstance();
	if (IsValid(ai)) {
		this->MainAnimInstance = ai;
	}

	/*第三步*/
	//获取DataTable并获取初始Normal状态下的MovementSettings_State
	SetMovementModel();

	/*第四步*/
	//根据结构体信息设置属性
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

	/*第五步*/
	//初始化一些旋转值
	FRotator rotator = GetActorRotation();
	TargetRotation = rotator;
	LastVelocityRotation = rotator;
	LastMovementInputRotation = rotator;
}


void AStarveCharacterBase::SetMovementModel()
{
	UDataTable* dt = LoadObject<UDataTable>(NULL,TEXT("DataTable'/Game/MyALS_CPP/Data/DataTable/Starve_MovementModel_DT.Starve_MovementModel_DT'"));
	if (dt != NULL) {
		MovementModel_DT = dt;
		FMovementSettings_State* ms_s = dt->FindRow<FMovementSettings_State>(FName("Normal"),"MovementSettings_State");
		if (ms_s != NULL) {
			MovementData = *ms_s;
		}
	}
}

void AStarveCharacterBase::UpdateCharacterMovement()
{
	/*第一步*/
	EStarve_Gait allowgait = GetAllowGait(); //允许的Gait，因为期望的Gait跟能达到的Gait是由差异的

	/*第二步*/
	//决定真正的Gait并赋值给动画蓝图用的Gait
	EStarve_Gait actualgait = GetActualGait(allowgait);
	if (Gait != actualgait) {
		I_SetGait(actualgait);
	}

	UpdateDynamicMovementSettings(allowgait);

}

EStarve_Gait AStarveCharacterBase::GetAllowGait()
{
	EStarve_Gait gait = EStarve_Gait::Walking;
	if (Stance == EStarve_Stance::Standing) {
		//站立
		if (RotationMode == EStarve_RotationMode::VelocityDirection || RotationMode == EStarve_RotationMode::LookingDirection) {
			//站立 + 旋转(Look或Velovity)
			switch (DesiredGait)
			{
				case EStarve_Gait::Walking: {
					//站立 + 旋转(Look或Velovity) + 期望是Walking = Walking
					gait = EStarve_Gait::Walking;
					break;
				}
				case EStarve_Gait::Running: {
					//站立 + 旋转(Look或Velovity) + 期望是Running = Walking
					gait = EStarve_Gait::Running;
					break;
				}
				case EStarve_Gait::Sprinting: {
					//站立 + 旋转(Look或Velovity) + 期望是Sprinting = Walking
					gait = CanSprint() ? EStarve_Gait::Sprinting : EStarve_Gait::Running;
					break;
				}
			}
		}
		else {
			//站立状态下瞄准
			//站立 + 旋转(瞄准) + 期望是Walking = Walking
			//站立 + 旋转(瞄准) + 期望是Running = Running
			gait = (DesiredGait == EStarve_Gait::Walking) ? EStarve_Gait::Walking : EStarve_Gait::Running;
		}
	}
	else {
		//下蹲 //下蹲 + 期望Walking = Walking
		//下蹲 + 其它期望Gait = Running
		gait = (DesiredGait == EStarve_Gait::Walking) ? EStarve_Gait::Walking : EStarve_Gait::Running;
	}

	return gait;
}

bool AStarveCharacterBase::CanSprint()
{
	if (bHasMovementInput) {
		//有运动输入
		switch (RotationMode)
		{
			case EStarve_RotationMode::VelocityDirection:
				return MovementInputAmount > 0.9 ? true : false; //输入大于0.9可以冲刺
			case EStarve_RotationMode::LookingDirection: {
				//有运动输入，但是在Looking的Rotation模式需要进行进一步判断
				FVector current_ac = GetCharacterMovement()->GetCurrentAcceleration();
				FRotator rotationx = current_ac.ToOrientationRotator();
				FRotator delta_r = UKismetMathLibrary::NormalizedDeltaRotator(rotationx, GetControlRotation());
				//限制可以进行冲刺的方向（只有当你的视角跟角色冲刺的方向夹角小于50°时才可以进行冲刺，即你不可以直接按A或D进行冲刺）
				return (MovementInputAmount > 0.9 && FMath::Abs(delta_r.Yaw) < 50.f) ? true : false;
			}
			case EStarve_RotationMode::Aiming:
				return false; //有运动输入，但是在瞄准状态下不允许冲刺
		}
	}
	return false;
}

EStarve_Gait AStarveCharacterBase::GetActualGait(EStarve_Gait AllowedGait)
{
	EStarve_Gait gait;
	//加10.f是因为浮点数不是精确的，加上10对判断影响不大
	if (Speed >= CurrentMovementSettings.RunSpeed + 10.f) {
		switch (AllowedGait) {
			case EStarve_Gait::Walking:
			case EStarve_Gait::Running: {
				gait = EStarve_Gait::Running;
				break;
			}
			default:
				gait = EStarve_Gait::Sprinting;
		}
	}
	else {
		gait = (Speed >= CurrentMovementSettings.WalkSpeed + 10.f) ? EStarve_Gait::Running : EStarve_Gait::Walking;
	}
	return gait;
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
	float mapspeed = GetMappedSpeed();
	FVector curvevector = CurrentMovementSettings.MovementCurve->GetVectorValue(speed);
	GetCharacterMovement()->MaxAcceleration = curvevector.X;//加速度
	GetCharacterMovement()->BrakingDecelerationWalking = curvevector.Y;//制动减速
	GetCharacterMovement()->GroundFriction = curvevector.Z;//地面摩擦力

}


FMovementSettings AStarveCharacterBase::GetTargetMovementSettings()
{
	FMovementSettings ms;

	switch (RotationMode) {
		case EStarve_RotationMode::VelocityDirection: {
			ms = (Stance == EStarve_Stance::Standing) ? MovementData.VelocityDirection.Standing : MovementData.VelocityDirection.Crouching;
		}
		case EStarve_RotationMode::LookingDirection: {
			ms = (Stance == EStarve_Stance::Standing) ? MovementData.LookingDirection.Standing : MovementData.LookingDirection.Crouching;
		}
		case EStarve_RotationMode::Aiming: {
			ms = (Stance == EStarve_Stance::Standing) ? MovementData.Aiming.Standing : MovementData.Aiming.Crouching;
		}
	}

	return ms;
}


float AStarveCharacterBase::GetMappedSpeed()
{
	float speed;
	float mapwalkspeed, maprunspeed, mapsprintspeed;
	/*下面的目的时间Speed的数值映射到0-3之间，方便去曲线中取值*/
	/*0-1*/
	mapwalkspeed = FMath::GetMappedRangeValueClamped(FVector2D(0, CurrentMovementSettings.WalkSpeed), FVector2D(0, 1), Speed);
	/*1-2*/
	maprunspeed = FMath::GetMappedRangeValueClamped(FVector2D(CurrentMovementSettings.WalkSpeed,CurrentMovementSettings.RunSpeed), FVector2D(1, 2), Speed);
	/*2-3*/
	mapsprintspeed = FMath::GetMappedRangeValueClamped(FVector2D(CurrentMovementSettings.RunSpeed,CurrentMovementSettings.SprintSpeed), FVector2D(2,3), Speed);

	speed = (Speed > CurrentMovementSettings.WalkSpeed) ? maprunspeed : mapwalkspeed;
	speed = (Speed > CurrentMovementSettings.RunSpeed) ? mapsprintspeed : speed;

	return speed;
}
#pragma endregion

//RatationSystem
void AStarveCharacterBase::UpdateGroundedRotation()
{
	//MovementAction用于判断有没有处于蒙太奇状态下
	if (MovementAction == EStarve_MovementAction::None) {
		if (CanUpdateMovingRotation()) {
			switch (RotationMode)
			{
			case EStarve_RotationMode::VelocityDirection:

				break;
			case EStarve_RotationMode::LookingDirection: {
				float rotationrate = CalculateGroundedRotationRate();
				//冲刺状态下的yaw跟走路奔跑有所不同
				float ratationyaw = (Gait == EStarve_Gait::Sprinting) ? LastVelocityRotation.Yaw : GetControlRotation().Yaw + GetAnimCurveValue(FName("YawOffset"));
				SmoothCharacterRotation(FRotator(0, ratationyaw, 0), 500.f, rotationrate);
			}
				break;
			case EStarve_RotationMode::Aiming:
				break;
			}
		}
		else {

		}
	}
	else if (MovementAction == EStarve_MovementAction::Rolling) {

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

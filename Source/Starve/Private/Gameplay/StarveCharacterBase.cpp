// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/StarveCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/InteractiveComponent.h"

// Sets default values
AStarveCharacterBase::AStarveCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRightShoulder = true;

	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);
	//设置碰撞预设为StarveCharacter
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

	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;

	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeleMesh(TEXT("SkeletalMesh'/Game/MyALS_CPP/CharacterAssets/Mesh/Proxy.Proxy'"));
	checkf(SkeleMesh.Succeeded(), TEXT("Can't find TankRightTrack Mesh."));
	this->GetMesh()->SetSkeletalMesh(SkeleMesh.Object);
	this->GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -92), FRotator(0, -90, 0));
	
}

// Called when the game starts or when spawned
void AStarveCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStarveCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*第一步，计算只要信息*/
	SetEssentialValues();

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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

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
#pragma endregion

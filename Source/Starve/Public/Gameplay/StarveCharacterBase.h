// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/CameraInterface.h"
#include "Interfaces/Starve_CharacterInterface.h"

#include "StarveCharacterBase.generated.h"

class UCameraComponent;
class USpringArmComponent;


UCLASS()
class STARVE_API AStarveCharacterBase : public ACharacter, public ICameraInterface, public IStarve_CharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStarveCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;


#pragma region CameraSystem
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bRightShoulder; 

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float ThirdPerson_FOV = 90.0f; /*第三人称FOV*/

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float FirstPerson_FOV = 90.0f; /*第一人称FOV*/

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookRightRate = 1.25;/*Controller水平变化速度，主要用于视线水平变化*/

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookUpRate = 1.25;/*Controller竖直变化速度，主要用于视线竖直变化*/

#pragma endregion

private:
	#pragma region EssentialInformation
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector Acceleration;/*加速度*/

	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FVector PreviousVelocity;/*前一时刻的速度*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Speed;/*XOY平面的速度*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsMoving;/*判断是否正在移动*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastVelocityRotation;/*XY平面速度旋转*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MovementInputAmount;/*可以用来判断是否有输入，它是0-1之间的值*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastMovementInputRotation;/*输入的旋转*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bHasMovementInput;/*判断是否有输入*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AimYawRate;/*控制器Yaw旋转的速度*/

	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PreviousAimYaw;/*前一时刻控制器Yaw方向旋转的速度*/
	#pragma endregion


	#pragma region AnimRelativeVariablies
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimInstance* MainAnimInstance;

	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* MovementModel_DT; //DataTable

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMovementSettings_State MovementData;

	/*下面三个参数是一开始的期望参数*/
	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_Gait DesiredGait = EStarve_Gait::Running;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_RotationMode DesiredRotationMode = EStarve_RotationMode::LookingDirection;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_Stance DesiredStance= EStarve_Stance::Standing;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FRotator TargetRotation;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMovementSettings CurrentMovementSettings;
	#pragma endregion


	#pragma region CharacterEnums
	EStarve_MovementState MovementState;  //当前状态
	EStarve_MovementState PrevMovementState;//上一帧的状态
	EStarve_MovementAction MovementAction;
	EStarve_RotationMode RotationMode;
	EStarve_Gait Gait; //主要状态
	EStarve_ViewMode ViewMode;
	EStarve_OverlayState OverlayState;
	EStarve_Stance Stance;
	#pragma endregion

protected:
	#pragma region CharacterMovementAndView
	/**
	* 根据Controller的Rotation获得方向
	* 改写ALS系统的 GetControlledForward/RightVector
	*/
	const FVector GetControllerDirection(EAxis::Type InAxis);

	/*人物移动*/
	void Starve_PlayerMovementInput(bool IsForward);

	/*修正输入对角线的信息*/
	FVector2D FixDiagonalGamepadValus(float InX, float InY);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void JumpAction();//跳跃操作

	void SprintAction();//冲刺

	void StopSprintAction();//停止冲刺

	void WalkAction();//冲刺

	void StopWalkAction();//停止冲刺

	void OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);
	#pragma endregion

	#pragma region TickFunctions
	/*计算角色运动所需要的数值*/
	void SetEssentialValues();

	/*计算角色加速度,原版角色加速度的变化是瞬间的，我们希望获得一个变化的加速度*/
	FVector CalculateAcceleration();

	/*储存前一帧的一些变量*/
	void CacheValus();
	#pragma endregion

public:
	#pragma region CameraInterface
	virtual FVector Get_FP_CameraTarget() override;
	virtual FTransform Get_TP_PivotTarget() override;
	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) override;
	virtual bool Get_CameraParameters(float& TP_FOV, float& FP_FOV) override;
	#pragma endregion


	#pragma region CharacterInterfaceGerInformation
	virtual FStarveCharacterState I_GetCurrentState() override; //返回枚举状态的信息
	virtual FEssentialValues I_GetEssentialValues() override;  //将在Tick中获取的主要信息传递出去	

	virtual void I_SetMovementState(EStarve_MovementState NewMovementState) override;/*设置角色运动状态*/
	virtual void I_SetMovementAction(EStarve_MovementAction NewMovementAction) override;/*设置角色移动时正在干什么*/
	virtual void I_SetRotationMode(EStarve_RotationMode NewRotationMode) override;	/*设置摄像机旋转模式*/
	virtual void I_SetGait(EStarve_Gait NewGait) override;	/*设置主行走状态*/
	virtual void I_SetViewMode(EStarve_ViewMode NewViewMode) override;	/*设置视角模式*/
	virtual void I_SetOverlayState(EStarve_OverlayState NewOverlayState) override;	/*设置叠加状态*/
	#pragma endregion

	#pragma region MovementStateChange
	void OnMovementStateChanged(EStarve_MovementState NewMovementState);
	void OnMovementActionChanged(EStarve_MovementAction NewMovementAction);
	void OnRotationModeChanged(EStarve_RotationMode NewRotationMode);
	void OnGaitChanged(EStarve_Gait NewGait);
	void OnViewModeChanged(EStarve_ViewMode NewViewMode);
	void OnOverlayStateChanged(EStarve_OverlayState NewOverlayState);
	#pragma endregion

	#pragma region OnBeginPlayFunctions
	void OnBeginPlay(); //BeginPlay初始化
	void SetMovementModel(); //初始化设置角色的枚举值
	void UpdateCharacterMovement(); //每帧更新角色在地面上的移动
	EStarve_Gait GetAllowGait();//获得当前状态下允许的行走Gait
	bool CanSprint(); //能否进行冲刺
	EStarve_Gait GetActualGait(EStarve_Gait AllowedGait);//获得真正可以用的Gait
	void UpdateDynamicMovementSettings(EStarve_Gait AllowedGait);//实时更新角色的Movement数据，就是将曲线中的信息应用到角色
	FMovementSettings GetTargetMovementSettings();//获得对应旋转模式下MovementSettings里面的数据
	float GetMappedSpeed();//获取曲线对应的速度
	#pragma endregion
};

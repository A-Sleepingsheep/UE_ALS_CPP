// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Interfaces/CameraInterface.h"
#include "Interfaces/Starve_CharacterInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include "StarveCharacterBase.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTimelineComponent;


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
	/***************Override Parent Functions************************************************/
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/*父类中响应Crouch的事件*/
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/*Character从空中进入地面时触发*/
	virtual void Landed(const FHitResult& Hit) override;
	/*****************************************************************************************/


	/***********************Camera System Setting Variables**********************************************/
	/*Camera视角是否在右肩*/
	UPROPERTY(Category = CameraSystem, VisibleAnywhere, BlueprintReadWrite)
	bool bRightShoulder; 
	
	/*第三人称FOV*/
	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadWrite)
	float ThirdPerson_FOV = 90.0f; 

	/*第一人称FOV*/
	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadWrite)
	float FirstPerson_FOV = 90.0f; 
	/*****************************************************************************************************/

	/***********Character Input Bind Functions***********************************************************/
	/*视角水平变化速度*/
	UPROPERTY(Category = ViewInputRate, EditAnywhere, BlueprintReadWrite)
	float LookRightRate = 1.25f;

	/*视角垂直变化速度*/
	UPROPERTY(Category = ViewInputRate, EditAnywhere, BlueprintReadWrite)
	float LookUpRate = 1.25f;

private:
	/*根据Controller的Yaw值进行对应方向向量的获取，X：Forward，Y：Right，Z：Up*/
	const FVector GetControllerDirection(EAxis::Type InAxis);

	/*人物移动*/
	void PlayerMovementInput(bool IsForward);

	/*这一步只要是针对手柄进行摇杆对角线数值的修正*/
	FVector2D FixDiagonalGamepadValus(float InX, float InY);

	/*Tick绘制Debug线条*/
	void DrawDebugShapes();

public:
	/*MoveForward轴映射绑定事件绑定函数*/
	virtual void MoveForward(float Value);

	/*MoveRight轴映射绑定函数*/
	virtual void MoveRight(float Value);

	/*Camera Input，Turn轴映射绑定函数，左右移动视角*/
	virtual void Turn(float Value);

	/*Camera Input，LookUp轴映射绑定函数，上下移动视角*/
	virtual void LookUp(float Value);

	/*JumpAction动作映射按键按下绑定函数*/
	virtual void JumpAction();

	/*SprintAction动作映射按键按下绑定函数，将DesiredGait设为Sprinting*/
	virtual void SprintAction();

	/*SprintAction按键松开绑定函数，将DesiredGait设为Running*/
	virtual void StopSprintAction();

	/*WalkAction动作映射按键按下绑定函数，Walking是行走速度最小的那种*/
	virtual void WalkAction();

	/*AimAction按键按下绑定函数*/
	virtual void AimPressedAction();

	/*AimAction按键松开绑定函数*/
	virtual void AimReleasedAction();

	/*StanceAction按键按下绑定函数*/
	virtual void StanceAction();

	/*CameraAction按键按下绑定函数*/
	virtual void CameraPressedAction();

	/*CameraAction按键松开绑定函数，暂时还未定义*/
	//virtual void CameraReleasedAction();

	/*开启Ragdoll系统*/
	virtual void RagdollPressedAction();
	/*******************************************************************************************/

	/****************BeginPlayFunctions**********************************************************/
private:
	/*SkeletonMesh的AnimInstance*/
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimInstance* MainAnimInstance;

	/*MovementModel的DataTable*/
	UPROPERTY(Category = MovementSystem, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UDataTable* MovementModel_DT;

	/*记录了角色不同Rotation模式下相关的速度信息以及过渡曲线*/
	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FMovementSettings_State MovementData;

	/*Character的期望Gait值，初始的默认DesiredGait为Running*/
	UPROPERTY(Category = DesiredVariables, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EStarve_Gait DesiredGait = EStarve_Gait::Running;

	/*Character的期望RotationMode值，游戏开始的默认值LookingDirection*/
	UPROPERTY(Category = DesiredVariables, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EStarve_RotationMode DesiredRotationMode = EStarve_RotationMode::LookingDirection;


	/*期望Stance，开始默认为Standing*/
	UPROPERTY(Category = DesiredVariables, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EStarve_Stance DesiredStance= EStarve_Stance::Standing;

	//Character Begin Play初始化
	void OnBeginPlay(); 

	//初始化设置角色的枚举值
	void SetMovementModel(); 

protected:
	/************Character Essential Information****************************************************************/
	/*加速度*/
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector Acceleration;

	/*前一时刻(上一帧)的速度，缓存值*/
	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector PreviousVelocity;

	/*XY平面的速率*/
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;/*判断是否正在移动*/

	/*上一帧角色XY平面的的速度朝向*/
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator LastVelocityRotation;

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MovementInputAmount;/*可以用来判断是否有输入，它是0-1之间的值*/

	/*上一帧玩家运动输入的朝向*/
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator LastMovementInputRotation;

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bHasMovementInput;/*判断是否有输入*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AimYawRate;/*控制器Yaw旋转的速度*/

	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PreviousAimYaw;/*前一时刻控制器Yaw方向旋转的速度*/
	/***************************************************************************************************************/

	#pragma region AnimRelativeVariablies

	/*角色朝向的目标值*/
	UPROPERTY(Category = "RotationSystem", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator TargetRotation;
	
	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FMovementSettings CurrentMovementSettings;

	UPROPERTY(Category = "RotationSystem", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator InAirRotation;//刚跳跃起来的角色朝向

	//在空中时的攀爬检测数据
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FMantle_TraceSettings FallingTraceSettings = FMantle_TraceSettings(150.f,50.f,70.f,30.f,30.f);

	//在地面上的攀爬检测数据
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FMantle_TraceSettings GroundedTraceSettings = FMantle_TraceSettings(250.f,50.f,75.f,30.f,30.f);


	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EMantleType MantleType;

	/*跟攀爬Montage相关的参数*/
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FMantle_Params MantleParams;

	/*局部坐标下攀爬点对应的Primitive组件和Transform*/
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FStarve_ComponentAndTransform MantleLedgeLS;

	/*攀爬点的World Transform*/
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform MantleTarget;

	/*攀爬点的实际开始偏移量*/
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FTransform MantleActualStartOffset;

	/*攀爬动画的开始偏移量*/
	UPROPERTY(Category = MantleSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform MantleAnimatedStartOffset;

	/*Mantle的时间轴*/
	UPROPERTY(Category = MantleSystem,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* MantleTimeline;

	/*时间轴所用的曲线*/
	UPROPERTY(Category = MantleSystem,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MantleTimelineCurve;

	/*是否打断下落执行翻滚*/
	UPROPERTY(Category = Roll,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bBreakfall;

	/*多次点击次数*/
	UPROPERTY(Category = Roll,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int PressCount = 0;

	/*Ragdoll是否在地面上*/
	UPROPERTY(Category = RagdollSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bRagdollOnGround;
	
	/*Ragdoll是否起身*/
	UPROPERTY(Category = RagdollSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bRagdollFaceUp;

	/*Ragdoll前的速度*/
	UPROPERTY(Category = RagdollSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector LastRagdollVelocity;

	#pragma endregion


	UPROPERTY(Category = "StateValues", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementState MovementState;  //当前状态

	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementState PrevMovementState;//上一帧的状态

	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_MovementAction MovementAction;

	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_RotationMode RotationMode;

	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Gait Gait; //主要状态

	/*OverlayState，开始默认为Default*/
	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_OverlayState OverlayState = EStarve_OverlayState::Default;

	/*Character的视角模式，开始默认是第三人称*/
	UPROPERTY(Category = "StateValues",VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_ViewMode ViewMode = EStarve_ViewMode::ThirdPerson;

	UPROPERTY(Category = StateValues,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EStarve_Stance Stance;

protected:
	

	void OnCharacterMovementModeChanged(EMovementMode CurrentMovementMode);

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
	/*获得第一人称摄像机的位置*/
	virtual FVector Get_FP_CameraTarget() override;

	/*获得第三人称的视角目标，在子类中有重写，第三人称下角色的PivotTarget是root跟head的中间点*/
	virtual FTransform Get_TP_PivotTarget() override;

	virtual float Get_TP_TraceParams(FVector& TraceOrigin, ETraceTypeQuery& TraceChannel) override;

	/*获得TP_FOV,FP_FOV*/
	virtual bool Get_CameraParameters(float& TP_FOV, float& FP_FOV) override;
	#pragma endregion


	#pragma region CharacterInterfaceGerInformation
	UFUNCTION(BlueprintCallable)
	virtual FStarveCharacterState I_GetCurrentState() override; //返回枚举状态的信息
	virtual FEssentialValues I_GetEssentialValues() override;  //将在Tick中获取的主要信息传递出去	

	virtual void I_SetMovementState(EStarve_MovementState NewMovementState) override;/*设置角色运动状态*/
	virtual void I_SetMovementAction(EStarve_MovementAction NewMovementAction) override;/*设置角色移动时正在干什么*/
	virtual void I_SetRotationMode(EStarve_RotationMode NewRotationMode) override;	/*设置摄像机旋转模式*/
	virtual void I_SetGait(EStarve_Gait NewGait) override;	/*设置主行走状态*/
	virtual void I_SetViewMode(EStarve_ViewMode NewViewMode) override;	/*设置视角模式*/

	UFUNCTION(BlueprintCallable)
	virtual void I_SetOverlayState(EStarve_OverlayState NewOverlayState) override;	/*设置叠加状态*/
	#pragma endregion

	#pragma region MovementStateChange
	void OnMovementStateChanged(EStarve_MovementState NewMovementState);
	void OnMovementActionChanged(EStarve_MovementAction NewMovementAction);
	void OnRotationModeChanged(EStarve_RotationMode NewRotationMode);
	void OnGaitChanged(EStarve_Gait NewGait);
	void OnViewModeChanged(EStarve_ViewMode NewViewMode);
	virtual void OnOverlayStateChanged(EStarve_OverlayState NewOverlayState);
	#pragma endregion

	#pragma region OnBeginPlayFunctions

	/*每帧更新角色在Grounded时的一些信息*/
	void UpdateCharacterMovement();

	/*根据Stance和RotationMode每帧调用获得允许的Gait*/
	EStarve_Gait GetAllowGait();

	/*能否进行冲刺的判断，对冲刺的行为进行限制*/
	bool CanSprint();

	/*获得真正可以用的Gait，主要是根据角色的Speed来决定当前的Gait，这样当角色处于减速状态时能有正确的Gait*/
	EStarve_Gait GetActualGait(EStarve_Gait AllowedGait);

	/*根据Gait实时更新角色的Movement数据，将结构体中的一些数据应用到角色*/
	void UpdateDynamicMovementSettings(EStarve_Gait AllowedGait);

	//获得Rotation模式下MovementSettings
	FMovementSettings GetTargetMovementSettings();
	
	//将角色的Speed映射到0~3范围，0~1代表Walk，1~2代表Run，2~3代表Sprint
	float GetMappedSpeed();
	#pragma endregion

	//RotationSystem
	//更新在地面上的旋转模式
	void UpdateGroundedRotation();

	//判断是否更新运动旋转
	bool CanUpdateMovingRotation();
	
	//平滑过渡人物Rotation
	void SmoothCharacterRotation(const FRotator& Target,float TargetInterpSpeed,float ActorInterpSpeed);

	//通过MovementSettings中的RotationRate曲线跟AimYawRate结合计算人物在地面上的旋转速度
	float CalculateGroundedRotationRate();


	float GetAnimCurveValue(FName CurveName);//获得动画曲线值


	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed);//限制Rotation

	//跳跃事件,里面调用了在动画蓝图中实现的接口I_Jumped()，主要是为了实现动画蓝图与角色动作的同步
	virtual void OnJumped_Implementation() override;

	//处于跳跃转态时（处于空中时)
	void UpdateInAirRotation();

	//MantleSystem
	/*通过多次射线检测判定角色是否可以攀爬，获取攀爬障碍物的Transform以及Component，以及根据攀爬高度确定攀爬类型*/
	bool MantleCheck(FMantle_TraceSettings TraceSettings,EDrawDebugTrace::Type DebugTrace);

	/**
	* 获得胶囊体底部的Location，ZOffset是Z方向上的偏移，用于细微调整
	* @return 从胶囊体中心Location向下偏移胶囊体半高+ZOffset
	*/
	FVector GetCapsuleBaseLocation(float ZOffset);

	/*获得人物在进行攀爬检测时是往哪边运动的*/
	FVector GetPlayerMovementInput();

	/*对BaseLocation的值进行GetCapsuleBaseLocation的逆运算获得所需Capsule胶囊体的位置*/
	FVector GetCapsuleLocationFromBase(const FVector& BaseLocation, float ZOffset);

	/*检测是否有足够的空间容纳胶囊体，HeightOffset和RadiusOffset是用于微调的量*/
	bool CapsuleHasRoomCheck(UCapsuleComponent* Capsule, const FVector& TargetLocation, float HeihtOffset, float RadiusOffset, EDrawDebugTrace::Type DegugType);

	EDrawDebugTrace::Type GetTraceDebugType(EDrawDebugTrace::Type ShowTraceType);
	
protected:

	/*攀爬实现*/
	virtual void MantleStart(float MantleHeight,const FStarve_ComponentAndTransform& MantleLedgeWS, EMantleType RefMantleType);

	/*通过MantleType获得对应的MantleAsset*/
	virtual FMantle_Asset GetMantleAsset(EMantleType MantleType);

public:
	/*MantleTimeliness的Update函数*/
	UFUNCTION(BlueprintCallable)
	void MantleUpdate(float BlendIn);

	/*MantleTimeliness的结束调用函数*/
	UFUNCTION(BlueprintCallable)
	virtual void MantleEnd();

	/*攀爬更新角色位置和旋转*/
	bool SetActorLocationAndRotationUpdateTarget(FVector NewLocation, FRotator NewRotator, bool bSweep, ETeleportType Teleport);




	/*响应蹲伏事件要改变的参数*/
	void OnStanceChanged(EStarve_Stance NewStance);


	/*着落延迟相应函数*/
	UFUNCTION()
	void LandedDelay();

	/*翻滚事件*/
	void RollEvent();

	/*在着陆前的那一刻执行翻滚的事件*/
	void BreakfallEvent();

	/*获取Roll翻滚需要播放的蒙太奇动画，子类重写*/
	virtual UAnimMontage* GetRollAnimation();

	/*是否连续输入,false表示一次，true表示多次*/
	bool MultiTapInput(float ResetTime);

	/*判断是否连续输入后触发*/
	UFUNCTION()
	void MultiTapInputDelay();

	/*布娃娃系统开始*/
	void RagdollStart();

	/*布娃娃系统结束*/
	void RagdollEnd();





	///*是否长按*/
	//bool HoldInput(float WaitTime);

	///*判断是否长按的bool值*/
	//bool bIsHold;

	///*HoldInputDelay*/
	//void HoldInputDelay();



	/*获得Ragdoll起身的动画蒙太奇,子类重载*/
	virtual UAnimMontage* GetGetUpAnimation(bool RagdollFaceUp);

	/*Ragdoll每帧更新*/
	void RagdollUpdate();

	/*更新Ragdoll时角色的位置*/
	void SetActorLocationDuringRagdoll();

	/*将组件的世界坐标需转变成局部坐标系*/
	static FStarve_ComponentAndTransform ComponentWorldToLocal(FStarve_ComponentAndTransform WorldSpaceComp);
	
	/*将组件从局部坐标系转到世界坐标系*/
	static FStarve_ComponentAndTransform ComponentLocalToWorld(FStarve_ComponentAndTransform LocalSpaceComp);

	/*自定义变换减法,只是简单的的对应的值相减，主要是计算攀爬最终点跟起点的偏移*/
	static FTransform TransformSub(const FTransform& A, const FTransform& B);

	/*自定义变换加法,主要是对Rotator个分量对应进行相加*/
	static FTransform TransformAdd(const FTransform& A, const FTransform& B);
};

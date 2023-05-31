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
		float ThirdPerson_FOV = 90.0f; /*�����˳�FOV*/

	UPROPERTY(Category = CameraSystem, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float FirstPerson_FOV = 90.0f; /*��һ�˳�FOV*/

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookRightRate = 1.25;/*Controllerˮƽ�仯�ٶȣ���Ҫ��������ˮƽ�仯*/

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float LookUpRate = 1.25;/*Controller��ֱ�仯�ٶȣ���Ҫ����������ֱ�仯*/

#pragma endregion

private:
	#pragma region EssentialInformation
	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector Acceleration;/*���ٶ�*/

	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FVector PreviousVelocity;/*ǰһʱ�̵��ٶ�*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Speed;/*XOYƽ����ٶ�*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsMoving;/*�ж��Ƿ������ƶ�*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastVelocityRotation;/*XYƽ���ٶ���ת*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MovementInputAmount;/*���������ж��Ƿ������룬����0-1֮���ֵ*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FRotator LastMovementInputRotation;/*�������ת*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bHasMovementInput;/*�ж��Ƿ�������*/

	UPROPERTY(Category = EssentialInformation, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AimYawRate;/*������Yaw��ת���ٶ�*/

	UPROPERTY(Category = CachedVariables, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PreviousAimYaw;/*ǰһʱ�̿�����Yaw������ת���ٶ�*/
	#pragma endregion


	#pragma region AnimRelativeVariablies
	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimInstance* MainAnimInstance;

	UPROPERTY(Category = Ref, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* MovementModelDT; //DataTable

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FMovementSettings_State MovementSettings_State;

	/*��������������һ��ʼ����������*/
	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_Gait DesiredGait = EStarve_Gait::Running;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_RotationMode DesiredRotationMode = EStarve_RotationMode::LookingDirection;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EStarve_Stance DesiredStance= EStarve_Stance::Standing;

	UPROPERTY(Category = MovementSystem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FRotator TargetRotation;
	#pragma endregion


	#pragma region CharacterEnums
	EStarve_MovementState MovementState;  //��ǰ״̬
	EStarve_MovementState PrevMovementState;//��һ֡��״̬
	EStarve_MovementAction MovementAction;
	EStarve_RotationMode RotationMode;
	EStarve_Gait Gait; //��Ҫ״̬
	EStarve_ViewMode ViewMode;
	EStarve_OverlayState OverlayState;
	EStarve_Stance Stance;
	#pragma endregion

protected:
	#pragma region CharacterMovementAndView
	/**
	* ����Controller��Rotation��÷���
	* ��дALSϵͳ�� GetControlledForward/RightVector
	*/
	const FVector GetControllerDirection(EAxis::Type InAxis);

	/*�����ƶ�*/
	void Starve_PlayerMovementInput(bool IsForward);

	/*��������Խ��ߵ���Ϣ*/
	FVector2D FixDiagonalGamepadValus(float InX, float InY);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void JumpAction();//��Ծ����

	void OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);
	#pragma endregion

	#pragma region TickFunctions
	/*�����ɫ�˶�����Ҫ����ֵ*/
	void SetEssentialValues();

	/*�����ɫ���ٶ�,ԭ���ɫ���ٶȵı仯��˲��ģ�����ϣ�����һ���仯�ļ��ٶ�*/
	FVector CalculateAcceleration();

	/*����ǰһ֡��һЩ����*/
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
	virtual FStarveCharacterState I_GetCurrentState() override; //����ö��״̬����Ϣ
	virtual FEssentialValues I_GetEssentialValues() override;  //����Tick�л�ȡ����Ҫ��Ϣ���ݳ�ȥ	

	virtual void I_SetMovementState(EStarve_MovementState NewMovementState) override;/*���ý�ɫ�˶�״̬*/
	virtual void I_SetMovementAction(EStarve_MovementAction NewMovementAction) override;/*���ý�ɫ�ƶ�ʱ���ڸ�ʲô*/
	virtual void I_SetRotationMode(EStarve_RotationMode NewRotationMode) override;	/*�����������תģʽ*/
	virtual void I_SetGait(EStarve_Gait NewGait) override;	/*������״̬*/
	virtual void I_SetViewMode(EStarve_ViewMode NewViewMode) override;	/*�����ӽ�ģʽ*/
	virtual void I_SetOverlayState(EStarve_OverlayState NewOverlayState) override;	/*���õ���״̬*/
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
	void OnBeginPlay(); //BeginPlay��ʼ��
	void SetMovementModel(); //��ʼ�����ý�ɫ��ö��ֵ
	void UpdateCharacterMovement(); //ÿ֡���½�ɫ�ڵ����ϵ��ƶ�
	EStarve_Gait GetAllowGait();//��õ�ǰ״̬�����������Gait
	bool CanSprint(); //�ܷ���г��
	#pragma endregion
};

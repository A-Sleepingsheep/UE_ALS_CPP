// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Starve_LocomotionEnum.generated.h"


/*
	Starve_LocomotionEnum
	��ɫ���ߵ�״̬
*/
UENUM(BlueprintType)
enum class EStarve_Gait : uint8 {
	//��������
	Walking,

	//�ܲ�
	Running,

	//���
	Sprinting
};

/*��ɫ�ƶ�ʱ�Ķ�������¼�ƶ�ʱ���ڸɵ���*/
UENUM(BlueprintType)
enum class EStarve_MovementAction : uint8 {
	/*�ޣ�������ҪMontageʱ��״̬���������ߡ��ܲ������*/
	None,

	/*��Ծ��ǽʱ��״̬*/
	LowMantle,

	/*��Ծ��ǽʱ��״̬*/
	HighMantle,

	/*����*/
	Rolling,

	/*����*/
	GettingUp,

};

/*�˶�ʱ������״̬,��¼���ڿ��У������*/
UENUM(BlueprintType)
enum class EStarve_MovementState : uint8 {
	None,

	/*�ڵ���*/
	Grounded,

	/*�ڿ���*/
	InAir,

	/*���ڷ�ǽ*/
	Mantling,

	/*������ϵͳ*/
	Ragdoll
};


/*��¼����״̬���������ǹ��·��*/
UENUM(BlueprintType)
enum class EStarve_OverlayState : uint8 {
	Default,
	Masculine
};


/*���������תģʽ*/
UENUM(BlueprintType)
enum class EStarve_RotationMode : uint8 {
	VelocityDirection,

	/*ƽʱ*/
	LookingDirection,

	/*��׼ʱ*/
	Aiming
};


/*����״̬�µ�״̬������������׷�*/
UENUM(BlueprintType)
enum class EStarve_Stance : uint8 {
	/*վ��*/
	Standing,

	/*�׷�*/
	Crouch
};


/*�ӽ�ģʽ����һ������˳�*/
UENUM(BlueprintType)
enum class EStarve_ViewMode : uint8 {
	/*�����˳�*/
	ThirdPerson,

	/*��һ�˳�*/
	FirstPerson
};


/*��������ʾ��*/
UENUM(BlueprintType)
enum class EAnimFeatureExample : uint8 {
	/*������*/
	StrideBlending,

	/*����̬*/
	AdditiveLeaning,

	/**/
	SprintImpulse
};


/*�Ų�״̬*/
UENUM(BlueprintType)
enum class EFootstepType : uint8 {
	Step,
	WalkOrRun,
	Jump,
	Land
};


/*��������ǰ��״̬*/
UENUM(BlueprintType)
enum class EGroundedEntryState : uint8 {
	None,
	Roll
};


/*�粽�����ĳ���*/
UENUM(BlueprintType)
enum class EHipsDirection : uint8 {
	F,
	B,
	RF,
	RB,
	LF,
	LB
};


/*��������*/
UENUM(BlueprintType)
enum class EMantleType : uint8 {
	HighMantle,
	LowMantle,
	FallingCatch
};


/*���������תģʽ*/
UENUM(BlueprintType)
enum class EMovementDirecction : uint8 {
	Forward,
	Right,
	Left,
	Backward
};


/*������Ϊ�˱༭���и������λ��*/
UCLASS()
class STARVE_API UStarve_LocomotionEnum : public UObject
{
	GENERATED_BODY()
	
};

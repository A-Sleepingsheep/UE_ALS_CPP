// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARVE_API IInteractiveInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/* 交互对象进入交互者范围时触发，例如人物进入一扇门前时显示文字 */
	virtual void Enter(class AActor* User) = 0 ;

	/* 交互对象持续处于交互者范围时触发，Tick函数，例如当人物持续处于一扇门前时，文字始终面向人物 */
	virtual void StayTick(class AActor* User, float DeltaTime) = 0;

	/* 交互对象离开交互者范围时，例如人物远离门，门的文字消失等。 */
	virtual void Exit(class AActor* User) = 0;

	/* 区别于Enter,Enter是交互对象在交互范围可以被交互但还没有进行交互，该函数是交互者跟交互对象正式开始交互 
	*  @param InteractiveComponent  交互者的InteractiveComponent
	*/
	virtual void BeginInteractive(class UInteractiveComponent* InteractiveComponent) = 0;

	/* 交互者与交互对象结束交互 */
	virtual void EndInteractive(class UInteractiveComponent* InteractiveComponent) = 0;
	
	/* 交互者与交互对象持续进行交互 */
	virtual void InteractiveTick(class UInteractiveComponent* InteractiveComponent, float DeltaTime) = 0;

};

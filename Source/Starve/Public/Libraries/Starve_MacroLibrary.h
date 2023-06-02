// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Starve_MacroLibrary.generated.h"


/**
 * 
 */
UCLASS()
class STARVE_API UStarve_MacroLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/*是否不相同*/
	template<typename T>
	static bool ML_IsDifferent(const T A, const T B);

	template<typename T>
	static void ML_SetPreviousAndNewValue(const T NewValue, T& NewTarget, T& Previous);

};

template<typename T>
inline bool UStarve_MacroLibrary::ML_IsDifferent(const T A, const T B) {
	return A != B ? true : false;
}


template<typename T>
inline void UStarve_MacroLibrary::ML_SetPreviousAndNewValue(const T NewValue, T& NewTarget, T& Previous)
{
	Previous = NewTarget;
	NewTarget = NewValue;
}

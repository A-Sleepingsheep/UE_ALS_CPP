// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/Starve_MacroLibrary.h"
#include "Kismet/KismetMathLibrary.h"


FStarve_ComponentAndTransform UStarve_MacroLibrary::ML_ComponentWorldToLocal(FStarve_ComponentAndTransform WorldSpaceComp)
{
	//获得世界坐标变换矩阵
	FTransform worldspacetransform = WorldSpaceComp.PrimitiveComponent->GetComponentToWorld();
	//获得局部坐标
	FTransform inverttransform = UKismetMathLibrary::InvertTransform(worldspacetransform);

	FTransform composetransform = UKismetMathLibrary::ComposeTransforms(WorldSpaceComp.Transform, inverttransform);

	return FStarve_ComponentAndTransform(composetransform, WorldSpaceComp.PrimitiveComponent);
}

FTransform UStarve_MacroLibrary::ML_TransformSub(const FTransform& A, const FTransform& B)
{
	FVector location = A.GetLocation() - B.GetLocation();
	FRotator rotator = FRotator(A.Rotator().Pitch - B.Rotator().Pitch, A.Rotator().Yaw - B.Rotator().Yaw, A.Rotator().Roll - B.Rotator().Roll);
	FVector scale = A.GetScale3D() - B.GetScale3D();

	return FTransform(rotator, location, scale);
}

FStarve_ComponentAndTransform UStarve_MacroLibrary::ML_ComponentLocalToWorld(FStarve_ComponentAndTransform LocalSpaceComp)
{
	//获得世界坐标
	FTransform worldspacetransform = LocalSpaceComp.PrimitiveComponent->GetComponentToWorld();

	FTransform composetransform = UKismetMathLibrary::ComposeTransforms(LocalSpaceComp.Transform, worldspacetransform);

	return FStarve_ComponentAndTransform(composetransform, LocalSpaceComp.PrimitiveComponent);
}

FTransform UStarve_MacroLibrary::ML_TransformAdd(const FTransform& A, const FTransform& B)
{
	FVector location = A.GetLocation() + B.GetLocation();
	FRotator rotator = FRotator(A.Rotator().Pitch + B.Rotator().Pitch, A.Rotator().Yaw + B.Rotator().Yaw, A.Rotator().Roll + B.Rotator().Roll);
	FVector scale = A.GetScale3D() + B.GetScale3D();

	return FTransform(rotator, location, scale);
}
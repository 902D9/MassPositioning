// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PositioningDebugLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MASSPOSITIONING_API UPositioningDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 范围内随机生成点位数据
	 * @param WorldContextObject 
	 * @param DebugPositioningPoints 
	 * @param DebugPersonData 
	 * @param GeneratedPoints 
	 * @param LastGeneratedPoints 上一次生成的点位
	 * @param PointsNum 点位数量
	 * @param UpdateTime 更新间隔时间
	 * @param Origin 模拟生产中心
	 * @param Radius 范围
	 */
	UFUNCTION(BlueprintCallable,
		meta=(WorldContext = "WorldContextObject", AdvancedDisplay = "UpdateTime, Origin, Radius"))
	static void GeneratePositioningDebugPoints(UObject* WorldContextObject,
	                                           TMap<FString, FVector>& DebugPositioningPoints,
	                                           TMap<FString, FPersonDataFragment>& DebugPersonData,
	                                           TArray<FVector>& GeneratedPoints,
	                                           TArray<FVector> LastGeneratedPoints, int32 PointsNum = 100,
	                                           float UpdateTime = 1.0f, FVector Origin = FVector::ZeroVector,
	                                           float Radius = 5000.0f);

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", AdvancedDisplay = "Duration, Color"))
	static void DrawPositioningDebug(UObject* WorldContextObject,
	                                 FVector Location,
	                                 FPersonDataFragment PersonData,
	                                 float Duration = 1.0f, FLinearColor Color = FLinearColor::Green);
};

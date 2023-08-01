// Fill out your copyright notice in the Description page of Project Settings.


#include "Positioning/PositioningDebugLibrary.h"

#include "NavigationSystem.h"
#include "AI/PersonnelPositioningTrait.h"
#include "Kismet/KismetSystemLibrary.h"

void UPositioningDebugLibrary::GeneratePositioningDebugPoints(UObject* WorldContextObject,
                                                              TMap<FString, FVector>& DebugPositioningPoints,
                                                              TMap<FString, FPersonDataFragment>& DebugPersonData,
                                                              TArray<FVector>& GeneratedPoints,
                                                              TArray<FVector> LastGeneratedPoints, int32 PointsNum,
                                                              float UpdateTime, FVector Origin, float Radius)
{
	for (int i = 0; i < PointsNum; i++)
	{
		FVector RandomLocation;
		if (LastGeneratedPoints.IsValidIndex(i))
		{
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(WorldContextObject,
			                                                           LastGeneratedPoints[i], RandomLocation,
			                                                           UpdateTime * Radius * 0.1f); // 取一较近点
		}
		else
		{
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(WorldContextObject, Origin, RandomLocation,
			                                                           Radius);
		}
		
		FString ID = FString::FromInt(i);
		DebugPositioningPoints.Emplace(ID, RandomLocation);
		FPersonDataFragment Data;
		Data.ID = ID;
		Data.Name = "Person - " + ID;
		DebugPersonData.Emplace(ID, Data);
		GeneratedPoints.Emplace(RandomLocation);

		DrawPositioningDebug(WorldContextObject, RandomLocation, Data, UpdateTime);
	}
}

void UPositioningDebugLibrary::DrawPositioningDebug(UObject* WorldContextObject, FVector Location,
                                                    FPersonDataFragment PersonData, float Duration, FLinearColor Color)
{
	Location += FVector::UpVector * 50.0f;
	UKismetSystemLibrary::DrawDebugBox(WorldContextObject, Location, FVector(50.0f),
	                                   Color, FRotator::ZeroRotator, Duration);
	UKismetSystemLibrary::DrawDebugString(WorldContextObject, Location, PersonData.Name, nullptr,
	                                      Color, Duration);
}

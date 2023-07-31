// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PositioningSpawnDataGenerator.h"

#include "MassSpawnLocationProcessor.h"

UPositioningSpawnDataGenerator::UPositioningSpawnDataGenerator()
{
}

void UPositioningSpawnDataGenerator::Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes,
	int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
	TArray<FMassEntitySpawnDataGeneratorResult> Results;
	BuildResultsFromEntityTypes(Count, EntityTypes, Results);
	for (FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		Result.SpawnDataProcessor = UMassSpawnLocationProcessor::StaticClass();
		Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();
		FMassTransformsSpawnData& Transforms = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();
		Transforms.Transforms.Reserve(Result.NumEntities);
	}

	FinishedGeneratingSpawnPointsDelegate.Execute(Results); // 事件分发
}

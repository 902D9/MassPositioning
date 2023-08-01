// Fill out your copyright notice in the Description page of Project Settings.


#include "Positioning/PersonnelPositioningSubsystem.h"

#include "MassEntityConfigAsset.h"
#include "MassEntitySpawnDataGeneratorBase.h"
#include "MassSpawnerSubsystem.h"
#include "MassSpawnerTypes.h"
#include "AI/PersonnelPositioningInitializer.h"
#include "AI/PositioningSpawnDataGenerator.h"

void UPersonnelPositioningSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpawnerSubsystem = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
}

void UPersonnelPositioningSubsystem::Deinitialize()
{
	// ClearPerson(); // 不调用
	
	Super::Deinitialize();
}

void UPersonnelPositioningSubsystem::StartPersonnelPositioningSimulation(TMap<FString, FVector> InPositioningPoints,
                                                                         TMap<FString, FPersonDataFragment>
                                                                         InPersonData, UMassEntityConfigAsset* Config)
{
	PersonConfig = Config;
	UpdatePersonnelPositioningSimulation(InPositioningPoints, InPersonData);
}

void UPersonnelPositioningSubsystem::UpdatePersonnelPositioningSimulation(TMap<FString, FVector> InPositioningPoints,
                                                                          TMap<FString, FPersonDataFragment>
                                                                          InPersonData)
{
	// 找出 InPositioningPoints 相对 PositioningPoints 新增和删除的 key
	TArray<FString> RemovedPersonKeys;
	for (const auto& Elem : PositioningPoints)
	{
		// 若 PositioningPoints 中的 key 在 InPositioningPoints 中不存在，则记录该 key
		if (!InPositioningPoints.Contains(Elem.Key))
		{
			RemovedPersonKeys.Add(Elem.Key);
		}
		else
		{
			// 若 PositioningPoints 中的 key 在 InPositioningPoints 中存在，则更新 PositioningPoints 中的点位
			PositioningPoints[Elem.Key] = InPositioningPoints[Elem.Key];
			InPositioningPoints.Remove(Elem.Key); // 移除已更新的点位
		}
	}
	// 移除 PositioningPoints 中的 RemovedPersonKeys
	for (const auto& Key : RemovedPersonKeys)
	{
		PositioningPoints.Remove(Key);
		RemovePerson(Key); // 移除对应的人员 MASS
	}
	// 添加 InPositioningPoints 剩余（新增）到 PositioningPoints
	NewPersonWaitingForMassToSpawn = InPositioningPoints;
	for (const auto& Elem : InPositioningPoints)
	{
		PositioningPoints.Add(Elem.Key, Elem.Value);
	}
	
	PersonData = InPersonData;

	if (NewPersonWaitingForMassToSpawn.Num() > 0)
	{
		// 更新 MASS
		if (PersonConfig)
		{
			// 使用初始化 - 参考 MassSpawner
			FMassSpawnDataGenerator Generator;
			Generator.GeneratorInstance = NewObject<UPositioningSpawnDataGenerator>(); // 创建生成器
			// 创建 EntityTypes
			TArray<FMassSpawnedEntityType> EntityTypes;
			FMassSpawnedEntityType EntityType;
			EntityType.Proportion = 1.f;
			EntityType.EntityConfig = PersonConfig;
			EntityTypes.Emplace(EntityType);

			FFinishedGeneratingSpawnDataSignature Delegate = FFinishedGeneratingSpawnDataSignature::CreateUObject(
				this, &UPersonnelPositioningSubsystem::OnSpawnDataGenerationFinished, &Generator);
			Generator.GeneratorInstance->Generate(*this, EntityTypes, NewPersonWaitingForMassToSpawn.Num(), Delegate);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Config 未指定"));
		}
	}
}

bool UPersonnelPositioningSubsystem::RemovePerson(FString ID)
{
	// todo 从 MASS 中移除对应的人员
	return false;
}

void UPersonnelPositioningSubsystem::ClearPerson()
{
	if (PersonConfig)
	{
		SpawnerSubsystem->DestroyEntities( MassEntityHandles);
		MassEntityHandles.Empty();
		PositioningPoints.Empty();
		PersonData.Empty();
		NewPersonWaitingForMassToSpawn.Empty();
	}
}

void UPersonnelPositioningSubsystem::OnSpawnDataGenerationFinished(
	TConstArrayView<FMassEntitySpawnDataGeneratorResult> Results, FMassSpawnDataGenerator* FinishedGenerator)
{
	FinishedGenerator->bDataGenerated = true;
	for (const FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		if (Result.NumEntities <= 0)
		{
			continue;
		}
		const FMassEntityTemplate& EntityTemplate = PersonConfig->GetOrCreateEntityTemplate(*GetWorld());
		SpawnerSubsystem->SpawnEntities(EntityTemplate.GetTemplateID(), Result.NumEntities,
		                                Result.SpawnData, TSubclassOf<UPersonnelPositioningInitializer>(),
		                                MassEntityHandles); // 5.1
	}
	// NewPersonWaitingForMassToSpawn.Empty(); // 此处不清空，因为初始化未完成需使用
}

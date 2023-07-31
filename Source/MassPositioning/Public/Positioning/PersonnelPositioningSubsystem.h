// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AI/PersonnelPositioningTrait.h"
#include "PersonnelPositioningSubsystem.generated.h"

class UMassSpawnerSubsystem;
struct FMassEntityHandle;
struct FMassEntitySpawnDataGeneratorResult;
struct FMassSpawnDataGenerator;

/**
 * 
 */
UCLASS()
class MASSPOSITIONING_API UPersonnelPositioningSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * @brief 根据 Config 开始人员定位模拟
	 * @param InPositioningPoints 定位点 UE 坐标 （经纬度转UE / 类似打卡机等目标位置）
	 * @param InPersonData 人员基础信息，用于查找、显示
	 * @param Config 
	 */
	UFUNCTION(BlueprintCallable, Category="Personnel Positioning")
	void StartPersonnelPositioningSimulation(TMap<FString, FVector> InPositioningPoints,
	                                         TMap<FString, FPersonDataFragment> InPersonData,
	                                         UMassEntityConfigAsset* Config);
	/**
	 * @brief 更新人员定位模拟
	 * @param InPositioningPoints 定位点 UE 坐标 （经纬度转UE / 类似打卡机等目标位置）
	 * @param InPersonData 人员基础信息，用于查找、显示
	 */
	UFUNCTION(BlueprintCallable, Category="Personnel Positioning")
	void UpdatePersonnelPositioningSimulation(TMap<FString, FVector> InPositioningPoints,
	                                          TMap<FString, FPersonDataFragment> InPersonData);

	/**
	 * @brief 从 MASS 中移除对应的人员
	 * @param ID 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Personnel Positioning")
	bool RemovePerson(FString ID);

	/**
	 * @brief 清除
	 */
	UFUNCTION(BlueprintCallable, Category="Personnel Positioning")
	void ClearPerson();

protected:
	UPROPERTY()
	UMassSpawnerSubsystem* SpawnerSubsystem;
	UPROPERTY()
	UMassEntityConfigAsset* PersonConfig;

	TMap<FString, FVector> PositioningPoints;
	// 等待 Mass 创建的人员位置
	TMap<FString, FVector> NewPersonWaitingForMassToSpawn;
	TMap<FString, FPersonDataFragment> PersonData;

	UPROPERTY()
	TArray<FMassEntityHandle> MassEntityHandles;

	void OnSpawnDataGenerationFinished(TConstArrayView<FMassEntitySpawnDataGeneratorResult> Results,
	                                   FMassSpawnDataGenerator* FinishedGenerator);

public:
	TMap<FString, FVector> GetNewPersonWaitingForMassToSpawn() { return NewPersonWaitingForMassToSpawn; }
	TMap<FString, FPersonDataFragment> GetPersonData() { return PersonData; }
};

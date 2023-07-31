// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PersonnelPositioningInitializer.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "AI/PersonnelPositioningTrait.h"
#include "Positioning/PersonnelPositioningSubsystem.h"

UPersonnelPositioningInitializer::UPersonnelPositioningInitializer()
{
	ObservedType = FPersonDataFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UPersonnelPositioningInitializer::ConfigureQueries()
{
	EntityQuery.AddRequirement<FPersonDataFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.RegisterWithProcessor(*this); //5.1
}

void UPersonnelPositioningInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	PersonnelPositioningSubsystem = GetWorld()->GetSubsystem<UPersonnelPositioningSubsystem>();
	if (!PersonnelPositioningSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("!PersonnelPositioningSubsystem"));
		return;
	}

	// 获取等待 Mass 创建的人员位置
	TMap<FString, FVector> PersonWaitingForMassToSpawn =
		PersonnelPositioningSubsystem->GetNewPersonWaitingForMassToSpawn();
	// 获取 GPSPoints KEYS, 即人员ID
	TArray<FString> PersonIDs;
	PersonWaitingForMassToSpawn.GetKeys(PersonIDs);

	TMap<FString, FPersonDataFragment> PersonData = PersonnelPositioningSubsystem->GetPersonData();

	int32 PersonIndex = 0;
	EntityQuery.ForEachEntityChunk(
		EntityManager, Context,
		([this, PersonWaitingForMassToSpawn, &PersonIndex, PersonIDs, PersonData](FMassExecutionContext& Context)
		{
			const TArrayView<FTransformFragment> TransformFragments =
				Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FPersonDataFragment> PersonDataFragments =
				Context.GetMutableFragmentView<FPersonDataFragment>();

			int32 count = 0;
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				// 初始化 ID + Name
				FString& MutablePersonDataID = PersonDataFragments[EntityIndex].ID;
				MutablePersonDataID = PersonIDs[PersonIndex];
				FString& MutablePersonDataName = PersonDataFragments[EntityIndex].Name;
				MutablePersonDataName = PersonData.Find(PersonIDs[PersonIndex])->Name;

				// 初始化位置
				FTransform& MutableTransform = TransformFragments[EntityIndex].GetMutableTransform();
				MutableTransform.SetLocation(*PersonWaitingForMassToSpawn.Find(MutablePersonDataID));

				++count;
				++PersonIndex; // 移动人员轨迹索引, 因为一次不会全部初始化
			}
			UE_LOG(LogTemp, Log,
			       TEXT("PersonGPSInitializerProcessor: 共初始化 %d PersonGPSFragment，当前处理到 %d 个人员信息"),
			       count, PersonIndex);
		}));
}

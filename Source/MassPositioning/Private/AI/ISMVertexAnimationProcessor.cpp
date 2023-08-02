// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ISMVertexAnimationProcessor.h"

#include "AnimToTextureInstancePlaybackHelpers.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassLookAtFragments.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassRepresentationFragments.h"
#include "MassRepresentationSubsystem.h"

UISMVertexAnimationProcessor::UISMVertexAnimationProcessor()
{
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
}

void UISMVertexAnimationProcessor::UpdateISMVertexAnimation(FMassInstancedStaticMeshInfo& ISMInfo,
                                                            FCrowdAnimationFragment& AnimationData,
                                                            const float LODSignificance,
                                                            const float PrevLODSignificance,
                                                            const int32 NumFloatsToPad)
{
	FAnimToTextureAutoPlayData AutoPlayData;
	UAnimToTextureInstancePlaybackLibrary::GetAutoPlayDataFromDataAsset(AnimationData.AnimToTextureData.Get(),
	                                                                    AnimationData.AnimationStateIndex,
	                                                                    AutoPlayData);
	ISMInfo.AddBatchedCustomData<FAnimToTextureAutoPlayData>(AutoPlayData, LODSignificance, PrevLODSignificance,
	                                                         NumFloatsToPad);
}


void UISMVertexAnimationProcessor::ConfigureQueries()
{
	Super::ConfigureQueries();

	EntityQuery.AddRequirement<FCrowdAnimationFragment>(EMassFragmentAccess::ReadWrite);
}

void UISMVertexAnimationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [](FMassExecutionContext& Context)
	{
		// 获取静态网格体实例
		UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetSharedFragment<
			FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;
		check(RepresentationSubsystem);
		FMassInstancedStaticMeshInfoArrayView ISMInfo = RepresentationSubsystem->GetMutableInstancedStaticMeshInfos();

		TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
		TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<
			FMassRepresentationFragment>();
		TConstArrayView<FMassRepresentationLODFragment> RepresentationLODList = Context.GetFragmentView<
			FMassRepresentationLODFragment>();
		TArrayView<FCrowdAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<
			FCrowdAnimationFragment>();

		const int32 NumEntities = Context.GetNumEntities();
		for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			const FTransformFragment& TransformFragment = TransformList[EntityIdx];
			const FMassRepresentationLODFragment& RepresentationLOD = RepresentationLODList[EntityIdx];
			FMassRepresentationFragment& Representation = RepresentationList[EntityIdx];
			FCrowdAnimationFragment& AnimationData = AnimationDataList[EntityIdx];

			if (Representation.CurrentRepresentation == EMassRepresentationType::StaticMeshInstance)
			{
				UpdateISMTransform(GetTypeHash(Context.GetEntity(EntityIdx)),
				                   ISMInfo[Representation.StaticMeshDescIndex], TransformFragment.GetTransform(),
				                   Representation.PrevTransform, RepresentationLOD.LODSignificance,
				                   Representation.PrevLODSignificance);
				UpdateISMVertexAnimation(ISMInfo[Representation.StaticMeshDescIndex], AnimationData,
				                         RepresentationLOD.LODSignificance, Representation.PrevLODSignificance);
			}
			Representation.PrevTransform = TransformFragment.GetTransform();
			Representation.PrevLODSignificance = RepresentationLOD.LODSignificance;
		}
	});
}

UMassProcessor_Animation::UMassProcessor_Animation()
	: AnimationEntityQuery_Conditional(*this)
	  , MontageEntityQuery(*this)
	  , MontageEntityQuery_Conditional(*this)
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Tasks;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::SyncWorldToMass);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Representation);

	bRequiresGameThreadExecution = true;
}


void UMassProcessor_Animation::ConfigureQueries()
{
	AnimationEntityQuery_Conditional.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	AnimationEntityQuery_Conditional.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	AnimationEntityQuery_Conditional.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadOnly);
	AnimationEntityQuery_Conditional.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
	AnimationEntityQuery_Conditional.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	AnimationEntityQuery_Conditional.AddRequirement<FMassLookAtFragment>(
		EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	AnimationEntityQuery_Conditional.AddRequirement<FMassMoveTargetFragment>(
		EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	AnimationEntityQuery_Conditional.AddRequirement<FCrowdAnimationFragment>(EMassFragmentAccess::ReadWrite);
	// AnimationEntityQuery_Conditional.AddRequirement<FMassMontageFragment>(
	// 	EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
	AnimationEntityQuery_Conditional.AddChunkRequirement<
		FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	AnimationEntityQuery_Conditional.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
	AnimationEntityQuery_Conditional.RequireMutatingWorldAccess();

	// MontageEntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	// MontageEntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	// MontageEntityQuery.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadOnly);
	// MontageEntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
	// MontageEntityQuery.AddRequirement<FCrowdAnimationFragment>(EMassFragmentAccess::ReadOnly);
	// MontageEntityQuery.AddRequirement<FMassMontageFragment>(EMassFragmentAccess::ReadWrite);
	// MontageEntityQuery.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	// MontageEntityQuery.RequireMutatingWorldAccess();
	//
	// MontageEntityQuery_Conditional = MontageEntityQuery;
	// MontageEntityQuery_Conditional.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
	// MontageEntityQuery_Conditional.RequireMutatingWorldAccess();
}

void UMassProcessor_Animation::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	World = Owner.GetWorld();
	check(World);
}

void UMassProcessor_Animation::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	check(World);

	QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_Run);

	const float GlobalTime = World->GetTimeSeconds();

	TArray<FMassEntityHandle, TInlineAllocator<32>> ActorEntities;

	{
		QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_UpdateAnimationFragmentData);
		AnimationEntityQuery_Conditional.ForEachEntityChunk(EntityManager, Context,
		                                                    [this, GlobalTime, &ActorEntities, &EntityManager](
		                                                    FMassExecutionContext& Context)
		                                                    {
			                                                    UMassProcessor_Animation::UpdateAnimationFragmentData(
				                                                    EntityManager, Context, GlobalTime,
				                                                    ActorEntities);
		                                                    });
	}
	{
		QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_UpdateVertexAnimationState);
		AnimationEntityQuery_Conditional.ForEachEntityChunk(EntityManager, Context,
		                                                    [this, GlobalTime, &EntityManager](
		                                                    FMassExecutionContext& Context)
		                                                    {
			                                                    UMassProcessor_Animation::UpdateVertexAnimationState(
				                                                    EntityManager, Context, GlobalTime);
		                                                    });
	}
}

void UMassProcessor_Animation::UpdateAnimationFragmentData(FMassEntityManager& EntityManager,
                                                           FMassExecutionContext& Context, float GlobalTime,
                                                           TArray<FMassEntityHandle, TInlineAllocator<32>>&
                                                           ActorEntities)
{
	TArrayView<FCrowdAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimationFragment>();
	// TConstArrayView<FMassMontageFragment> MontageDataList = Context.GetFragmentView<FMassMontageFragment>();
	TConstArrayView<FMassRepresentationFragment> VisualizationList = Context.GetFragmentView<
		FMassRepresentationFragment>();
	TConstArrayView<FMassActorFragment> ActorInfoList = Context.GetFragmentView<FMassActorFragment>();

	const int32 NumEntities = Context.GetNumEntities();
	for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
	{
		FCrowdAnimationFragment& AnimationData = AnimationDataList[EntityIdx];
		const FMassRepresentationFragment& Visualization = VisualizationList[EntityIdx];
		const FMassActorFragment& ActorFragment = ActorInfoList[EntityIdx];

		if (!ActorFragment.IsOwnedByMass())
		{
			continue;
		}

		const bool bWasActor = (Visualization.PrevRepresentation == EMassRepresentationType::HighResSpawnedActor) || (
			Visualization.PrevRepresentation == EMassRepresentationType::LowResSpawnedActor);
		const bool bIsActor = (Visualization.CurrentRepresentation == EMassRepresentationType::HighResSpawnedActor) || (
			Visualization.CurrentRepresentation == EMassRepresentationType::LowResSpawnedActor);
		AnimationData.bSwappedThisFrame = (bWasActor != bIsActor);

		// if (!MontageDataList.IsEmpty() && MontageDataList[EntityIdx].MontageInstance.SequenceChangedThisFrame())
		// {
		// 	AnimationData.GlobalStartTime = GlobalTime - MontageDataList[EntityIdx].MontageInstance.
		// 		GetPositionInSection();
		// }

		switch (Visualization.CurrentRepresentation)
		{
		case EMassRepresentationType::LowResSpawnedActor:
		case EMassRepresentationType::HighResSpawnedActor:
			{
				FMassEntityHandle Entity = Context.GetEntity(EntityIdx);
				ActorEntities.Add(Entity);
				break;
			}
		default:
			break;
		}
	}
}

void UMassProcessor_Animation::UpdateVertexAnimationState(FMassEntityManager& EntityManager,
                                                          FMassExecutionContext& Context, float GlobalTime)
{
	const int32 NumEntities = Context.GetNumEntities();
	TArrayView<FCrowdAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimationFragment>();
	// TConstArrayView<FMassMontageFragment> MontageDataList = Context.GetFragmentView<FMassMontageFragment>();
	TConstArrayView<FMassRepresentationFragment> VisualizationList = Context.GetFragmentView<
		FMassRepresentationFragment>();
	TConstArrayView<FMassVelocityFragment> VelocityList = Context.GetFragmentView<FMassVelocityFragment>();

	for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
	{
		FCrowdAnimationFragment& AnimationData = AnimationDataList[EntityIdx];

		const FMassRepresentationFragment& Visualization = VisualizationList[EntityIdx];
		const FMassVelocityFragment& Velocity = VelocityList[EntityIdx];

		// Need current anim state to update for skeletal meshes to do a smooth blend between poses
		if (Visualization.CurrentRepresentation != EMassRepresentationType::None)
		{
			int32 StateIndex = 0;

			// @todo: Make a better way to map desired anim states here. Currently the anim texture index to access is hard-coded.
			const float VelocitySizeSq = Velocity.Value.SizeSquared();
			bool bIsWalking = Velocity.Value.SizeSquared() > MoveThresholdSq;
			bIsWalking = true; // test
			if (bIsWalking)
			{
				StateIndex = 1;
				const float AuthoredAnimSpeed = 140.0f;
				const float PrevPlayRate = AnimationData.PlayRate;
				AnimationData.PlayRate = FMath::Clamp(
					FMath::Sqrt(VelocitySizeSq / (AuthoredAnimSpeed * AuthoredAnimSpeed)), 0.8f, 2.0f);

				// Need to conserve current frame on a playrate switch so (GlobalTime - Offset1) * Playrate1 == (GlobalTime - Offset2) * Playrate2
				AnimationData.GlobalStartTime = GlobalTime - PrevPlayRate * (GlobalTime - AnimationData.GlobalStartTime)
					/ AnimationData.PlayRate;
			}
			else
			{
				AnimationData.PlayRate = 1.0f;
				StateIndex = 0;
			}

			AnimationData.AnimationStateIndex = StateIndex;
		}
	}
}

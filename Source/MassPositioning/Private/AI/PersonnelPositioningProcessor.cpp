// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PersonnelPositioningProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "AI/PersonnelPositioningTrait.h"
#include "Positioning/PersonnelPositioningSubsystem.h"

UPersonnelPositioningProcessor::UPersonnelPositioningProcessor()
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UPersonnelPositioningProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FPersonDataFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	EntityQuery.RegisterWithProcessor(*this); // 5.1
}

void UPersonnelPositioningProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	UPersonnelPositioningSubsystem* PersonnelPositioningSubsystem = GetWorld()->GetSubsystem<
		UPersonnelPositioningSubsystem>();
	if (!PersonnelPositioningSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("!PersonnelPositioningSubsystem"));
		return;
	}
	TMap<FString, FVector> GPSPoints = PersonnelPositioningSubsystem->GetPositioningPoints();

	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, GPSPoints](FMassExecutionContext& Context)
	{
		const TConstArrayView<FPersonDataFragment> PersonDataFragments = Context.GetFragmentView<FPersonDataFragment>();
		const TArrayView<FTransformFragment> TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FMassMoveTargetFragment> MassMoveTargetFragments =
			Context.GetMutableFragmentView<FMassMoveTargetFragment>();
		const FMassMovementParameters& MovementParameters = Context.GetConstSharedFragment<FMassMovementParameters>();
		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			const FTransform& Transform = TransformFragments[EntityIndex].GetTransform();
			FMassMoveTargetFragment& MoveTarget = MassMoveTargetFragments[EntityIndex];

			FVector CurLocation = Transform.GetLocation();
			FVector TargetVector = MoveTarget.Center - CurLocation;
			TargetVector.Z = 0.f;
			MoveTarget.DistanceToGoal = TargetVector.Size();
			MoveTarget.Forward = TargetVector.GetSafeNormal();

			// 根据 ID 获取目标位置
			FString PersonID = PersonDataFragments[EntityIndex].ID;
			FVector TargetGPSLocation = *GPSPoints.Find(PersonID);
			if (MoveTarget.Center != TargetGPSLocation)
			{
				MoveTarget.Center = TargetGPSLocation;
				MoveTarget.DistanceToGoal = (MoveTarget.Center - Transform.GetLocation()).Size();
				MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
				MoveTarget.DesiredSpeed = FMassInt16Real(MovementParameters.DefaultDesiredSpeed);
				MoveTarget.IntentAtGoal = EMassMovementAction::Stand;
			}
			if (MoveTarget.DistanceToGoal <= 150.f)
			{
				// 停止
				MoveTarget.Forward = FVector::ZeroVector;
				MoveTarget.DesiredSpeed = FMassInt16Real(0.f);
			}
		}
	});
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassUpdateISMProcessor.h"
#include "ISMVertexAnimationProcessor.generated.h"

class UAnimToTextureDataAsset;
struct FMassInstancedStaticMeshInfo;

USTRUCT()
struct MASSPOSITIONING_API FCrowdAnimationFragment : public FMassFragment
{
	GENERATED_BODY()

	TWeakObjectPtr<UAnimToTextureDataAsset> AnimToTextureData;
	float GlobalStartTime = 0.0f;
	float PlayRate = 1.0f;
	int32 AnimationStateIndex = 0;
	bool bSwappedThisFrame = false;
};

/**
 * 
 */
UCLASS()
class MASSPOSITIONING_API UISMVertexAnimationProcessor : public UMassUpdateISMProcessor
{
	GENERATED_BODY()

public:
	UISMVertexAnimationProcessor();

	static void UpdateISMVertexAnimation(FMassInstancedStaticMeshInfo& ISMInfo, FCrowdAnimationFragment& AnimationData,
	                                     const float LODSignificance, const float PrevLODSignificance,
	                                     const int32 NumFloatsToPad = 0);

protected:
	/** Configure the owned FMassEntityQuery instances to express processor's requirements */
	virtual void ConfigureQueries() override;

	/**
	 * Execution method for this processor
	 * @param EntityManager is the system to execute the lambdas on each entity chunk
	 * @param Context is the execution context to be passed when executing the lambdas */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
};

UCLASS()
class MASSPOSITIONING_API UMassProcessor_Animation : public UMassProcessor
{
	GENERATED_BODY()

public:
	UMassProcessor_Animation();

	UPROPERTY(EditAnywhere, Category="Animation", meta=(ClampMin=0.0, UIMin=0.0))
	float MoveThresholdSq = 750.0f;

protected:
	/** Configure the owned FMassEntityQuery instances to express processor's requirements */
	virtual void ConfigureQueries() override;
	virtual void Initialize(UObject& Owner) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	UPROPERTY(Transient)
	UWorld* World = nullptr;

	FMassEntityQuery AnimationEntityQuery_Conditional;
	FMassEntityQuery MontageEntityQuery;
	FMassEntityQuery MontageEntityQuery_Conditional;

private:
	void UpdateAnimationFragmentData(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
	                                 float GlobalTime, TArray<FMassEntityHandle, TInlineAllocator<32>>& ActorEntities);
	void UpdateVertexAnimationState(FMassEntityManager& EntityManager, FMassExecutionContext& Context,
	                                float GlobalTime);
	// void UpdateSkeletalAnimation(FMassEntityManager& EntityManager, float GlobalTime, TArrayView<FMassEntityHandle> ActorEntities);
};

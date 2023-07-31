// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassObserverProcessor.h"
#include "PersonnelPositioningInitializer.generated.h"

class UPersonnelPositioningSubsystem;

/**
 * 
 */
UCLASS()
class MASSPOSITIONING_API UPersonnelPositioningInitializer : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UPersonnelPositioningInitializer();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	UPersonnelPositioningSubsystem* PersonnelPositioningSubsystem;
};

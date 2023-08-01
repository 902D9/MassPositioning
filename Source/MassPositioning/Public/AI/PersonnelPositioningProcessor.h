// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "PersonnelPositioningProcessor.generated.h"

/**
 * 
 */
UCLASS()
class MASSPOSITIONING_API UPersonnelPositioningProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UPersonnelPositioningProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};

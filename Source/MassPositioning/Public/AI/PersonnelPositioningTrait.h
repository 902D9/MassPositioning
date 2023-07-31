// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "PersonnelPositioningTrait.generated.h"

/**
 * 人员基础信息
 */
USTRUCT(BlueprintType)
struct FPersonDataFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Name;

	// ...
};

/**
 * 人员定位并向目标移动
 */
UCLASS()
class MASSPOSITIONING_API UPersonnelPositioningTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

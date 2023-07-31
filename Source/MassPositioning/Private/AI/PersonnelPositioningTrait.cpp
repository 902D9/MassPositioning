// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PersonnelPositioningTrait.h"
#include "MassEntityTemplateRegistry.h"

void UPersonnelPositioningTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FPersonDataFragment>();
}

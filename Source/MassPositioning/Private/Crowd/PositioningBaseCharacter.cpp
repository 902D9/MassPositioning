// Fill out your copyright notice in the Description page of Project Settings.


#include "Crowd/PositioningBaseCharacter.h"
#include "AnimToTextureDataAsset.h"

// Sets default values
APositioningBaseCharacter::APositioningBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APositioningBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APositioningBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APositioningBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


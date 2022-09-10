// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyCharacter.h"

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

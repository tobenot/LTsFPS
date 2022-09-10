// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "MyObPawn.generated.h"

/**
 * 
 */
UCLASS()
class LTSFPS_API AMyObPawn : public ADefaultPawn
{
	GENERATED_BODY()
	
public:
	AMyObPawn();

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TSubclassOf<class AMyCharacter> CharacterClass;

	UFUNCTION(Server,Unreliable)
	void SendLocationToServer(FVector Location);

	UFUNCTION()
	void RespawnPlayer();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

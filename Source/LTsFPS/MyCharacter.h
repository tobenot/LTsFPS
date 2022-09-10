// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LTsFPSCharacter.h"
#include "MyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LTSFPS_API AMyCharacter : public ALTsFPSCharacter
{
	GENERATED_BODY()
public:

	AMyCharacter();

	/** Gun mesh: 3st person view (seen only by other) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
		TSubclassOf<class AMyObPawn> ObPawnClass;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
		float Health;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
		float HealthMax;
	UPROPERTY(ReplicatedUsing = "OnRep_bIsDead", VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		bool bIsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Action)
		bool bCrouchButtonDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Action)
		bool bJumpButtonDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TPFireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* DeadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TakeDamageAnimation;
public:

	UFUNCTION()
		void OnFire();
	UFUNCTION(Server, Reliable)
		void ServerOnFire();
	UFUNCTION(NetMulticast, Unreliable)
		void NetFireEffect();
	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION(NetMulticast, Unreliable)
		void NetTakeDamageEffect();
	UFUNCTION()
		void CheckDead();
// 	UFUNCTION()
// 		void OnDestroyedCheckDead(AActor* DesActor);
	UFUNCTION()
		void OnRep_bIsDead();
	UFUNCTION()
		void ActionCrouch();
	UFUNCTION()
		void ActionStopCrouch();
	UFUNCTION()
		void ActionJump();
	UFUNCTION()
		void ActionStopJump();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};

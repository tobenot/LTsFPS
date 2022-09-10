// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LTsFPSProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "MyObPawn.h"

AMyCharacter::AMyCharacter()
{
	GetMesh()->SetOwnerNoSee(true);
	// Create a gun mesh component
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->bCastDynamicShadow = true;
	TP_Gun->CastShadow = true;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	TP_Gun->SetupAttachment(RootComponent);

	HealthMax = 100.0f;
	Health = HealthMax;

}
void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter,Health);
	DOREPLIFETIME(AMyCharacter,HealthMax);
	DOREPLIFETIME(AMyCharacter,bIsDead);
}
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AMyCharacter::HandleTakeAnyDamage);
	}
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	TP_Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	//OnDestroyed.AddDynamic(this, &AMyCharacter::OnDestroyedCheckDead);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (GetActorLocation().Z < -100.f)
		{
			CheckDead();
		}
	}
	
}

void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::OnFire);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::ActionJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::ActionStopJump);

	// Bind crouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyCharacter::ActionCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyCharacter::ActionStopCrouch);
}
void AMyCharacter::OnFire()
{
	if (bIsDead)return;
	if (!HasAuthority())
	{
		ServerOnFire();
		return;
	}
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{

			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((GetMuzzleLocation() != nullptr) ? GetMuzzleLocation()->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = this;
			// spawn the projectile at the muzzle
			World->SpawnActor<ALTsFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			NetFireEffect();
		}
	}

}

void AMyCharacter::ServerOnFire_Implementation()
{
	OnFire();
}


void AMyCharacter::NetFireEffect_Implementation()
{
	//表现开火特效
	if (GetGameInstance()->IsDedicatedServerInstance())
	{
		return;
	}
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (IsLocallyControlled())
	{
		//是自己控制的就播放第一人称射击动画
		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
	else
	{
		if (TPFireAnimation)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(TPFireAnimation, 1.f);
			}
		}
	}
	
}

void AMyCharacter::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s:MyHealthIs: % f"), HasAuthority() ? UTF8_TO_TCHAR("Server") : UTF8_TO_TCHAR("Client"), Health);
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	

	Health = FMath::Clamp(Health - Damage, 0.0f, HealthMax);
	if (Health <= 0)
	{
		CheckDead();
	}
	else
	{
		NetTakeDamageEffect();
	}
	return;
}

void AMyCharacter::NetTakeDamageEffect_Implementation()
{
	if (!IsLocallyControlled())
	{
		if (TakeDamageAnimation)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(TakeDamageAnimation, 1.f);
			}
		}
	}
}

void AMyCharacter::CheckDead()
{
	if (!bIsDead)
	{
		bIsDead = 1;
		OnRep_bIsDead();


		//切换死亡的Pawn
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (ObPawnClass)
			{
				const FRotator SpawnRotation = GetActorRotation();

				const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100.f) - 250.f * GetActorForwardVector();

				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ActorSpawnParams.Owner = GetController();

				// spawn the actor for ob
				AMyObPawn* ObPawn = World->SpawnActor<AMyObPawn>(ObPawnClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				if (ObPawn)
				{
					GetController()->Possess(ObPawn);
				}
			}


			//destroy after 10 seconds
			SetLifeSpan(10.f);

		}

	}



}

// void AMyCharacter::OnDestroyedCheckDead(AActor* DesActor)
// {
// 	CheckDead();
// }

void AMyCharacter::OnRep_bIsDead()
{

	if (DeadAnimation)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DeadAnimation, 1.f);
		}
	}
	//死亡效果

	//碰撞关闭一些
	UCapsuleComponent* MyCapsuleComponent = Cast<UCapsuleComponent>(GetRootComponent());
	if (MyCapsuleComponent)
	{
		MyCapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		MyCapsuleComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	}

}

void AMyCharacter::ActionCrouch()
{
	bCrouchButtonDown = 1;
}

void AMyCharacter::ActionStopCrouch()
{
	bCrouchButtonDown = 0;
}

void AMyCharacter::ActionJump()
{
	Jump();
	bJumpButtonDown = CanJump();

}

void AMyCharacter::ActionStopJump()
{
	StopJumping();
	bJumpButtonDown = 0;
}

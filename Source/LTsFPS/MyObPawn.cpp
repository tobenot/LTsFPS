// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObPawn.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyCharacter.h"

AMyObPawn::AMyObPawn()
{
	bReplicates = 1;
	GetMovementComponent()->SetIsReplicated(true);

	

// 	GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 	GetCollisionComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
// 	GetCollisionComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void AMyObPawn::SendLocationToServer_Implementation(FVector Location)
{
	SetActorLocation(Location);
}

void AMyObPawn::RespawnPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("%s:SRespawnPlayer"), HasAuthority() ? UTF8_TO_TCHAR("Server") : UTF8_TO_TCHAR("Client"));
	if (CharacterClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{

			const FRotator SpawnRotation = FRotator(0,GetControlRotation().Yaw,0);
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetActorLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			ActorSpawnParams.Owner = GetController();
			// spawn the projectile at the muzzle
			APawn* MyCharacter = Cast<APawn>(World->SpawnActor<AMyCharacter>(CharacterClass, SpawnLocation, SpawnRotation, ActorSpawnParams));

			if (MyCharacter)
			{
				GetController()->Possess(MyCharacter);
			}
			Destroy();
		}
	}
}

void AMyObPawn::BeginPlay()
{
	Super::BeginPlay();

	//碰撞修改
	SetActorEnableCollision(false);

	//复活时钟
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s:SetTimerForResapanwe"), HasAuthority() ? UTF8_TO_TCHAR("Server") : UTF8_TO_TCHAR("Client"));
		float RespawnTime = 6.f;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyObPawn::RespawnPlayer, RespawnTime, false);
	}
	
}

void AMyObPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//每Tick向服务器传自己坐标
	if (IsLocallyControlled())
	{
		if (!HasAuthority())
		{
			SendLocationToServer(GetActorLocation());
		}
	}
}

void AMyObPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
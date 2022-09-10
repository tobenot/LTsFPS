// Copyright Epic Games, Inc. All Rights Reserved.

#include "LTsFPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ALTsFPSProjectile::ALTsFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALTsFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bReplicates = 1;
	ProjectileMovement->SetIsReplicated(true);

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	MyDamage = 20.0f;
}

void ALTsFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();
	}
	if (HasAuthority())
	{
		if (Cast<APawn>(OtherActor))
		{
			if (GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s:TryApplyDamage"), HasAuthority() ? UTF8_TO_TCHAR("Server") : UTF8_TO_TCHAR("Client"));
				UGameplayStatics::ApplyDamage(OtherActor, MyDamage, GetOwner()->GetInstigatorController(), this, DamageType);	// ©º”…À∫¶
			}
			Destroy();
		}
	}
	
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

void AMyPlayerController::UpdatePhyActor()
{
	//刷新一下物理体
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), FoundActors);
	for ( AActor* Actor : FoundActors)
	{
		AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Actor);
		if (MeshActor && MeshActor->GetStaticMeshComponent() && MeshActor->GetStaticMeshComponent()->IsSimulatingPhysics())
		{
			MeshActor->GetStaticMeshComponent()->AddImpulseAtLocation(FVector(0,0,-10.f), MeshActor->GetStaticMeshComponent()->GetComponentLocation());
		}
	}

}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	const FInputModeGameOnly a;
	SetInputMode(a);

	//刷新一下物理体
	if (HasAuthority())
	{
		UpdatePhyActor();
	}
	
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

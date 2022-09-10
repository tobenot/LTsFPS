// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LTsFPSHUD.generated.h"

UCLASS()
class ALTsFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALTsFPSHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};


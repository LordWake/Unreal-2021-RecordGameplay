// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RecordGameplayHUD.generated.h"

UCLASS()
class ARecordGameplayHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARecordGameplayHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};


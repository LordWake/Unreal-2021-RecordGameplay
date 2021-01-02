//+---------------------------------------------------------+
//| Project   : RecordGameplay UE 4.25						|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ReplayPC.generated.h"

UCLASS()
class RECORDGAMEPLAY_API AReplayPC : public APlayerController
{
	GENERATED_BODY()
	
protected:

	void RestartRecording();
	virtual void SetupInputComponent() override;
};

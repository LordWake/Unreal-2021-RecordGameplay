//+---------------------------------------------------------+
//| Project   : RecordGameplay UE 4.25						|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RecordGameInstance.generated.h"

UCLASS()
class RECORDGAMEPLAY_API URecordGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	URecordGameInstance();

	UPROPERTY(EditDefaultsOnly, Category = "Replays")
	FString RecordingName;

	UPROPERTY(EditDefaultsOnly, Category = "Replays")
	FString FriendlyRecordingName;

	void StartRecording();
	void StopRecording();
	void StartReplay();
};

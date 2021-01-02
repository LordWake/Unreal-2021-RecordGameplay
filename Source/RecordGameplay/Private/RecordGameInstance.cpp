//+---------------------------------------------------------+
//| Project   : RecordGameplay UE 4.25						|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#include "RecordGameInstance.h"

URecordGameInstance::URecordGameInstance()
{
	RecordingName = "MyReplay";
	FriendlyRecordingName = "My Replay";
}

void URecordGameInstance::StartRecording()
{
	StartRecordingReplay(RecordingName, FriendlyRecordingName);
}

void URecordGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void URecordGameInstance::StartReplay()
{
	PlayReplay(RecordingName, nullptr);
}

//+---------------------------------------------------------+
//| Project   : RecordGameplay UE 4.25						|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+


#include "ReplayPC.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerInput.h"

void AReplayPC::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RestartRecord", EKeys::Four));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RestartRecord", EKeys::NumPadFour));

	InputComponent->BindAction("RestartRecord", IE_Pressed, this, &AReplayPC::RestartRecording);
}

void AReplayPC::RestartRecording()
{
	if (UWorld* World = GetWorld())
	{
		if (UDemoNetDriver* DemoDriver = World->DemoNetDriver)
			DemoDriver->GotoTimeInSeconds(0.0f);
	}
}
//+---------------------------------------------------------+
//| Project   : RecordGameplay UE 4.25						|
//| Author    : github.com/LordWake					 		|
//+---------------------------------------------------------+

#include "RecordGameplayCharacter.h"
#include "RecordGameplayProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RecordGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ARecordGameplayCharacter::ARecordGameplayCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
}

void ARecordGameplayCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	RecordGameInstance = Cast<URecordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

void ARecordGameplayCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StartRecording", EKeys::One));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StartRecording", EKeys::NumPadOne));
	
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StopRecording", EKeys::Two));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StopRecording", EKeys::NumPadTwo));
	
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StartReplay", EKeys::Three));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("StartReplay", EKeys::NumPadThree));

	PlayerInputComponent->BindAction("StartRecording",	IE_Pressed, this, &ARecordGameplayCharacter::StartRecord);
	PlayerInputComponent->BindAction("StopRecording",	IE_Pressed, this, &ARecordGameplayCharacter::StopRecord);
	PlayerInputComponent->BindAction("StartReplay",		IE_Pressed, this, &ARecordGameplayCharacter::StartReplay);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ARecordGameplayCharacter::OnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARecordGameplayCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARecordGameplayCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARecordGameplayCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARecordGameplayCharacter::LookUpAtRate);
}

void ARecordGameplayCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ARecordGameplayProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ARecordGameplayCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ARecordGameplayCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ARecordGameplayCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARecordGameplayCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARecordGameplayCharacter::StartRecord()
{
	if(RecordGameInstance)
	RecordGameInstance->StartRecording();
}

void ARecordGameplayCharacter::StopRecord()
{
	if (RecordGameInstance)
		RecordGameInstance->StopRecording();
}

void ARecordGameplayCharacter::StartReplay()
{
	if (RecordGameInstance)
		RecordGameInstance->StartReplay();
}

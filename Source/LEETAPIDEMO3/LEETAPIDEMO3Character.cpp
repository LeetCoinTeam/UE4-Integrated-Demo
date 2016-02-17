// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LEETAPIDEMO3.h"
#include "UnrealNetwork.h"
#include "MyBomb.h"
#include "MyPlayerController.h"
#include "MyGameInstance.h"
#include "LEETAPIDEMO3Character.h"

//////////////////////////////////////////////////////////////////////////
// ALEETAPIDEMO3Character

ALEETAPIDEMO3Character::ALEETAPIDEMO3Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	Health = 1.0f;
	//HealthMax = 100.0f;
	BombCount = 3;
	//BombsMax = 3;

	//bReplicates = true;
	//bReplicateMovement = true;


}

//////////////////////////////////////////////////////////////////////////
// Input

void ALEETAPIDEMO3Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ALEETAPIDEMO3Character::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ALEETAPIDEMO3Character::OnStopFire);

	InputComponent->BindAxis("MoveForward", this, &ALEETAPIDEMO3Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALEETAPIDEMO3Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ALEETAPIDEMO3Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ALEETAPIDEMO3Character::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &ALEETAPIDEMO3Character::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &ALEETAPIDEMO3Character::TouchStopped);
}


void ALEETAPIDEMO3Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ALEETAPIDEMO3Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ALEETAPIDEMO3Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALEETAPIDEMO3Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALEETAPIDEMO3Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ALEETAPIDEMO3Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ALEETAPIDEMO3Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ALEETAPIDEMO3Character, Health);
	DOREPLIFETIME(ALEETAPIDEMO3Character, BombCount);
}

void ALEETAPIDEMO3Character::OnStartFire()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [OnStartFire] "));
	if (CanFire()) {
		FVector ShootDir = GetActorForwardVector();
		FVector Origin = GetActorLocation();
		//  Tell the server to Spawn a bomb
		ServerAttemptSpawnBomb();
	}
	
}

void ALEETAPIDEMO3Character::OnStopFire()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [OnStopFire] "));
	//StopWeaponFire();
}

float ALEETAPIDEMO3Character::TakeDamageCustom(float Damage, struct FDamageEvent const& DamageEvent,  class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [TakeDamageCustom] "));
	if (Role >= ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [TakeDamageCustom] SERVER "));

		Health -= Damage;
		if (Health <= 0)
		{
			Die(Damage, DamageEvent, DamageCauser);
			UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [TakeDamage] DIE "));
		}
		else
		{
			//PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
			UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [TakeDamage] HIT "));
			
		}
	}

	return 0.f;
}

bool ALEETAPIDEMO3Character::hasBombs()
{
	if (BombCount > 0) {
		return true;
	}
	else {
		return false;
	}
}

void ALEETAPIDEMO3Character::ServerAttemptSpawnBomb_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [ServerAttemptSpawnBomb_Implementation]  "));
	if (CanFire()) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [ServerAttemptSpawnBomb_Implementation] CanFire "));
		float placementDistance = 200.0f;
		FVector ShootDir = GetActorForwardVector();
		FVector Origin = GetActorLocation();
		FVector spawnlocationstart = ShootDir * placementDistance;
		FVector spawnlocation = spawnlocationstart + Origin;
		// Spawn the actor
		//Spawn a bomb
		FTransform const SpawnTransform(ShootDir.Rotation(), spawnlocation);
		int32 playerID = PlayerState->PlayerId;
		AMyBomb* const BombActor = GetWorld()->SpawnActor<AMyBomb>(AMyBomb::StaticClass(), SpawnTransform);
		BombActor->setPlayerID(playerID);
	}
	

	// decrement bomb count
}

bool ALEETAPIDEMO3Character::ServerAttemptSpawnBomb_Validate()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3Character] [ServerAttemptSpawnBomb_Validate]  "));
	return true;
}

bool ALEETAPIDEMO3Character::Die(float KillingDamage, FDamageEvent const& DamageEvent,  AActor* DamageCauser)
{

	Health = FMath::Min(0.0f, Health);

	//AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	//GetWorld()->GetAuthGameMode<AMyGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	GetCharacterMovement()->ForceReplicationUpdate();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	// Death anim
	//float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	AMyPlayerController* victim = Cast<AMyPlayerController>(GetController());
	AMyBomb* bomb = Cast<AMyBomb>(DamageCauser);
	
	
	UMyGameInstance* TheGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	TheGameInstance->RecordKill(bomb->ownerPlayerID, victim->PlayerState->PlayerId);
	
	return true;
}

bool ALEETAPIDEMO3Character::IsAlive() const
{
	return Health > 0;
}

bool ALEETAPIDEMO3Character::CanFire() const
{
	return IsAlive();
}

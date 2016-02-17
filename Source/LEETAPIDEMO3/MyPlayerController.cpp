// Fill out your copyright notice in the Description page of Project Settings.

#include "LEETAPIDEMO3.h"
// Include the engine for casting
#include "Engine.h"
// Incude our game mode and instance
#include "MyGameMode.h"
#include "MyGameInstance.h"
// This is supposed to be last for some voodoo
#include "MyPlayerController.h"



void AMyPlayerController::GetAuthorizationInfo_Implementation(const FString& PlatformID, const int& playerID)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyPlayerController] PlatformID: %s"), *PlatformID);

	SendAuthorizationInfo(PlatformID, playerID);
	//UE_LOG(LogTemp, Log, TEXT("do stuff on server here"));



	// Use Game Instance because it does not get erased on level change
	// Get the game instance and cast to our game instance.
	UMyGameInstance* TheGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	TheGameInstance->AuthorizePlayer(PlatformID, playerID);

}

bool AMyPlayerController::GetAuthorizationInfo_Validate(const FString& PlatformID, const int32& playerID)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyPlayerController] Validate"));
	return true;
}

void AMyPlayerController::SendAuthorizationInfo_Implementation(const FString& PlatformID, const int32& playerID)
{
	// do stuff on client here
	UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyPlayerController] [SendAuthorizationInfo_Implementation] do stuff on client here"));
}




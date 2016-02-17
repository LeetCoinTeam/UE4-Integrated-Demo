// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "LEETAPIDEMO3GameMode.generated.h"

UCLASS(minimalapi)
class ALEETAPIDEMO3GameMode : public AGameMode
{
	GENERATED_BODY()

	FTimerHandle ChatTimerHandle;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** always pick new random spawn */
	//virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

protected:
	/** check if player can use spawnpoint */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check if player should use spawnpoint */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;
public:
	ALEETAPIDEMO3GameMode();
	void Logout(AController* Exiting);

	void PollAPIChat();

	/**
	* Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string.
	* PreLogin is called before Login.  Significant game time may pass before Login is called, especially if content is downloaded.
	*
	* @param	Options					The URL options (e.g. name/spectator) the player has passed
	* @param	Address					The network address of the player
	* @param	UniqueId				The unique id the player has passed to the server
	* @param	ErrorMessage			When set to a non-empty value, the player will be rejected using the error message set
	*/
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage);

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerAController. */
	virtual void PostLogin(APlayerController* NewPlayer);
};




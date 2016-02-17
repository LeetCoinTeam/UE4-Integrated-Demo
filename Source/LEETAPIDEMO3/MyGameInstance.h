// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Http.h"
#include "Json.h"
#include "Base64.h"
#include <string>
#include "MyPlayerState.h"
#include "MyGameInstance.generated.h"

USTRUCT()
struct FMyActivePlayer {

	GENERATED_USTRUCT_BODY()
	int32 playerID;
	FString platformID;
	FString playerKey;
	FString playerTitle;
	//FString title; 
	bool authorized;
	int32 roundKills;
	int32 roundDeaths;
	TArray<FString> killed;
	int32 Rank;
	int32 BTCHold;
};

/**
 * 
 */
UCLASS()
class LEETAPIDEMO3_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	// Populated via Game.ini
	FString ServerAPIKey;
	FString ServerAPISecret;
	// Populated through the get server info API call
	int32 incrementBTC;
	int32 killRewardBTC; //convenience so we don't have to do the math every time.
	int32 minimumBTCHold;
	float serverRakeBTCPercentage;
	float leetRakePercentage;

	bool matchStarted;

	TArray<FMyActivePlayer> ActivePlayers;

	UMyGameInstance();
public:

	UFUNCTION(BlueprintCallable, Category = "LEET")
	FMyActivePlayer getPlayerByPlayerID(int32 playerID);

	UFUNCTION(BlueprintCallable, Category = "LEET")
		FString getPlayerTitleByPlayerID(int32 playerID);

	UFUNCTION(BlueprintCallable, Category = "LEET")
		int32 getPlayerBalanceByPlayerID(int32 playerID);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LEET")
		int32 MinimumPlayersNeededToStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LEET")
		int32 MinimumPlayerDeathsBeforeRoundReset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LEET")
		int32 MinimumKillsBeforeResultsSubmit;

	UFUNCTION(BlueprintCallable, Category = "LEET")
		bool AuthorizePlayer(FString PlatformID, int32 playerID);
	/**
	* Delegate called when the request completes
	*
	* @param HttpRequest - object that started/processed the request
	* @param HttpResponse - optional response object if request completed
	* @param bSucceeded - true if Url connection was made and response was received
	*/
	void ActivateRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool DeAuthorizePlayer(int32 playerID);
	void DeAuthorizeRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool OutgoingChat(int32 playerID, FText message);
	void OutgoingChatComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool SubmitMatchResults();
	void SubmitMatchResultsComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool GetServerInfo();
	void GetServerInfoComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION(BlueprintCallable, Category = "LEET")
	bool RecordKill(int32 killerPlayerID, int32 victimPlayerID);

	bool GetMatchStarted();
	
};

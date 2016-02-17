// Fill out your copyright notice in the Description page of Project Settings.

#include "LEETAPIDEMO3.h"
#include "MyPlayerController.h"
#include "LEETAPIDEMO3Character.h"
#include "MyPlayerState.h"
#include "MyGameInstance.h"


UMyGameInstance::UMyGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] GAME INSTANCE INIT"));

	if (!GConfig) return;
	//~~

	// This totally works.  Just needed some voodooo to build reload refresh rebuild etc.

	// try setting a variable
	//New Section to Add
	//FString VictorySection = "Victory.Core";

	//String
	//GConfig->SetString(
	//	*VictorySection,
	//	TEXT("RootDir"),
	//	TEXT("ExampleVariable"),
	//	GGameIni
	//	);
	//GConfig->Flush(false, GGameIni);

	//Retrieve Defaults that we saved in the .ini file
	FString ValueReceived2;
	GConfig->GetString(
		TEXT("LEET.Server"),
		TEXT("ServerAPIKey"),
		ValueReceived2,
		GGameIni
		);
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] ValueReceived2: %s"), *ValueReceived2);
	ServerAPIKey = ValueReceived2;

	FString ValueReceived3;
	GConfig->GetString(
		TEXT("LEET.Server"),
		TEXT("ServerAPISecret"),
		ValueReceived3,
		GGameIni
		);
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] ValueReceived3: %s"), *ValueReceived3);
	ServerAPISecret = ValueReceived3;
	//ActivePlayers;
	//int32 playerlistmax = 20;
	//ActivePlayers.SetNumUninitialized(playerlistmax);

	matchStarted = false;

	// Just hardcoding this for now
	MinimumPlayersNeededToStart = 2;
	MinimumPlayerDeathsBeforeRoundReset = 1;  // When one player dies reset the level
	MinimumKillsBeforeResultsSubmit = 3; // When a player has a score of x, submit match results

	//GetWorld()->GetTimerManager().SetTimer(ChatTimerHandle, this, &UMyGameInstance::PollAPIChat, 1.0f, true);

	// Grab the server setup from leet
	GetServerInfo();

}

FMyActivePlayer UMyGameInstance::getPlayerByPlayerID(int32 playerID)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] getPlayerByPlayerID"));
	for (int32 b = 0; b < ActivePlayers.Num(); b++)
	{
		//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [AuthorizePlayer] playerID: %s"), *ActivePlayers[b].playerID);
		if (ActivePlayers[b].playerID == playerID) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] AuthorizePlayer - FOUND MATCHING playerID"));
			return ActivePlayers[b];
		}
	}
	FString platformID = "None";
	int32 playerIDFake = 0;
	FMyActivePlayer fakeplayer;
	fakeplayer.playerID = 0;
	return fakeplayer;

}

FString UMyGameInstance::getPlayerTitleByPlayerID(int32 playerID)
{
	FMyActivePlayer player = getPlayerByPlayerID(playerID);
	if (player.playerID == 0) {
		return "Unknown";
	}
	else {
		return player.playerTitle;
	}
}

int32 UMyGameInstance::getPlayerBalanceByPlayerID(int32 playerID)
{
	FMyActivePlayer player = getPlayerByPlayerID(playerID);
	if (player.playerID == 0) {
		return 0;
	}
	else {
		return player.BTCHold;
	}
}

bool UMyGameInstance::AuthorizePlayer(FString PlatformID, int32 playerID)
{

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] AuthorizePlayer"));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DEBUG TEST"));

	// check to see if this player is in the active list already
	bool platformIDFound = false;
	int32 ActivePlayerIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] check to see if this player is in the active list already"));
	
	//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [AuthorizePlayer] ActivePlayers: %i"), ActivePlayers);

		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] ActivePlayers.Num() > 0"));
		for (int32 b = 0; b < ActivePlayers.Num(); b++)
		{
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [AuthorizePlayer] platformID: %s"), *ActivePlayers[b].platformID);
			if (ActivePlayers[b].platformID == PlatformID) {
				UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] AuthorizePlayer - FOUND MATCHING platformID"));
				platformIDFound = true;
				ActivePlayerIndex = b;
			}

		}
	
	
	if (platformIDFound == false) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] AuthorizePlayer - No existing platformID found"));

		// add the player to the TArray as authorized=false
		FMyActivePlayer activeplayer;
		activeplayer.playerID = playerID;
		activeplayer.platformID = PlatformID;
		activeplayer.authorized = false;
		activeplayer.roundDeaths = 0;
		activeplayer.roundKills = 0;


		ActivePlayers.Add(activeplayer);

		FHttpModule* Http = &FHttpModule::Get();
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character"));
		if (!Http) { return false; }
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is available"));
		if (!Http->IsHttpEnabled()) { return false; }
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is enabled"));


		UE_LOG(LogTemp, Log, TEXT("PlatformID: %s"), *PlatformID);
		UE_LOG(LogTemp, Log, TEXT("Object is: %s"), *GetName());

		FString nonceString = "10951350917635";
		FString encryption = "off";  // Allowing unencrypted on sandbox for now.  


		FString OutputString;
		// TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		// Build Params as text string
		OutputString = "nonce=" + nonceString + "&encryption=" + encryption;
		// urlencode the params

		//Make sure we are using UTF-8 as that is the de-facto standard when hashing string with SHA1
		std::string stdstring(TCHAR_TO_UTF8(*OutputString));

		FSHA1 HashState;
		HashState.Update((uint8*)stdstring.c_str(), stdstring.size());
		HashState.Final();

		uint8 Hash[FSHA1::DigestSize];
		HashState.GetHash(Hash);

		FString HashStr = BytesToHex(Hash, FSHA1::DigestSize);
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character hash"));
		UE_LOG(LogTemp, Log, TEXT("HashStr: %s"), *HashStr);

		// Base64 encode?
		//FString BaseStr = FBase64::Encode()
		//FBase64->Encode

		FString TargetHost = "http://apitest-dot-leetsandbox.appspot.com/api/v2/player/" + PlatformID + "/activate";
		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->SetVerb("POST");
		Request->SetURL(TargetHost);
		Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetHeader("Key", ServerAPIKey);
		Request->SetHeader("Sign", HashStr);
		Request->SetContentAsString(OutputString);

		Request->OnProcessRequestComplete().BindUObject(this, &UMyGameInstance::ActivateRequestComplete);
		if (!Request->ProcessRequest()) { return false; }

		return true;

	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] AuthorizePlayer - TODO update record"));
	}
	return true;

}

void UMyGameInstance::ActivateRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Test failed. NULL response"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Completed test [%s] Url=[%s] Response=[%d] [%s]"),
			*HttpRequest->GetVerb(),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			*HttpResponse->GetContentAsString());

		APlayerController* pc = NULL;
		int32 playerstateID;

		FString JsonRaw = *HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			bool Authorization = JsonParsed->GetBoolField("authorization");
			UE_LOG(LogTemp, Log, TEXT("Authorization"));
			if (Authorization)
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization True"));
				bool PlayerAuthorized = JsonParsed->GetBoolField("player_authorized");
				if (PlayerAuthorized) {
					UE_LOG(LogTemp, Log, TEXT("Player Authorized"));


					//FString JsonLeetPlatformId = JsonParsed->GetStringField("player_platformid");
					//this->LeetPlatformId = JsonLeetPlatformId;
					//UE_LOG(LogTemp, Log, TEXT("JsonLeetPlatformId: %s"), *JsonLeetPlatformId);

					//LeetPlatformId = JsonParsed->GetStringField("player_platformid");
					//UE_LOG(LogTemp, Log, TEXT("LeetPlatformId: %s"), *LeetPlatformId);

					//UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap");

					int32 activeAuthorizedPlayers = 0;
					int32 activePlayerIndex;

					UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] ActivePlayers.Num() > 0"));
					for (int32 b = 0; b < ActivePlayers.Num(); b++)
					{
						UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] platformID: %s"), *ActivePlayers[b].platformID);
						if (ActivePlayers[b].platformID == JsonParsed->GetStringField("player_platformid")) {
							UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - FOUND MATCHING platformID"));
							activePlayerIndex = b;
							ActivePlayers[b].authorized = true;
							ActivePlayers[b].playerTitle = JsonParsed->GetStringField("player_name");
							ActivePlayers[b].playerKey = JsonParsed->GetStringField("player_key");
							ActivePlayers[b].BTCHold = JsonParsed->GetIntegerField("player_btchold");
							ActivePlayers[b].Rank = JsonParsed->GetIntegerField("player_rank");
							
						}
						if (ActivePlayers[b].authorized) {
							activeAuthorizedPlayers++;
						}

					}

					// ALso set this player state playerName
					
					for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
					{
						UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Looking for player to set name"));
						pc = Iterator->Get();
						AMyPlayerController* thisPlayerController = Cast<AMyPlayerController>(pc);
						if (thisPlayerController) {
							UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Cast Controller success"));

							if (matchStarted) {
								UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Match in progress - setting spectator"));
								thisPlayerController->PlayerState->bIsSpectator = true;
								thisPlayerController->ChangeState(NAME_Spectating);
								thisPlayerController->ClientGotoState(NAME_Spectating);
							}
							playerstateID = thisPlayerController->PlayerState->PlayerId;
							if (ActivePlayers[activePlayerIndex].playerID == playerstateID)
							{
								UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - playerID match - setting name"));
								thisPlayerController->PlayerState->SetPlayerName(JsonParsed->GetStringField("player_name"));
							}
						}
					}


					if (activeAuthorizedPlayers >= MinimumPlayersNeededToStart)
					{
						matchStarted = true;
						// travel to the third person map
						FString UrlString = TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
						GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
						GetWorld()->ServerTravel(UrlString);
					}
				
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Player NOT Authorized"));

					// First grab the active player data from our struct
					int32 activePlayerIndex;
					bool platformIDFound = false;
					FString jsonPlatformID = JsonParsed->GetStringField("player_platformid");

					for (int32 b = 0; b < ActivePlayers.Num(); b++)
					{
						UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] platformID: %s"), *ActivePlayers[b].platformID);
						UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] jsonPlatformID: %s"), *jsonPlatformID);
						if (ActivePlayers[b].platformID == jsonPlatformID) {
							UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Found active player record"));
							platformIDFound = true;
							activePlayerIndex = b;
						}
					}

					
					

					if (platformIDFound)
					{
						UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - PlatformID is found - moving to kick"));
						for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
						{
							UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Looking for player to kick"));
							pc = Iterator->Get();
							AMyPlayerController* thisPlayerController = Cast<AMyPlayerController>(pc);
							if (thisPlayerController) {
								UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - Cast Controller success"));
								playerstateID = thisPlayerController->PlayerState->PlayerId;
								if (ActivePlayers[activePlayerIndex].playerID == playerstateID)
								{
									UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] - playerID match - kicking back to connect"));
									FString UrlString = TEXT("/Game/MyConnectLevel");
									ETravelType seamlesstravel = TRAVEL_Absolute;
									thisPlayerController->ClientTravel(UrlString, seamlesstravel);
								}
							}
						}
					}
				}
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [ActivateRequestComplete] Done!"));
}


bool UMyGameInstance::DeAuthorizePlayer(int32 playerID)
{

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DeAuthorizePlayer"));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DEBUG TEST"));

	// check to see if this player is in the active list already
	bool playerIDFound = false;
	int32 ActivePlayerIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] check to see if this player is in the active list already"));

	//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [AuthorizePlayer] ActivePlayers: %i"), ActivePlayers);

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] ActivePlayers.Num() > 0"));
	for (int32 b = 0; b < ActivePlayers.Num(); b++)
	{
		//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [DeAuthorizePlayer] playerID: %s"), ActivePlayers[b].playerID);
		if (ActivePlayers[b].playerID == playerID) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DeAuthorizePlayer - FOUND MATCHING playerID"));
			playerIDFound = true;
			ActivePlayerIndex = b;
		}

	}


	if (playerIDFound == true) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DeAuthorizePlayer - existing playerID found"));

		// update the TArray as authorized=false
		FMyActivePlayer leavingplayer;
		leavingplayer.playerID = playerID;
		leavingplayer.authorized = false;
		leavingplayer.platformID = ActivePlayers[ActivePlayerIndex].platformID;

		FString PlatformID = ActivePlayers[ActivePlayerIndex].platformID;

		ActivePlayers[ActivePlayerIndex] = leavingplayer;

		FHttpModule* Http = &FHttpModule::Get();
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character"));
		if (!Http) { return false; }
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is available"));
		if (!Http->IsHttpEnabled()) { return false; }
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is enabled"));


		UE_LOG(LogTemp, Log, TEXT("PlatformID: %s"), *PlatformID);
		UE_LOG(LogTemp, Log, TEXT("Object is: %s"), *GetName());

		FString nonceString = "10951350917635";
		FString encryption = "off";  // Allowing unencrypted on sandbox for now.  


		FString OutputString;
		// TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		// Build Params as text string
		OutputString = "nonce=" + nonceString + "&encryption=" + encryption;
		// urlencode the params

		//Make sure we are using UTF-8 as that is the de-facto standard when hashing string with SHA1
		std::string stdstring(TCHAR_TO_UTF8(*OutputString));

		FSHA1 HashState;
		HashState.Update((uint8*)stdstring.c_str(), stdstring.size());
		HashState.Final();

		uint8 Hash[FSHA1::DigestSize];
		HashState.GetHash(Hash);

		FString HashStr = BytesToHex(Hash, FSHA1::DigestSize);
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character hash"));
		UE_LOG(LogTemp, Log, TEXT("HashStr: %s"), *HashStr);

		// Base64 encode?
		//FString BaseStr = FBase64::Encode()
		//FBase64->Encode

		FString TargetHost = "http://apitest-dot-leetsandbox.appspot.com/api/v2/player/" + PlatformID + "/deactivate";
		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->SetVerb("POST");
		Request->SetURL(TargetHost);
		Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetHeader("Key", ServerAPIKey);
		Request->SetHeader("Sign", HashStr);
		Request->SetContentAsString(OutputString);

		Request->OnProcessRequestComplete().BindUObject(this, &UMyGameInstance::DeAuthorizeRequestComplete);
		if (!Request->ProcessRequest()) { return false; }

		return true;

	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DeAuthorizePlayer - Not found - Ignoring"));
	}
	return true;

}

void UMyGameInstance::DeAuthorizeRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Test failed. NULL response"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Completed test [%s] Url=[%s] Response=[%d] [%s]"),
			*HttpRequest->GetVerb(),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			*HttpResponse->GetContentAsString());

		FString JsonRaw = *HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			bool Authorization = JsonParsed->GetBoolField("authorization");
			//  We don't care too much about the results from this call.  
			UE_LOG(LogTemp, Log, TEXT("Authorization"));
			if (Authorization)
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization True"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization False"));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [DeAuthorizeRequestComplete] Done!"));
}

bool UMyGameInstance::OutgoingChat(int32 playerID, FText message)
{

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] OutgoingChat"));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DEBUG TEST"));

	// check to see if this player is in the active list already
	bool playerIDFound = false;
	int32 ActivePlayerIndex = 0;
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] check to see if this player is in the active list already"));

	//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [AuthorizePlayer] ActivePlayers: %i"), ActivePlayers);

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] ActivePlayers.Num() > 0"));
	for (int32 b = 0; b < ActivePlayers.Num(); b++)
	{
		//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [OutgoingChat] playerID: %s"), ActivePlayers[b].playerID);
		if (ActivePlayers[b].playerID == playerID) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] OutgoingChat - FOUND MATCHING playerID"));
			playerIDFound = true;
			ActivePlayerIndex = b;
		}

	}


	if (playerIDFound == true) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] OutgoingChat - existing playerID found"));

		FString PlatformID = ActivePlayers[ActivePlayerIndex].platformID;

		FHttpModule* Http = &FHttpModule::Get();
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character"));
		if (!Http) { return false; }
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is available"));
		if (!Http->IsHttpEnabled()) { return false; }
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character http is enabled"));


		UE_LOG(LogTemp, Log, TEXT("PlatformID: %s"), *PlatformID);
		UE_LOG(LogTemp, Log, TEXT("Object is: %s"), *GetName());
		UE_LOG(LogTemp, Log, TEXT("message is: %s"), *message.ToString());

		FString nonceString = "10951350917635";
		FString encryption = "off";  // Allowing unencrypted on sandbox for now.  


		FString OutputString;
		// TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		// Build Params as text string
		OutputString = "nonce=" + nonceString + "&encryption=" + encryption + "&message=" + message.ToString();
		// urlencode the params

		//Make sure we are using UTF-8 as that is the de-facto standard when hashing string with SHA1
		std::string stdstring(TCHAR_TO_UTF8(*OutputString));

		FSHA1 HashState;
		HashState.Update((uint8*)stdstring.c_str(), stdstring.size());
		HashState.Final();

		uint8 Hash[FSHA1::DigestSize];
		HashState.GetHash(Hash);

		FString HashStr = BytesToHex(Hash, FSHA1::DigestSize);
		//UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character hash"));
		UE_LOG(LogTemp, Log, TEXT("HashStr: %s"), *HashStr);

		// Base64 encode?
		//FString BaseStr = FBase64::Encode()
		//FBase64->Encode

		FString TargetHost = "http://apitest-dot-leetsandbox.appspot.com/api/v2/player/" + PlatformID + "/chat";
		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->SetVerb("POST");
		Request->SetURL(TargetHost);
		Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetHeader("Key", ServerAPIKey);
		Request->SetHeader("Sign", HashStr);
		Request->SetContentAsString(OutputString);

		Request->OnProcessRequestComplete().BindUObject(this, &UMyGameInstance::OutgoingChatComplete);
		if (!Request->ProcessRequest()) { return false; }

		return true;

	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] OutgoingChat - Not found - Ignoring"));
	}
	return true;

}

void UMyGameInstance::OutgoingChatComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Test failed. NULL response"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Completed test [%s] Url=[%s] Response=[%d] [%s]"),
			*HttpRequest->GetVerb(),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			*HttpResponse->GetContentAsString());

		FString JsonRaw = *HttpResponse->GetContentAsString();
		//  We don't care too much about the results from this call.  
	}
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [OutgoingChatComplete] Done!"));
}

bool UMyGameInstance::RecordKill(int32 killerPlayerID, int32 victimPlayerID)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] "));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] killerPlayerID: %i"), killerPlayerID);
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] victimPlayerID: %i "), victimPlayerID);

	// get attacker activeplayer
	bool attackerPlayerIDFound = false;
	int32 killerPlayerIndex = 0;
	// sum all the round kills while we're looping
	int32 roundKillsTotal = 0;

	for (int32 b = 0; b < ActivePlayers.Num(); b++)
	{
		//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [DeAuthorizePlayer] playerID: %s"), ActivePlayers[b].playerID);
		if (ActivePlayers[b].playerID == killerPlayerID) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - FOUND MATCHING killer playerID"));
			attackerPlayerIDFound = true;
			killerPlayerIndex = b;
		}
		roundKillsTotal = roundKillsTotal + ActivePlayers[b].roundKills;
	}

	// we need one more since this kill has not been added to the list yet
	roundKillsTotal = roundKillsTotal + 1;

	if (killerPlayerID == victimPlayerID) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] suicide"));

		
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] Not a suicide"));

		

		// get victim activeplayer
		bool victimPlayerIDFound = false;
		int32 victimPlayerIndex = 0;

		for (int32 b = 0; b < ActivePlayers.Num(); b++)
		{
			//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [DeAuthorizePlayer] playerID: %s"), ActivePlayers[b].playerID);
			if (ActivePlayers[b].playerID == victimPlayerID) {
				UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - FOUND MATCHING victim playerID"));
				victimPlayerIDFound = true;
				victimPlayerIndex = b;
			}
		}

		// check to see if this victim is already in the kill list
		bool victimIDFoundInKillList = false;
		

		for (int32 b = 0; b < ActivePlayers[killerPlayerIndex].killed.Num(); b++)
		{
			if (ActivePlayers[killerPlayerIndex].killed[b] == ActivePlayers[victimPlayerIndex].playerKey)
			{
				UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Victim already in kill list"));
				victimIDFoundInKillList = true;
			}
			
		}

		if (victimIDFoundInKillList == false) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Adding victim to kill list"));
			ActivePlayers[killerPlayerIndex].killed.Add(ActivePlayers[victimPlayerIndex].playerKey);
		}


		// Increase the killer's kill count
		ActivePlayers[killerPlayerIndex].roundKills = ActivePlayers[killerPlayerIndex].roundKills +1;
		// Increase the killer's balance
		ActivePlayers[killerPlayerIndex].BTCHold = ActivePlayers[killerPlayerIndex].BTCHold + killRewardBTC;
		// And increase the victim's deaths
		ActivePlayers[victimPlayerIndex].roundDeaths = ActivePlayers[victimPlayerIndex].roundDeaths + 1;
		// Decrease the victim's balance
		ActivePlayers[victimPlayerIndex].BTCHold = ActivePlayers[victimPlayerIndex].BTCHold - incrementBTC;

		// TODO kick the victim if it falls below the minimum?
		// Not super critical since we force everyone to login again after each match is over.

		APlayerController* pc = NULL;
		// Send out a chat message to all players
		FText chatSender = NSLOCTEXT("LEET", "chatSender", "SYSTEM");
		FText chatMessageText = NSLOCTEXT("LEET", "chatMessageText", " killed ");  //TODO figure out how to set up this string correctly.

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Sending text chat"));
			pc = Iterator->Get();
			AMyPlayerController* thisPlayerController = Cast<AMyPlayerController>(pc);
			if (thisPlayerController) {
				UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Cast Controller success"));

				APlayerState* thisPlayerState = thisPlayerController->PlayerState;
				AMyPlayerState* thisMyPlayerState = Cast<AMyPlayerState>(thisPlayerState);
				if (thisMyPlayerState) {
					UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Cast State success"));
					thisMyPlayerState->ReceiveChatMessage(chatSender, chatMessageText);
				}
			}
		}

		
	}
	//Check to see if the game is over
	if (roundKillsTotal >= MinimumKillsBeforeResultsSubmit) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Ending the Game"));
		bool gamesubmitted = SubmitMatchResults();

		//Deauthorize everyone
		for (int32 b = 0; b < ActivePlayers.Num(); b++)
		{
			DeAuthorizePlayer(ActivePlayers[b].playerID);
		}

		// We might need some kind of delay in here because we're going to wipe this data.  plz don't crash

		// Reset the active players
		ActivePlayers.Empty();
		// Kick everyone back to login
		FString UrlString = TEXT("/Game/MyLoginLevel?listen");
		GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
		GetWorld()->ServerTravel(UrlString);

	}
	else {
		// Check to see if the round is over
		if (roundKillsTotal >= MinimumPlayerDeathsBeforeRoundReset) {
			UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [RecordKill] - Ending the Round"));
			// travel to the third person map
			FString UrlString = TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
			GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(UrlString);
		}
	}



	return true;
}

bool UMyGameInstance::SubmitMatchResults()
{

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] SubmitMatchResults"));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DEBUG TEST"));

	FString player_dict_list = "%5B";  //TODO build this string urlencoded from json properly
	bool FoundKills = false;
	// get the data ready
	
	for (int32 b = 0; b < ActivePlayers.Num(); b++)
	{
		//UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [DeAuthorizePlayer] playerID: %s"), ActivePlayers[b].playerID);
		if (ActivePlayers[b].roundKills > 0) {
			FoundKills = true;
		}
		player_dict_list = player_dict_list + "%7B%22deaths%22%3A" + FString::FromInt(ActivePlayers[b].roundDeaths)+ "%2C";  // deaths
		player_dict_list = player_dict_list + "%22killed%22%3A+%5B"; // killed -list, go through em.
		for (int32 pkilledi = 0; pkilledi < ActivePlayers[b].killed.Num(); pkilledi++)
		{
			player_dict_list = player_dict_list + "%22" + ActivePlayers[b].killed[pkilledi] + "%22";
			if (pkilledi < ActivePlayers[b].killed.Num() - 1) {
				// If it's not the last one add a comma.  I know this is dumb and should just be json
				player_dict_list = player_dict_list + "%2C+";
			}
		}
		player_dict_list = player_dict_list + "%5D%2C";
		player_dict_list = player_dict_list + "%22platformID%22%3A%22" + ActivePlayers[b].platformID + "%22%2C";
		player_dict_list = player_dict_list + "%22kills%22%3A+" + FString::FromInt(ActivePlayers[b].roundKills) + "%2C";
		player_dict_list = player_dict_list + "%22experience%22%3A+" + FString::FromInt(ActivePlayers[b].roundKills) + "%2C";
		player_dict_list = player_dict_list + "%22weapon%22%3A%22Bomb%22";
		player_dict_list = player_dict_list + "%7D";
		if (b < ActivePlayers.Num() - 1) {
			// If it's not the last one add a comma.  I know this is dumb and should just be json
			player_dict_list = player_dict_list + "%2C+";
		}
	}
	player_dict_list = player_dict_list + "%5D";
	


	if (FoundKills == true) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] SubmitMatchResults - found kills"));

		FHttpModule* Http = &FHttpModule::Get();
		if (!Http) { return false; }
		if (!Http->IsHttpEnabled()) { return false; }

		UE_LOG(LogTemp, Log, TEXT("Object is: %s"), *GetName());

		FString nonceString = "10951350917635";
		FString encryption = "off";  // Allowing unencrypted on sandbox for now.  


		FString OutputString;
		// TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

		// Build Params as text string
		OutputString = "nonce=" + nonceString + "&encryption=" + encryption + "&map_title=Demo&player_dict_list=" + player_dict_list;
		// urlencode the params

		//Make sure we are using UTF-8 as that is the de-facto standard when hashing string with SHA1
		std::string stdstring(TCHAR_TO_UTF8(*OutputString));

		FSHA1 HashState;
		HashState.Update((uint8*)stdstring.c_str(), stdstring.size());
		HashState.Final();

		uint8 Hash[FSHA1::DigestSize];
		HashState.GetHash(Hash);

		FString HashStr = BytesToHex(Hash, FSHA1::DigestSize);
		UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character hash"));
		UE_LOG(LogTemp, Log, TEXT("HashStr: %s"), *HashStr);

		// Base64 encode?
		//FString BaseStr = FBase64::Encode()
		//FBase64->Encode

		FString TargetHost = "http://apitest-dot-leetsandbox.appspot.com/api/v2/match/results";
		TSharedRef < IHttpRequest > Request = Http->CreateRequest();
		Request->SetVerb("POST");
		Request->SetURL(TargetHost);
		Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetHeader("Key", ServerAPIKey);
		Request->SetHeader("Sign", HashStr);
		Request->SetContentAsString(OutputString);

		Request->OnProcessRequestComplete().BindUObject(this, &UMyGameInstance::SubmitMatchResultsComplete);
		if (!Request->ProcessRequest()) { return false; }

		return true;

	}
	else {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] SubmitMatchResults - No Kills - Ignoring"));
	}
	return true;

}

void UMyGameInstance::SubmitMatchResultsComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Test failed. NULL response"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Completed test [%s] Url=[%s] Response=[%d] [%s]"),
			*HttpRequest->GetVerb(),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			*HttpResponse->GetContentAsString());

		FString JsonRaw = *HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			bool Authorization = JsonParsed->GetBoolField("authorization");
			//  We don't care too much about the results from this call.  
			UE_LOG(LogTemp, Log, TEXT("Authorization"));
			if (Authorization)
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization True"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization False"));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [SubmitMatchResults] Done!"));
}

bool UMyGameInstance::GetServerInfo()
{

	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] GetServerInfo"));
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] DEBUG TEST"));

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) { return false; }
	if (!Http->IsHttpEnabled()) { return false; }

	UE_LOG(LogTemp, Log, TEXT("Object is: %s"), *GetName());

	FString nonceString = "10951350917635";
	FString encryption = "off";  // Allowing unencrypted on sandbox for now.  


	FString OutputString;
	// TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	// Build Params as text string
	OutputString = "nonce=" + nonceString + "&encryption=" + encryption;
	// urlencode the params

	//Make sure we are using UTF-8 as that is the de-facto standard when hashing string with SHA1
	std::string stdstring(TCHAR_TO_UTF8(*OutputString));

	FSHA1 HashState;
	HashState.Update((uint8*)stdstring.c_str(), stdstring.size());
	HashState.Final();

	uint8 Hash[FSHA1::DigestSize];
	HashState.GetHash(Hash);

	FString HashStr = BytesToHex(Hash, FSHA1::DigestSize);
	UE_LOG(LogTemp, Log, TEXT("ALEETDEMO2Character hash"));
	UE_LOG(LogTemp, Log, TEXT("HashStr: %s"), *HashStr);

	// Base64 encode?
	//FString BaseStr = FBase64::Encode()
	//FBase64->Encode

	FString TargetHost = "http://apitest-dot-leetsandbox.appspot.com/api/v2/server/info";
	TSharedRef < IHttpRequest > Request = Http->CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL(TargetHost);
	Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	Request->SetHeader("Key", ServerAPIKey);
	Request->SetHeader("Sign", HashStr);
	Request->SetContentAsString(OutputString);

	Request->OnProcessRequestComplete().BindUObject(this, &UMyGameInstance::GetServerInfoComplete);
	if (!Request->ProcessRequest()) { return false; }

	return true;
}

void UMyGameInstance::GetServerInfoComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpResponse.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Test failed. NULL response"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Completed test [%s] Url=[%s] Response=[%d] [%s]"),
			*HttpRequest->GetVerb(),
			*HttpRequest->GetURL(),
			HttpResponse->GetResponseCode(),
			*HttpResponse->GetContentAsString());

		FString JsonRaw = *HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
		if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
		{
			bool Authorization = JsonParsed->GetBoolField("authorization");
			//  We don't care too much about the results from this call.  
			UE_LOG(LogTemp, Log, TEXT("Authorization"));
			if (Authorization)
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization True"));
				// Set up our instance variables
				incrementBTC = JsonParsed->GetIntegerField("incrementBTC");
				minimumBTCHold = JsonParsed->GetIntegerField("minimumBTCHold");
				serverRakeBTCPercentage = JsonParsed->GetNumberField("serverRakeBTCPercentage");
				leetRakePercentage = JsonParsed->GetNumberField("leetcoinRakePercentage");
				killRewardBTC = incrementBTC - ((incrementBTC * serverRakeBTCPercentage) + (incrementBTC * leetRakePercentage));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Authorization False"));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[LEET] [UMyGameInstance] [GetServerInfoComplete] Done!"));
}

bool UMyGameInstance::GetMatchStarted()
{
	return matchStarted;
}
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LEETAPIDEMO3.h"
#include "LEETAPIDEMO3GameMode.h"
// Include the engine for casting
#include "Engine.h"
#include "MyPlayerState.h"
#include "MyGameInstance.h"
#include "LEETAPIDEMO3Character.h"

ALEETAPIDEMO3GameMode::ALEETAPIDEMO3GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	//GetWorldTimerManager().SetTimer(ChatTimerHandle, this, &ALEETAPIDEMO3GameMode::PollAPIChat, 5.0f, true);
}

void ALEETAPIDEMO3GameMode::Logout(AController* Exiting)
{
	// Handle users that disconnect from the server.
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3GameMode] [Logout] "));
	Super::Logout(Exiting);

	AMyPlayerState* ExitingPlayerState = Cast<AMyPlayerState>(Exiting->PlayerState);
	int ExitingPlayerId = ExitingPlayerState->PlayerId;

	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3GameMode] [Logout] ExitingPlayerId: %i"), ExitingPlayerId);

	UMyGameInstance* TheGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());

	TheGameInstance->DeAuthorizePlayer(ExitingPlayerId);

}

void ALEETAPIDEMO3GameMode::PollAPIChat()
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3GameMode] [PollAPIChat] Start"));
}

AActor* ALEETAPIDEMO3GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3GameMode] [ChoosePlayerStart_Implementation] Start"));
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;
		if (TestSpawn->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
		else
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}


	if (BestStart == NULL)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

bool ALEETAPIDEMO3GameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
		return true;
}

bool ALEETAPIDEMO3GameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Cast<ACharacter>((*DefaultPawnClass)->GetDefaultObject<ACharacter>());

	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void ALEETAPIDEMO3GameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	ErrorMessage = GameSession->ApproveLogin(Options);
	UMyGameInstance* TheGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (TheGameInstance->GetMatchStarted()) {
		ErrorMessage = "The match has already started";
	}
}

void ALEETAPIDEMO3GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UMyGameInstance* TheGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (TheGameInstance->GetMatchStarted()) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [ALEETAPIDEMO3GameMode] [PostLogin] - Match in progress - setting spectator"));
		NewPlayer->PlayerState->bIsSpectator = true;
		NewPlayer->ChangeState(NAME_Spectating);
		NewPlayer->ClientGotoState(NAME_Spectating);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
//Input
#include "InputCoreTypes.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LEETAPIDEMO3_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// This is the function that gets called in the widget Blueprint
	UFUNCTION(BlueprintCallable, Category = "LEET", Server, Reliable, WithValidation)
		void GetAuthorizationInfo(const FString& PlatformID, const int32& playerID);
	// valid/imp methods

	// This function is called remotely by the server.
	UFUNCTION(BlueprintCallable, Category = "LEET", Client, Reliable)
		void SendAuthorizationInfo(const FString& PlatformID, const int32& playerID);
	// imp method
	
	
};

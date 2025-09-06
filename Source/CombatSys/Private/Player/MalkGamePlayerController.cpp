// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MalkGamePlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"


void AMalkGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
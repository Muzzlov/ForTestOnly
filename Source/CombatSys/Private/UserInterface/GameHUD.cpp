#include "Userinterface/GameHUD.h"
#include "Userinterface/MainMenu.h"
#include "Player/PlayerChar.h"
#include "Userinterface/Interaction/InteractionWidget.h"
#include "Interfaces/InteractionInterface.h"

AGameHUD::AGameHUD()
{
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5); //5 - это позиция на экране, глубина виджитов, изменить позднее
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1); // негативный, под основным меню
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		CrosshairWidget->AddToViewport();
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void AGameHUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AGameHUD::ToggleMenu()
{
	if (bIsMenuVisible)
	{
		HideMenu();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();
		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AGameHUD::ShowCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
}

void AGameHUD::HideCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AGameHUD::ShowInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AGameHUD::UpdateInteractionWidget(const FInteractableData* InteractableData)
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}
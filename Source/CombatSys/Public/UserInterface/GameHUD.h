// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UMainMenu;
class UInteractionWidget;
struct FInteractableData;

/**
 *
 */
UCLASS()
class COMBATSYS_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:

	/* ================ Переменные ================= */

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	bool bIsMenuVisible;

	/* ============ Функции ============ */

	AGameHUD();


	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowCrosshair();
	void HideCrosshair();

	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateInteractionWidget(const FInteractableData* InteractableData);

protected:

	/* ================ Переменные ================= */

	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY()
	UUserWidget* CrosshairWidget;

	/* ============ Функции ============ */

	virtual void BeginPlay() override;

};

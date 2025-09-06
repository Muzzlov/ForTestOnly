// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class APlayerChar;
class UInventoryPanel;
class UContainerInterface;
class UDragDropOperation;
class UItemDragDropOperation;

UCLASS()
class COMBATSYS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY()
	APlayerChar* PlayerChar;
	UPROPERTY()
	APlayerChar* PlayerCharacter;


protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};

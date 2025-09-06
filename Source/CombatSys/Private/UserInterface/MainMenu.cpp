// Fill out your copyright notice in the Description page of Project Settings.


#include "Userinterface/MainMenu.h"
#include "Player/PlayerChar.h"
#include "Items/ItemBase.h"
#include "UserInterface/Inventory/InventoryPanel.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Blueprint/DragDropOperation.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

// Drag&Drop на пол, что бы появлялся предмет в мире

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<APlayerChar>(GetOwningPlayerPawn());
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;

}

#include "Items/ItemBase.h"
#include "Player/PlayerChar.h"
#include "Userinterface/Inventory/InventoryComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickUp(false)
{
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickUp = false;
}

UItemBase* UItemBase::CreateItemCopy()
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->AssetData = this->AssetData;
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, NumericData.bIsStackable ? NumericData.MaxStackSize : 1); // -> if? else: short variant of it

		if (OwningInventory)
		{
			if (Quantity <= 0)
			{
				OwningInventory->RemoveSingleInstanceOfItem(this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemBase OwningInventory was null (item may be a pickup)."));
		}
	}
}


void UItemBase::Use(APlayerChar* Character)
{
}

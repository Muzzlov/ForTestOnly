#include "Userinterface/Inventory/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "World/PickUp.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Вызывается при старте игры
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Поиск точного совпадения предмета в инвентаре
UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

// Поиск следующего предмета с таким же ID
UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

// Поиск незаполненного стака
UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem)
			{
				return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
			}
		))
	{
		return *Result;
	}

	return nullptr;
}

// Расчет количества, которое можно добавить по весу
int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

// Расчет количества для полного стака
int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

// Удаление одной единицы предмета
void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

// Удаление определенного количества предмета
int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);
	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

// Разделение стака
void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

// Обработка предметов, которые нельзя стакать
FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* InputItem, int32 RequestedAddAmount)
{
	// Проверка валидности веса
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNon(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."),
			InputItem->TextData.Name));
	}

	// Проверка переполнения по весу
	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNon(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."),
			InputItem->TextData.Name));
	}

	// Проверка переполнения по слотам
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNon(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."),
			InputItem->TextData.Name));
	}

	AddNewItem(InputItem, 1);

	return FItemAddResult::AddedAll(1, FText::Format(
		FText::FromString("Successfully added a single {0} to the inventory."),
		InputItem->TextData.Name));
}


int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		//invalid item data
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;

	// check if the input item already exists in the inventory and is not a full stack
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn);

	// distribut item stack over existing stacks
	while (ExistingItemStack)
	{
		// calculate how many of the existing item would be needed to make the next full stack
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		// calculate how many of the AmountToMakeFullStack can actually be carried based on weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		// as long as the remaining amount of the item does not evrflow weight capacity
		if (WeightLimitAddAmount > 0)
		{
			// adjust the existing items stack quantity and inventory total weight
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);

			// adjust the count to be deistributed
			AmountToDistribute -= WeightLimitAddAmount;

			ItemIn->SetQuantity(AmountToDistribute);

			// TODO Refine this logic since going over weight capcity should not ever be possible
			// if max weight capcity is reached, no need to run the loop again
			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				// this block will be reached if distributing an item across multiple stacks
				// and the weight limit is hit during that process
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// reached if there is a partial stack but non of it can be added at all
			return 0;
		}
		if (AmountToDistribute <= 0)
		{
			// all of the input item was distributed across existing stacks
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}
		// check if there is still another valid partial stack of the input item
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}

	// no more partial stacks found, check if a new stack can be added
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// attempt to add as many from the remaining item quantity that can fi inventory weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// if there is still more item to distribute, but weight limit has been reached
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				// adjust the input item and add a new stack with as many as can be held
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				// create a copy since only a partial stack is beaing added
				AddNewItem(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// otherwise, the full remainder of the stack can be added
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}
		// reached if there is free item slots, but no remaining weight capacity
		// TODO: maybe OnInventoryUpdated.Broadcast() should be added.
		// OnInventoryUpdated.Broadcast();
		return RequestedAddAmount - AmountToDistribute;
	}
	// can only be reached if there is no existing stack and no extra capacity slots
	return 0;
}

// Основная функция добавления предметов
FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		// Обработка стекуемых предметов
		if (!InputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem, InitialRequestedAddAmount);
		}

		// Обработка не стекуемых предметов
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				FText::AsNumber(InitialRequestedAddAmount),
				InputItem->TextData.Name));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedAll(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				InputItem->TextData.Name,
				FText::AsNumber(StackableAmountAdded)));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNon(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid items."),
				InputItem->TextData.Name));
		}
	}

	check(false);
	return FItemAddResult::AddedNon(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));
}

// Добавление нового предмета в инвентарь
void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickUp)
	{
		// Если это уже копия или предмет из мира
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// Создание копии, если предмет переносится из другого инвентаря
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();

	OnInventoryUpdated.Broadcast();
}

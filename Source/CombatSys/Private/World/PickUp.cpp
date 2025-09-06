// Fill out your copyright notice in the Description page of Project Settings.


#include "World/PickUp.h"
#include "Items/ItemBase.h"
#include "Player/PlayerChar.h"
#include "UserInterface/Inventory/InventoryComponent.h"
#include "Engine/DataTable.h"

// Sets default values
APickUp::APickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickUpMesh");
	PickUpMesh->SetSimulatePhysics(true);
	SetRootComponent(PickUpMesh);

}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();

	InitializePickUp(UItemBase::StaticClass(), ItemQuantity);

}


void APickUp::InitializePickUp(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemsData* ItemData = ItemDataTable->FindRow<FItemsData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);

		ItemReference->ID = ItemData->ID;
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->ItemQuality = ItemData->ItemQuality;
		ItemReference->NumericData = ItemData->NumericData;
		ItemReference->TextData = ItemData->TextData;
		ItemReference->AssetData = ItemData->AssetData;

		ItemReference->NumericData.bIsStackable = (ItemData->NumericData.MaxStackSize > 1) ? true : false;
		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickUpMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickUp::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;

	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	ItemReference->OwningInventory = nullptr;
	PickUpMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

	UpdateInteractableData();
}


void APickUp::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::PickUp;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickUp::BeginFocus()
{
	if (PickUpMesh)
	{
		PickUpMesh->SetRenderCustomDepth(true);
	}
}

void APickUp::EndFocus()
{
	if (PickUpMesh)
	{
		PickUpMesh->SetRenderCustomDepth(false);
	}
}


void APickUp::Interact(APlayerChar* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickUp(PlayerCharacter);
	}
}


void APickUp::TakePickUp(const APlayerChar* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					break;
				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				}

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null!"));
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was somehow null!"));
		}
	}
}

void APickUp::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickUp, DesiredItemID))
	{
		if (ItemDataTable)
		{
			const FString ContextString{ DesiredItemID.ToString() };

			if (const FItemsData* ItemData = ItemDataTable->FindRow<FItemsData>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickUpMesh->SetStaticMesh(ItemData->AssetData.Mesh);
			}
		}
	}
}

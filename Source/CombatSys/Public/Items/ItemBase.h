#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/ItemDataStructs.h"
#include "Player/PlayerChar.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

UCLASS()
class COMBATSYS_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	//=================================================================================
	//   PROPERTIES & VARIABLES
	//=================================================================================
	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemQuality ItemQuality;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData TextData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData NumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData AssetData;

	bool bIsCopy;
	bool bIsPickUp;

	//===================================================================================
	//   FUNCTION
	//===================================================================================
	UItemBase();

	void ResetItemFlags();

	UItemBase* CreateItemCopy();

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemStackWeight() const { return Quantity * NumericData.Weight; };

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return NumericData.Weight; };

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == NumericData.MaxStackSize; };

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(Category = "Item")
	virtual void Use(APlayerChar* Character);

protected:

	//used for search for items in TArray
	bool operator==(const FName& OtherID) const
	{
		return this->ID == OtherID;
	}

	//=================================================================================
	//   PROPERTIES & VARIABLES
	//=================================================================================



	//===================================================================================
	//   FUNCTION
	//===================================================================================
};

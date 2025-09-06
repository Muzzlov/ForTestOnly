#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8
{
	Shoddy UMETA(DisplayName = "Shoddy"),
	Cheap UMETA(DisplayName = "Cheap"),
	Quality UMETA(DisplayName = "Quality"),
	Masterwork UMETA(DisplayName = "Masterwork"),
	Grandmaster UMETA(DisplayName = "Grandmaster")
};

UENUM()
enum class EItemType : uint8
{
	Armor UMETA(DisplayName = "Armor"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Spell UMETA(DisplayName = "Spell"),
	Consumable UMETA(DisplayName = "Consumable"),
	QuestItem UMETA(DisplayName = "QuestItem"),
	Mundane UMETA(DisplayName = "Mundane")
};

USTRUCT()
struct FItemStatistics
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float ArmorRating;

	UPROPERTY(EditAnywhere)
	float DamageValue;

	UPROPERTY(EditAnywhere)
	float RestorationAmount;

	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT()
struct FItemTextData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	FText UsageText;
};


USTRUCT()
struct FItemNumericData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;

};

USTRUCT()
struct FItemAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
};

USTRUCT()
struct FItemsData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Items Data")
	FName ID;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Items Data")
	FItemAssetData AssetData;
};

class COMBATSYS_API ItemDataStructs
{
public:




};

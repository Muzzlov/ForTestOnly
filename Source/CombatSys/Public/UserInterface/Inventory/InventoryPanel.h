#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

// Вперёд объявляем типы, которые используем как указатели в заголовке
class UWrapBox;
class UTextBlock;
class UInventoryItemSlot;
class UInventoryComponent;
class UDragDropOperation;
class APlayerChar;

UCLASS()
class COMBATSYS_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	// Метод, который дергаем при обновлении инвентаря
	UFUNCTION()
	void RefreshInventory();

	// Виджеты, привязываемые в Blueprint через BindWidget
	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	// Ссылки на игрока и его инвентарь
	UPROPERTY()
	APlayerChar* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	// Класс виджета слота предмета
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

protected:
	// Обновление текстовой информации о весе и количестве слотов
	void SetInfoText() const;

	// Инициализация виджета
	virtual void NativeOnInitialized() override;

	// Обработка drag and drop
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};

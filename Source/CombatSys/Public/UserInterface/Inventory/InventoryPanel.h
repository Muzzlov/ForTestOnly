#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

// ����� ��������� ����, ������� ���������� ��� ��������� � ���������
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
	// �����, ������� ������� ��� ���������� ���������
	UFUNCTION()
	void RefreshInventory();

	// �������, ������������� � Blueprint ����� BindWidget
	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	// ������ �� ������ � ��� ���������
	UPROPERTY()
	APlayerChar* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	// ����� ������� ����� ��������
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

protected:
	// ���������� ��������� ���������� � ���� � ���������� ������
	void SetInfoText() const;

	// ������������� �������
	virtual void NativeOnInitialized() override;

	// ��������� drag and drop
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include <Components/WidgetInteractionComponent.h>
#include "Interfaces/InteractionInterface.h"
#include "Math/Transform.h"
#include "Math/Color.h"
#include "EnhancedInputLibrary.h"
#include "InputActionValue.h"
#include "PlayerChar.generated.h"


// Вперёд объявляем классы, чтобы не включать лишние заголовки
class USpringArmComponent;
class UCameraComponent;
class AWeaponStatic;
class USphereComponent;
class UInventoryComponent;
class AGameHUD;
class UItemBase;
class UCurveFloat;
class UTimelineComponent;
class UInputMappingContext;
class AMalkGamePlayerController;
class UInputAction;

//struct для взаимодействий с предметами
USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

		FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};


UCLASS()
class COMBATSYS_API APlayerChar : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerChar();


		/** ========== КАМЕРА ========== */
	bool bAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* ViewCamera;

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	void UpdateInteractionWidget() const;

	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);

protected:

	/* ===================== Взаимодействие с предметами ================== */
	
	UPROPERTY()
	AGameHUD* HUD;
	
	UPROPERTY(VisibleAnyWhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;
	
	UPROPERTY(VisibleAnywhere, Category="Character | Inventory")
	UInventoryComponent* PlayerInventory;

	float InteractionCheckFrequency;

	float InteractionCheckDistance;

	FTimerHandle TimerHandle_Interaction;

	FInteractionData InteractionData;

	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();

	/** ========== ЖИЗНЕННЫЙ ЦИКЛ ========== */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ToggleMenu();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Aim();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopAiming();
	UFUNCTION()
	void UpdateCameraTimeline(const float TimelineValue) const;
	UFUNCTION()
	void CameraTimelineEnd() const;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter | Aiming")
	FVector DefaultCameraLocation;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter | Aiming")
	FVector AimingCameraLocation;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter | Aiming")
	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter | Aiming")
	TObjectPtr<UCurveFloat> AimingCameraCurve;

	/** ========== ДВИЖЕНИЕ ========== */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void StartSprint();
	void StopSprint();
	void ToggleWalk();
	void UpdateSpeed(float DeltaTime);
	float GetBaseSpeed() const;

	// Скорости
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 230.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float JogSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 800.f;

	// Время разгона до спринта
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintAccelerationTime = 2.f;

	// Внутренние переменные
	bool bSprintPressed = false;
	bool bWalkToggled = false;
	float ForwardInput = 0.f;
	float RightInput = 0.f;
	float SprintHoldTime = 0.f;
};

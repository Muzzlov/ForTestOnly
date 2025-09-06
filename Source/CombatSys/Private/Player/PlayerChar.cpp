#include "Player/PlayerChar.h"
#include "Items/ItemBase.h"
#include "Items/Weapon/WeaponStatic.h"
#include "UserInterface/GameHUD.h"
#include "UserInterface/Inventory/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "World/PickUp.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MalkGamePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TimelineComponent.h"


APlayerChar::APlayerChar()
{
	PrimaryActorTick.bCanEverTick = true;

	// ========== Настройки поворота ==========
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Ориентация по направлению движения
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 550.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;

	// ========== Камера ==========
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);


	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 225.0f;

	/*=========== Инвентарь =============*/
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.0f);

	AimingCameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimingCameraTimeline"));
	DefaultCameraLocation = FVector{ 0.0f, 0.0f, 65.0f };
	AimingCameraLocation = FVector{ 175.0f, 50.0f, 55.0f };
	CameraBoom->SocketOffset = DefaultCameraLocation;
}



void APlayerChar::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;

	HUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

}

void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSpeed(DeltaTime);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}



/* ================ Ф-ции Взаимодействия с Предметами =============== */
void APlayerChar::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{GetPawnViewLocation()};
	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};
	

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());
	
	if (LookDirection > 0)
	{
		//debug
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	NoInteractableFound();
}

void APlayerChar::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void APlayerChar::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void APlayerChar::BeginInteract()
{
	// verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&APlayerChar::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void APlayerChar::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void APlayerChar::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void APlayerChar::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void APlayerChar::ToggleMenu()
{
	HUD->ToggleMenu();
}

void APlayerChar::Aim()
{
	if (!HUD->bIsMenuVisible)
	{
		bAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 100.0f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->PlayFromStart();
	}
}

void APlayerChar::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		bUseControllerRotationYaw = false;
		HUD->HideCrosshair();
		GetCharacterMovement()->MaxWalkSpeed = 230.0f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->Reverse();
	}

}

void APlayerChar::UpdateCameraTimeline(float TimelineValue) const
{
	const FVector CameraLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, TimelineValue);
	CameraBoom->SocketOffset = CameraLocation;
}

void APlayerChar::CameraTimelineEnd() const
{
	if (AimingCameraTimeline)
	{
		if (AimingCameraTimeline->GetPlaybackPosition() != 0.0f)
		{
			HUD->ShowCrosshair();
		}
	}
}

void APlayerChar::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickUp* PickUp = GetWorld()->SpawnActor<APickUp>(APickUp::StaticClass(), SpawnTransform, SpawnParams);

		PickUp->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}

/*
void APlayerChar::Move(const FInputActionValue& Value)
{
}

void APlayerChar::Look(const FInputActionValue& Value)
{
}
*/


// =============== Конец Ф-ций Взаимодействия ==================

void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ========== Привязка инпута ==========
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerChar::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerChar::StopSprint);
	PlayerInputComponent->BindAction("ToggleWalk", IE_Pressed, this, &APlayerChar::ToggleWalk);

	// Нажатие на взаимодействие — пробуем подобрать ближайший предмет
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &APlayerChar::EndInteract);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerChar::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerChar::StopAiming);

	PlayerInputComponent->BindAction("ToggleMenu", IE_Pressed, this, &APlayerChar::ToggleMenu);


	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("TurnCamera", this, &APlayerChar::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::LookUp);
}



// ==================== Движение ====================
void APlayerChar::MoveForward(float Value)
{
	ForwardInput = Value;
	if (Controller && (FMath::Abs(Value) > KINDA_SMALL_NUMBER))
	{
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerChar::MoveRight(float Value)
{
	RightInput = Value;
	if (Controller && (FMath::Abs(Value) > KINDA_SMALL_NUMBER))
	{
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerChar::Turn(float Value) { AddControllerYawInput(Value); }
void APlayerChar::LookUp(float Value) { AddControllerPitchInput(Value); }

void APlayerChar::StartSprint() { bSprintPressed = true; SprintHoldTime = 0.f; }

void APlayerChar::StopSprint()
{
	bSprintPressed = false;
	SprintHoldTime = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = GetBaseSpeed();
}

void APlayerChar::ToggleWalk()
{
	bWalkToggled = !bWalkToggled;
	if (!bSprintPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetBaseSpeed();
	}
}

float APlayerChar::GetBaseSpeed() const
{
	return bWalkToggled ? WalkSpeed : JogSpeed;
}

void APlayerChar::UpdateSpeed(float DeltaTime)
{
	const bool bMoving = FMath::Abs(ForwardInput) > KINDA_SMALL_NUMBER || FMath::Abs(RightInput) > KINDA_SMALL_NUMBER;
	auto* MoveComp = GetCharacterMovement();

	if (bMoving && bSprintPressed)
	{
		SprintHoldTime = FMath::Min(SprintHoldTime + DeltaTime, SprintAccelerationTime);
		const float Alpha = SprintAccelerationTime > 0.f ? SprintHoldTime / SprintAccelerationTime : 1.f;
		MoveComp->MaxWalkSpeed = FMath::Lerp(GetBaseSpeed(), SprintSpeed, Alpha);
	}
	else
	{
		MoveComp->MaxWalkSpeed = bSprintPressed ? SprintSpeed : GetBaseSpeed();
		SprintHoldTime = 0.f;
	}
}

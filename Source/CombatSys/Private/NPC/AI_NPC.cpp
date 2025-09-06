#include "NPC/AI_NPC.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAI_NPC::AAI_NPC()
{
    PrimaryActorTick.bCanEverTick = true;

    // Базовые настройки поворота/скорости
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UCharacterMovementComponent* Move = GetCharacterMovement();
    Move->bOrientRotationToMovement = true;                // при бродилке поворачиваемся по движению
    Move->RotationRate = FRotator(0.f, 500.f, 0.f);
    Move->MaxWalkSpeed = WalkSpeed;

    // Простой AI-контроллер
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();

    // Маркер «небоевой»
    Tags.Add(FName("NoCombat"));
}

void AAI_NPC::BeginPlay()
{
    Super::BeginPlay();

    CachedAI = Cast<AAIController>(GetController());
    CachedPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    // Мирный — урон не получает
    SetCanBeDamaged(false);

    // Если стоим на месте — поворот берём на себя (для LookAt)
    if (!bWanderEnabled)
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }

    if (bWanderEnabled)
    {
        StartWandering();
    }
}

void AAI_NPC::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Смотрим на игрока только когда НЕ бродим
    if (!bWanderEnabled && bLookAtPlayerWhenNear)
    {
        UpdateLookAt(DeltaSeconds);
    }
}

void AAI_NPC::SetWanderEnabled(bool bEnable)
{
    if (bEnable) StartWandering();
    else         StopWandering();
}

void AAI_NPC::StartWandering()
{
    bWanderEnabled = true;

    // Управление поворотом отдаём движению
    GetCharacterMovement()->bOrientRotationToMovement = true;

    // Сбрасываем таймер и сразу планируем первую точку
    GetWorldTimerManager().ClearTimer(WanderTimerHandle);
    const float FirstDelay = FMath::FRandRange(0.1f, 0.5f);
    GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &AAI_NPC::PickNextLocation, FirstDelay, false);
}

void AAI_NPC::StopWandering()
{
    bWanderEnabled = false;

    // Поворот берём на себя (для LookAt)
    GetCharacterMovement()->bOrientRotationToMovement = false;

    GetWorldTimerManager().ClearTimer(WanderTimerHandle);

    if (!CachedAI) { CachedAI = Cast<AAIController>(GetController()); }
    if (CachedAI)
    {
        CachedAI->StopMovement();
    }
}

void AAI_NPC::PickNextLocation()
{
    if (!bWanderEnabled) return;

    if (!CachedAI) { CachedAI = Cast<AAIController>(GetController()); }
    if (!CachedAI) { return; }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) { return; }

    FNavLocation Result;
    const FVector Origin = GetActorLocation();

    if (NavSys->GetRandomReachablePointInRadius(Origin, WanderRadius, Result))
    {
        CachedAI->MoveToLocation(Result.Location, AcceptanceRadius, /*bStopOnOverlap*/true);
    }

    if (bWanderEnabled)
    {
        const float Wait = FMath::FRandRange(WaitTimeMin, WaitTimeMax);
        GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &AAI_NPC::PickNextLocation, Wait, false);
    }
}

void AAI_NPC::UpdateLookAt(float DeltaSeconds)
{
    if (!CachedPlayerPawn)
    {
        CachedPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
        if (!CachedPlayerPawn) return;
    }

    const FVector MyLoc = GetActorLocation();
    const FVector Target = CachedPlayerPawn->GetActorLocation();

    // Проверяем дистанцию по горизонту
    const float DistSq2D = FVector::DistSquared2D(MyLoc, Target);
    if (DistSq2D > LookAtRadius * LookAtRadius)
    {
        return; // далеко — не крутимся
    }

    FVector ToPlayer = Target - MyLoc;
    ToPlayer.Z = 0.f; // вращаемся по плоскости
    if (ToPlayer.IsNearlyZero()) return;

    FRotator Desired = ToPlayer.Rotation();
    if (bLookAtYawOnly)
    {
        Desired.Pitch = 0.f;
        Desired.Roll = 0.f;
    }

    const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Desired, DeltaSeconds, LookAtInterpSpeed);
    SetActorRotation(NewRot);
}

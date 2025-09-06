#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_NPC.generated.h"

UCLASS()
class COMBATSYS_API AAI_NPC : public ACharacter
{
    GENERATED_BODY()

public:
    AAI_NPC();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // ---- Движение ----
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float WalkSpeed = 200.f;

    // ---- Бродилка (переключаемая) ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Wander")
    bool bWanderEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Wander", meta = (EditCondition = "bWanderEnabled"))
    float WanderRadius = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Wander", meta = (EditCondition = "bWanderEnabled"))
    float WaitTimeMin = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Wander", meta = (EditCondition = "bWanderEnabled"))
    float WaitTimeMax = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Wander")
    float AcceptanceRadius = 60.f;

    UFUNCTION()
    void PickNextLocation();

    // --- Look-at игрока для стоячих NPC ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|LookAt")
    bool bLookAtPlayerWhenNear = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|LookAt", meta = (EditCondition = "bLookAtPlayerWhenNear"))
    float LookAtRadius = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|LookAt", meta = (EditCondition = "bLookAtPlayerWhenNear"))
    float LookAtInterpSpeed = 5.f;

    // Только вокруг вертикали (Yaw), без кивки/кренов
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|LookAt", meta = (EditCondition = "bLookAtPlayerWhenNear"))
    bool bLookAtYawOnly = true;

    void UpdateLookAt(float DeltaSeconds);

    UPROPERTY()
    APawn* CachedPlayerPawn = nullptr;


    FTimerHandle WanderTimerHandle;

    UPROPERTY()
    class AAIController* CachedAI = nullptr;

public:
    // --- Вкл/выкл из BP или кода ---
    UFUNCTION(BlueprintCallable, Category = "AI|Wander")
    void SetWanderEnabled(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "AI|Wander")
    void StartWandering();

    UFUNCTION(BlueprintCallable, Category = "AI|Wander")
    void StopWandering();

    UFUNCTION(BlueprintPure, Category = "AI|Wander")
    bool IsWanderEnabled() const { return bWanderEnabled; }
};

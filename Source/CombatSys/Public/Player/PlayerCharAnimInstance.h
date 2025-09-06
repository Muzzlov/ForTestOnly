// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharAnimInstance.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class COMBATSYS_API UPlayerCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;


	UPROPERTY(BlueprintReadOnly)
	class APlayerChar* PlayerChar = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerCharMovement = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling = false;

};

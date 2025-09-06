// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharAnimInstance.h"
#include "Player/PlayerChar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"



void UPlayerCharAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerChar = Cast<APlayerChar>(TryGetPawnOwner());
	if (PlayerChar)
	{
		PlayerCharMovement = PlayerChar->GetCharacterMovement();
	}
}

void UPlayerCharAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!PlayerChar) 
    {
        PlayerChar = Cast<APlayerChar>(TryGetPawnOwner());
        if (PlayerChar) { PlayerCharMovement = PlayerChar->GetCharacterMovement(); }
    }

    if (APawn* OwningPawn = TryGetPawnOwner()) 
    {
        const FVector V = OwningPawn->GetVelocity();
        GroundSpeed = FVector(V.X, V.Y, 0.f).Size();
    }
    else {
        GroundSpeed = 0.f;
    }

    IsFalling = (PlayerCharMovement ? PlayerCharMovement->IsFalling() : false);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerChar.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "InterfaceTestActor.generated.h"

UCLASS()
class COMBATSYS_API AInterfaceTestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInterfaceTestActor();

protected:
	UPROPERTY(EditAnywhere, Category = "Test Actor")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditInstanceOnly, Category = "Test Actor")
	FInteractableData InstanceInteractableData;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(APlayerChar* PlayerChar) override;
};

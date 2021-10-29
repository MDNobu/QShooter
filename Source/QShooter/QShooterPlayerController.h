// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API AQShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AQShooterPlayerController();

protected:
	void BeginPlay() override;

private:

	/**  HUDOverlay widget class */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UUserWidget* HUDOverlay;



};

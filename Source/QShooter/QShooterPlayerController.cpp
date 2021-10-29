// Fill out your copyright notice in the Description page of Project Settings.


#include "QShooterPlayerController.h"
#include "Blueprint/UserWidget.h"

AQShooterPlayerController::AQShooterPlayerController()
{

}

void AQShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

#pragma region SpawnAndShowHUD
	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		ensure(HUDOverlay);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
#pragma endregion

}

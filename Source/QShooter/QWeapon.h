// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QItem.h"
#include "QWeapon.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API AQWeapon : public AQItem
{
	GENERATED_BODY()
public:
	AQWeapon();

	
public:

	void SetToEquipped();

	/** 注意这个方法必须在item已经设置为falling状态时才能调用 */
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; };

	bool HasAmmo();
	void FireOneBullet();
protected:

private:
	void StopFalling();
private:


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float FallingDuration = 2.0f;

	FTimerHandle FallingStopTimerHandle;

	bool bIsFalling = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 AmmoAmount = 30;

public:

};

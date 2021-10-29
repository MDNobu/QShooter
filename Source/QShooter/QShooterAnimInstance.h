// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API UQShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void UpdateAnimProperties(float deltatTime);

private:

	class AQShooterCharacter* ShooterCharacterCache;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsAccelerate = false;

	// AimMovementDeltaYaw是 角色动画strafe blending的参数
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimMovementDeltaYaw = 0.0f;


	// 这是 aim 和 movement方向差值的cache，只在bIsAccelerate时更新， 目的是为了 jog stop动画blending用
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float LastAimMovementOffsetYawCache = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsAimming = false;
};

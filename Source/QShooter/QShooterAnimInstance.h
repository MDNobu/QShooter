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

	// AimMovementDeltaYaw�� ��ɫ����strafe blending�Ĳ���
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimMovementDeltaYaw = 0.0f;


	// ���� aim �� movement�����ֵ��cache��ֻ��bIsAccelerateʱ���£� Ŀ����Ϊ�� jog stop����blending��
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float LastAimMovementOffsetYawCache = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsAimming = false;
};

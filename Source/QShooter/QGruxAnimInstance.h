// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QGruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API UQGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void UpdateAnimProperties(float deltatTime);


	void NativeInitializeAnimation() override;

private:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class AQEnemy* OwningEnemy = nullptr;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "QGruxAnimInstance.h"
#include "QEnemy.h"

void UQGruxAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningEnemy = Cast<AQEnemy>(TryGetPawnOwner());
}

void UQGruxAnimInstance::UpdateAnimProperties(float deltatTime)
{
	if (nullptr == OwningEnemy)
	{
		OwningEnemy = Cast<AQEnemy>(TryGetPawnOwner());
	}
	if (nullptr == OwningEnemy)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s animation instance should have a AQEnemy Owner"), *GetName());
		return;
	}

	FVector velocity = OwningEnemy->GetVelocity();
	velocity.Z = 0; //忽略上下的速度
	Speed = velocity.Size();
}



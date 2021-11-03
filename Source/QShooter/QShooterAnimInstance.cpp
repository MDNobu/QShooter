// Fill out your copyright notice in the Description page of Project Settings.


#include "QShooterAnimInstance.h"
#include "QShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UQShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacterCache = Cast<AQShooterCharacter>(TryGetPawnOwner());
}

void UQShooterAnimInstance::UpdateAnimProperties(float deltatTime)
{
	if (nullptr == ShooterCharacterCache)
	{
		ShooterCharacterCache = Cast<AQShooterCharacter>(TryGetPawnOwner());
	}
	ensure(ShooterCharacterCache);
	if (ShooterCharacterCache)
	{
		FVector velocity = ShooterCharacterCache->GetVelocity();
		velocity.Z = 0;

		Speed = velocity.Size();
		bIsInAir = ShooterCharacterCache->GetCharacterMovement()->IsFalling();
		bIsAccelerate = ShooterCharacterCache->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;


		const FRotator& aimRotator = ShooterCharacterCache->GetBaseAimRotation();
		const FRotator& moveRotator = UKismetMathLibrary::MakeRotFromX(ShooterCharacterCache->GetVelocity());
		const FRotator& deltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(moveRotator, aimRotator);
		//deltaRotator
		AimMovementDeltaYaw = deltaRotator.Yaw;

		//UKismetMathLibrary::Rotat
	/*	FString debugMsg = FString::Printf(TEXT("aim and movement delta yaw: %f"), AimMovementDeltaYaw);
		GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, debugMsg);*/

		if (bIsAccelerate)
		{
			LastAimMovementOffsetYawCache = AimMovementDeltaYaw;
		}

		bIsAimming = ShooterCharacterCache->GetIsAimming();

		AimPitch = ShooterCharacterCache->GetBaseAimRotation().Pitch;

		bIsReloading = ShooterCharacterCache->GetCombatState() == ECombatState::ECS_Reloading;
	}

	TurnInPlace();
}

void UQShooterAnimInstance::TurnInPlace()
{
	if (nullptr == ShooterCharacterCache)
		return;

	if (Speed > 0)// Speed > 0，即角色在移动时，不进行in place转身动画
	{
		RotationCurveValueLastFrame = RotationCuveValue;
		CharacterYaw = ShooterCharacterCache->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		//return;
		RootYawOffset = 0;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacterCache->GetActorRotation().Yaw;
		const float deltaYaw = CharacterYaw - CharacterYawLastFrame;

		//RootYawOffset -= deltaYaw;
		// NormalizeAxis可以相当于clamp to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - deltaYaw);

		// 读取curve数据，只有当turning 为1时，rotation 数据才有效
		float turning = GetCurveValue(FName(TEXT("Turning")));
		if (turning > 0)
		{
			RotationCurveValueLastFrame = RotationCuveValue;
			RotationCuveValue = GetCurveValue(TEXT("Rotation"));
			const float deltaRotation = RotationCuveValue - RotationCurveValueLastFrame;
			RootYawOffset = RootYawOffset > 0 ? RootYawOffset - deltaRotation : RootYawOffset + deltaRotation;
			RootYawOffset = UKismetMathLibrary::ClampAngle(RootYawOffset, -90.0f, 90.0f);
		}
	}
	

	GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue,
		FString::Printf(TEXT("CharacterYaw : %f"), CharacterYaw));
	GEngine->AddOnScreenDebugMessage(2, -1, FColor::Blue,
		FString::Printf(TEXT("RootYawOffset : %f"), RootYawOffset));
}


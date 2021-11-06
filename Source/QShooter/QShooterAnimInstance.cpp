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
	if (!ShooterCharacterCache)
	{
		return;
	}
	FVector velocity = ShooterCharacterCache->GetVelocity();
	velocity.Z = 0;

	Speed = velocity.Size();
	bIsInAir = ShooterCharacterCache->GetCharacterMovement()->IsFalling();
	bIsAccelerate = ShooterCharacterCache->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;

#pragma region UpdateAimMovementDeltaYaw
	const FRotator& aimRotator = ShooterCharacterCache->GetBaseAimRotation();
	const FRotator& moveRotator = UKismetMathLibrary::MakeRotFromX(ShooterCharacterCache->GetVelocity());
	const FRotator& deltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(moveRotator, aimRotator);
	//deltaRotator
	AimMovementDeltaYaw = deltaRotator.Yaw;
	if (bIsAccelerate)
	{
		LastAimMovementOffsetYawCache = AimMovementDeltaYaw;
	}
#pragma endregion

	bIsAimming = ShooterCharacterCache->GetIsAimming();

	AimPitch = ShooterCharacterCache->GetBaseAimRotation().Pitch;

	bIsReloading = ShooterCharacterCache->GetCombatState() == ECombatState::ECS_Reloading;
	bIsEquipping = ShooterCharacterCache->GetCombatState() == ECombatState::ECS_Equipping;

#pragma region SetAimOffsetState
	if (bIsReloading)
	{
		AimOffsetState = EAimOffsetState::EAOS_Reloading;
	} else if (bIsInAir)
	{
		AimOffsetState = EAimOffsetState::EAOS_InAir;
	} else if (bIsAimming)
	{
		AimOffsetState = EAimOffsetState::EAOS_Aiming;
	}
	else
	{
		AimOffsetState = EAimOffsetState::EAOS_Hip;
	}
#pragma endregion

	bIsCrouch = ShooterCharacterCache->GetIsCrouching();

	bool isTurning = TurnInPlace();
	Lean(deltatTime);


#pragma region UpdateRecoilWeight
	RecoilWeight = 0.9f;// Reset to default
	if (isTurning)
	{
		RecoilWeight = 0.1f;
	}
	else if (bIsAimming)
	{
		RecoilWeight = 1.0f;
	} else if (bIsCrouch)
	{
		RecoilWeight = 0.5f;
	} 
#pragma endregion
}

bool UQShooterAnimInstance::TurnInPlace()
{
	bool isTurning = false;
	if (nullptr == ShooterCharacterCache)
		return isTurning;

	if (Speed > 0 || bIsInAir)// Speed > 0，即角色在移动时，不进行in place转身动画
	{
		RotationCurveValueLastFrame = RotationCuveValue;
		CharacterYaw_TIP = ShooterCharacterCache->GetActorRotation().Yaw;
		CharacterYawLastFrame_TIP = CharacterYaw_TIP;
		//return;
		RootYawOffset = 0;
	}
	else
	{
		CharacterYawLastFrame_TIP = CharacterYaw_TIP;
		CharacterYaw_TIP = ShooterCharacterCache->GetActorRotation().Yaw;
		const float deltaYaw = CharacterYaw_TIP - CharacterYawLastFrame_TIP;

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
		isTurning = turning > 0;
	}
	//GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue,
	//	FString::Printf(TEXT("RootYawOffset : %f"), RootYawOffset));
	return isTurning;
}

void UQShooterAnimInstance::Lean(float deltatTime)
{
	if (nullptr == ShooterCharacterCache) 
		return;

	CharacterYawLastFrame_Lean = CharacterYaw_Lean;
	CharacterYaw_Lean = ShooterCharacterCache->GetActorRotation().Yaw;


	
	/**  */
	const float turnRate = UKismetMathLibrary::NormalizeAxis(CharacterYaw_Lean - CharacterYawLastFrame_Lean) / deltatTime;
	const float interp = FMath::FInterpTo(YawOffset_Lean, turnRate, deltatTime, 6.0f);

	//YawOffset_Lean = CharacterYaw_Lean - CharacterYawLastFrame_Lean;
	YawOffset_Lean = FMath::Clamp(interp, -90.0f, 90.0f);
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red,
	//		FString::Printf(TEXT("YawOffset_Lean : %f"), YawOffset_Lean));
	//	GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red,
	//		FString::Printf(TEXT("YawOffset : %f"), CharacterYaw_Lean - CharacterYawLastFrame_Lean));
	//}

}


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
		FString debugMsg = FString::Printf(TEXT("aim and movement delta yaw: %f"), AimMovementDeltaYaw);
		GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, debugMsg);

		if (bIsAccelerate)
		{
			LastAimMovementOffsetYawCache = AimMovementDeltaYaw;
		}

		bIsAimming = ShooterCharacterCache->GetIsAimming();
	}
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QShooterAnimInstance.generated.h"

/** AimOffset state 主要是为了表示当前动画aim offset的状态，以在ABP中用不同的动画 */
UENUM(BlueprintType)
enum class EAimOffsetState : uint8
{
	EAOS_Reloading UMETA(DisplayName = "Reloading"),  //不用AimOffset动画
	EAOS_Aiming UMETA(DisplayName = "Aiming"),  //aim状态，用aimOffset_Aim 动画
	EAOS_InAir UMETA(DisplayName = "InAir"),  //在地上的普通状态，用AimOffset_Hip动画，但只有pitch输入
	EAOS_Hip UMETA(DisplayName = "Hip"),  //在地上的普通状态，用AimOffset_Hip动画

	EAOS_MAX  UMETA(DisplayName = "MAX", Hidden)
};


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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class AQShooterCharacter* ShooterCharacterCache;

	/** 返回值表示当前是否在转身*/
	bool TurnInPlace();
	void Lean(float deltatTime);
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimPitch = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsEquipping = false;
#pragma region TurnParams
	/** TIP指的是turn in place， */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CharacterYawLastFrame_TIP = 0;
	/** TIP指的是turn in place， */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CharacterYaw_TIP = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float RootYawOffset = 0.0f;

	float RotationCuveValue = 0.0f;
	float RotationCurveValueLastFrame = 0.0f;
#pragma endregion


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EAimOffsetState AimOffsetState = EAimOffsetState::EAOS_Hip;

#pragma region LeanParams

	float CharacterYaw_Lean = 0;
	float CharacterYawLastFrame_Lean = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float YawOffset_Lean = 0;
#pragma endregion

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsCrouch = false;

	/**  后坐力比重，主要用来控制射击时的射击动画（包括了后坐力造成的肩膀抖动）的比例 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float RecoilWeight = 1.0f;

};

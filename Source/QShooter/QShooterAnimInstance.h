// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QShooterAnimInstance.generated.h"

/** AimOffset state ��Ҫ��Ϊ�˱�ʾ��ǰ����aim offset��״̬������ABP���ò�ͬ�Ķ��� */
UENUM(BlueprintType)
enum class EAimOffsetState : uint8
{
	EAOS_Reloading UMETA(DisplayName = "Reloading"),  //����AimOffset����
	EAOS_Aiming UMETA(DisplayName = "Aiming"),  //aim״̬����aimOffset_Aim ����
	EAOS_InAir UMETA(DisplayName = "InAir"),  //�ڵ��ϵ���ͨ״̬����AimOffset_Hip��������ֻ��pitch����
	EAOS_Hip UMETA(DisplayName = "Hip"),  //�ڵ��ϵ���ͨ״̬����AimOffset_Hip����

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

	/** ����ֵ��ʾ��ǰ�Ƿ���ת��*/
	bool TurnInPlace();
	void Lean(float deltatTime);
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimPitch = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsEquipping = false;
#pragma region TurnParams
	/** TIPָ����turn in place�� */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CharacterYawLastFrame_TIP = 0;
	/** TIPָ����turn in place�� */
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

	/**  ���������أ���Ҫ�����������ʱ����������������˺�������ɵļ�򶶶����ı��� */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float RecoilWeight = 1.0f;

};

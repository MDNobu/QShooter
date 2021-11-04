// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QItem.h"
#include "AmmoType.h"
#include "QWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX  UMETA(DisplayName = "MAX", Hidden)
};

/**
 * 
 */
UCLASS()
class QSHOOTER_API AQWeapon : public AQItem
{
	GENERATED_BODY()
public:
	AQWeapon();
	
	
	
	void IncreaseAmmo(int32 ammoDelta);
public:

	void SetToEquipped(class AQShooterCharacter* player);

	/** 注意这个方法必须在item已经设置为falling状态时才能调用 */
	void ThrowWeapon();

	

	bool HasAmmo();
	void FireOneBullet();

	bool IsClipFull() const;
protected:


	void BeginPlay() override;

private:
	void StopFalling();
private:

	/** 装备当前weapon 的player */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQShooterCharacter* PlayerEuipThis = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float FallingDuration = 2.0f;

	FTimerHandle FallingStopTimerHandle;

	bool bIsFalling = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 AmmoAmount = 30;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 MagazineCapcity = 30;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType = EWeaponType::EWT_SubmachineGun;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

	/** Animation section name in AnimMontage, different weapon may use different section name */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName EquipAnimName{ FName(TEXT("Reload_SMG")) };
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName ClipName{ FName(TEXT("smg_clip")) };
	


#pragma region GetterAndSetters
public:
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; };
	FORCEINLINE FName GetEquipAnimName() const { return EquipAnimName; }
	FORCEINLINE int32 GetMagazineCapcity() const { return MagazineCapcity; }

	void SetAmmoAmount(int32 val) { AmmoAmount = val; }

	FORCEINLINE FName GetClipName() const { return ClipName; }
#pragma endregion

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QItem.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "QWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX  UMETA(DisplayName = "MAX", Hidden)
};


USTRUCT(BlueprintType)
struct FWeaponDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 WeaponAmmo = 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MagazineCapcity = 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USoundCue* PickupSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundCue* EquipSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMesh* ItemMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ItemName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* InventoryIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* AmmoIcon;
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

	void OnConstruction(const FTransform& Transform) override;

public:

	void SetToEquipped(class AQShooterCharacter* player);

	/** 注意这个方法必须在item已经设置为falling状态时才能调用 */
	void ThrowItem() override;

	bool HasAmmo();
	void FireOneBullet();

	bool IsClipFull() const;

	UFUNCTION(BlueprintPure, Category = "QShooter")
	UTexture2D* GetAmmoIconTexture();
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
	
	UPROPERTY(EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* AmmoIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UDataTable* WeaponDataTable = nullptr;

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

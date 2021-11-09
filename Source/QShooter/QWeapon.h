// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QItem.h"
#include "AmmoType.h"
#include "WeaponType.h"
#include "Engine/DataTable.h"
#include "QWeapon.generated.h"




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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ClipName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ReloadAnimMontageSection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UAnimInstance> AnimBlueprintClass;

#pragma region CrosshairTextures
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CrosshairMiddleTex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CrosshairRightTex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CrosshairLeftTex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CrosshairUpTex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* CrosshairBottomTex;

#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AutoFireRate = 60.0f/60.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundCue* FireSound;

	/** 这一项存在是因为belicguns这样一个skeleton 里同时有SMG和pistol skeleton的情况 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName BoneToHide;

	/**  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaterialIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsAutomatic = false;
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

	


	void Tick(float DeltaTime) override;

	
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

	void FinishSlide();

	bool IsSlideMovable() const;
	void UpdateSlideDisplacement();
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName Reload_AM_SectionName{ FName(TEXT("Reload_SMG")) };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName ClipName{ FName(TEXT("smg_clip")) };
	
	UPROPERTY(EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* AmmoIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UDataTable* WeaponDataTable = nullptr;



#pragma region CrosshairTextures
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* CrosshairMiddleTex;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* CrosshairRightTex;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* CrosshairLeftTex;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* CrosshairUpTex;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* CrosshairBottomTex;
#pragma endregion


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AutoFireRate = 60.0f / 60.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USoundCue* FireSound;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName BoneToHide;

#pragma region Params4MoveSlide
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float SlideDisplacement = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float RecoilRotation = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UCurveFloat* SlideDisplacmentCurve = nullptr;

	FTimerHandle MoveSlideTimerHandle;

	bool bIsMovingSlide = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float SlideMoveDuration = 0.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float SlideDisplacementMAX = 15.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float RecoilRotationMAX = 25.0f;
#pragma endregion

	bool bIsAutomatic = false;
#pragma region GetterAndSetters
public:
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetAmmoAmount() const { return AmmoAmount; };
	FORCEINLINE	FName GetReload_AM_SectionName() const { return Reload_AM_SectionName; }
	//FORCEINLINE FName GetEquipAnimName() const { return Reload_AM_SectionName; }
	FORCEINLINE int32 GetMagazineCapcity() const { return MagazineCapcity; }

	FORCEINLINE void SetAmmoAmount(int32 val) { AmmoAmount = val; }

	FORCEINLINE FName GetClipName() const { return ClipName; }
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }

	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }
#pragma endregion

};

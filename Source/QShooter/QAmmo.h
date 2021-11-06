// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QItem.h"
#include "AmmoType.h"
#include "QAmmo.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API AQAmmo : public AQItem
{
	GENERATED_BODY()
public:
	AQAmmo();


	void EnableCustomDepth() override;


	void DisableCustomDepth() override;

protected:


	void BeginPlay() override;


	void SetItemProperties(EQItemState targetItemState) override;

private:
#pragma region Components
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* AmmoMesh = nullptr;

	/**  用来触发玩家角色碰到ammo item时，自动collect */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USphereComponent* CollectTrigerSphere = nullptr;
#pragma endregion

	UFUNCTION()
	void OnCollectSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

#pragma region GetterAndSetter
public:
	EAmmoType GetAmmoType() const { return AmmoType; }

#pragma endregion

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* AmmoTexture = nullptr;
};

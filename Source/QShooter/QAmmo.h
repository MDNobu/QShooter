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

protected:


	void BeginPlay() override;


	void SetItemProperties(EQItemState targetItemState) override;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* AmmoMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

#pragma region GetterAndSetter
public:
	EAmmoType GetAmmoType() const { return AmmoType; }

#pragma endregion

};

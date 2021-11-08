// Fill out your copyright notice in the Description page of Project Settings.


#include "QWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "QShooterCharacter.h"

AQWeapon::AQWeapon()
{

}

void AQWeapon::IncreaseAmmo(int32 ammoDelta)
{
	ensureMsgf(ammoDelta >= 0 && ammoDelta <= MagazineCapcity, TEXT("ammoDelta is not legal"));
	AmmoAmount += ammoDelta;
}

void AQWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#pragma region ReadParamFromDataTable
	const FString tablePath(TEXT("DataTable'/Game/_Game/DataTable/DT_Weapon.DT_Weapon'"));
	UDataTable* weaponTable = LoadObject<UDataTable>(nullptr, *tablePath);
	if (weaponTable)
	{
		FWeaponDataTableRow* tableRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			tableRow = weaponTable->FindRow<FWeaponDataTableRow>(TEXT("SubmachineGun"), TEXT(""));
			break;
		case EWeaponType::EWT_AssaultRifle:
			tableRow = weaponTable->FindRow<FWeaponDataTableRow>(TEXT("AssaultRifle"), TEXT(""));
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("weapon %s has a illegal type"), *GetName());
			break;
		}

		if (tableRow)
		{
			AmmoType = tableRow->AmmoType;
			AmmoAmount = tableRow->WeaponAmmo;
			MagazineCapcity = tableRow->MagazineCapcity;
			SetPickupSound(tableRow->PickupSound);
			SetEquipSound(tableRow->EquipSound);
			ItemMeshComponent->SetSkeletalMesh(tableRow->ItemMesh);
			SetItemInventoryIcon(tableRow->InventoryIcon);
			AmmoIcon = tableRow->AmmoIcon;
			SetItemName(tableRow->ItemName);
		}
	}
#pragma endregion

}

void AQWeapon::FireOneBullet()
{
	if (AmmoAmount <= 0)
	{
		return;
	}
	SetAmmoAmount(AmmoAmount- 1);
}

bool AQWeapon::IsClipFull() const
{
	return AmmoAmount == MagazineCapcity;
}

UTexture2D* AQWeapon::GetAmmoIconTexture()
{
	return AmmoIcon;
}

void AQWeapon::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(AmmoAmount <= MagazineCapcity, TEXT("AmmoAmount must be <= MagazineCapcity"));
}

void AQWeapon::SetToEquipped(AQShooterCharacter* player)
{
	ensureMsgf(player, TEXT("Playe should not be null "));
	PlayerEuipThis = player;

	ConfigItemState(EQItemState::EIS_Equipped);

	if (USoundCue* euipSound = GetEquipSound())
	{
		UGameplayStatics::PlaySound2D(this, euipSound);
	}
}

void AQWeapon::ThrowItem()
{
	// 1. ����һ��impulse direction, Ԥ�ڷ���������ǰ������ƫһ��
	FVector impulseDirection = FVector::ForwardVector;
#pragma region CalImpulseDirection
	FRotator weaponRoation(0.0f, ItemMeshComponent->GetComponentRotation().Yaw, 0.0f);
	//FVector weaponFoward = 
	ItemMeshComponent->SetWorldRotation(weaponRoation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector itemRightDir = ItemMeshComponent->GetRightVector();
	const FVector itemForward = ItemMeshComponent->GetForwardVector();

	//������mesh ��right Dir ����Ϊ��ǰ��ǹ������Щ���ԣ�����ǹ��ǰ�����X��
	impulseDirection = itemRightDir.RotateAngleAxis(FMath::RandRange(10.0f, 30.f), ItemMeshComponent->GetUpVector());
	//impulseDirection = itemRightDir;
	impulseDirection *= 20000.0f;
#pragma endregion

	// 2. addimpulse
	ItemMeshComponent->AddImpulse(impulseDirection);
	bIsFalling = true;

	// 3. ����timer ��stop
	GetWorld()->GetTimerManager().SetTimer(FallingStopTimerHandle, this, &AQWeapon::StopFalling, FallingDuration);
}

bool AQWeapon::HasAmmo()
{
	return AmmoAmount > 0;
}

void AQWeapon::StopFalling()
{
	bIsFalling = false;
	ConfigItemState(EQItemState::EIS_ToPickUp);
}

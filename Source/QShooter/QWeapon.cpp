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

void AQWeapon::ThrowWeapon()
{
	// 1. 计算一个impulse direction, 预期方向是武器前向，左右偏一点
	FVector impulseDirection = FVector::ForwardVector;
#pragma region CalImpulseDirection
	FRotator weaponRoation(0.0f, ItemMeshComponent->GetComponentRotation().Yaw, 0.0f);
	//FVector weaponFoward = 
	ItemMeshComponent->SetWorldRotation(weaponRoation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector itemRightDir = ItemMeshComponent->GetRightVector();
	const FVector itemForward = ItemMeshComponent->GetForwardVector();

	//这里用mesh 的right Dir 是因为当前的枪坐标有些不对，不是枪的前向对着X轴
	impulseDirection = itemRightDir.RotateAngleAxis(FMath::RandRange(10.0f, 30.f), ItemMeshComponent->GetUpVector());
	//impulseDirection = itemRightDir;
	impulseDirection *= 20000.0f;
#pragma endregion

	// 2. addimpulse
	ItemMeshComponent->AddImpulse(impulseDirection);
	bIsFalling = true;

	// 3. 设置timer 以stop
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

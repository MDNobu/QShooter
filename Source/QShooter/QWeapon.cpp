// Fill out your copyright notice in the Description page of Project Settings.


#include "QWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

AQWeapon::AQWeapon()
{

}

void AQWeapon::FireOneBullet()
{
	if (AmmoAmount <= 0)
	{
		return;
	}
	AmmoAmount--;
}

void AQWeapon::SetToEquipped()
{
	/** 被装备状态下，武器忽略所有碰撞 */
	/*ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	ItemState = EQItemState::EIS_Equipped;*/
	SetItemState(EQItemState::EIS_Equipped);
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
	ItemState = EQItemState::EIS_ToPickUp;
	SetItemState(ItemState);
}

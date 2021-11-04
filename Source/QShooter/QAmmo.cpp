// Fill out your copyright notice in the Description page of Project Settings.


#include "QAmmo.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"


AQAmmo::AQAmmo()
{
	AmmoMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	ItemMeshComponent->SetupAttachment(RootComponent);
	ItemTriggerSphere->SetupAttachment(RootComponent);
	ItemWidgetVisibilityBox->SetupAttachment(RootComponent);
	ItemWidgetComponent->SetupAttachment(RootComponent);
}

void AQAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void AQAmmo::SetItemProperties(EQItemState targetItemState)
{
	Super::SetItemProperties(targetItemState);

	//USkeletalMeshComponent* AmmoMesh;
	switch (targetItemState)
	{
	case EQItemState::EIS_ToPickUp:
		// mesh property setting
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EQItemState::EIS_Equipped:
		// mesh property setting
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EQItemState::EIS_EquipInterping:
		// mesh property setting
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EQItemState::EIS_PickedUp:
		break;
	case EQItemState::EIS_Falling:
		// mesh property setting
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		break;
	case EQItemState::EIS_MAX:
		checkNoEntry();
		break;
	default:
		checkNoEntry();
		break;
	}

}

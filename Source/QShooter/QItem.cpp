// Fill out your copyright notice in the Description page of Project Settings.


#include "QItem.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "QShooterCharacter.h"
#include "QWeapon.h"
#include "Curves/CurveFloat.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"

// Sets default values
AQItem::AQItem() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMeshComponent"));
	SetRootComponent(ItemMeshComponent);

	ItemWidgetVisibilityBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemVisibilityBox"));
	ItemWidgetVisibilityBox->SetupAttachment(RootComponent);
	ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemWidgetVisibilityBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	ItemWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemWidgetComponnet"));
	ItemWidgetComponent->SetupAttachment(RootComponent);

	ItemTriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemTriggerSphere"));
	ItemTriggerSphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AQItem::BeginPlay()
{
	Super::BeginPlay();
	ItemWidgetComponent->SetVisibility(false);
	
	ItemTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AQItem::OnItemBeginOverlap);
	ItemTriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AQItem::OnItemEndOverlap);

	ConfigItemState(ItemState);

	if (ItemZFloatCurve && ItemScaleCurve)
	{
		float minTime = 0;
		float maxTime = 0;
		ItemZFloatCurve->GetTimeRange(minTime, maxTime);
		CollectAnimDuraction = maxTime - minTime;
	}

	DisableCustomDepth();
	EnableGlowEffect();
}

// Called every frame
void AQItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update Visibility
	{ //这里要求当前item actor 最好在 player character之后tick，
		//ItemWidgetComponent->SetVisibility(bIsVisible);
		if (!bIsVisibleLastFrame && bIsVisibleCurFrame) //前一帧没有显示而当前帧显示
		{
			ItemWidgetComponent->SetVisibility(true);
			EnableCustomDepth();
		}
		else if (bIsVisibleLastFrame && !bIsVisibleCurFrame) //前一帧显示而当前帧没有显示
		{
			ItemWidgetComponent->SetVisibility(false);
			DisableCustomDepth();
		}
		
		bIsVisibleLastFrame = bIsVisibleCurFrame;
		bIsVisibleCurFrame = false;
	}

	if (bIsCollectLerping) //lerping时EnableCustomDepth
	{
		EnableCustomDepth();
	}
	

	CollectLerping(DeltaTime);

	UpdateDynamicGlowParams(DeltaTime);
}

void AQItem::ShowItem()
{
	bIsVisibleCurFrame = true;
	
}

void AQItem::SetItemProperties(EQItemState targetItemState)
{
	switch (targetItemState)
	{
	case EQItemState::EIS_ToPickUp:
		// mesh property setting
		ItemMeshComponent->SetSimulatePhysics(false);
		ItemMeshComponent->SetEnableGravity(false);
		ItemMeshComponent->SetVisibility(true);
		ItemMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// trigger sphere setting
		ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		ItemTriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// visiblity box setting
		ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemWidgetVisibilityBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		ItemWidgetVisibilityBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// widget setting

		break;
	case EQItemState::EIS_Equipped:
		// mesh property setting
		ItemMeshComponent->SetSimulatePhysics(false);
		ItemMeshComponent->SetEnableGravity(false);
		ItemMeshComponent->SetVisibility(true);
		ItemMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// trigger sphere setting
		ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemTriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// visiblity box setting
		ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemWidgetVisibilityBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// widget setting
		ItemWidgetComponent->SetVisibility(false);
		break;
	case EQItemState::EIS_EquipInterping:
		// mesh property setting
		ItemMeshComponent->SetSimulatePhysics(false);
		ItemMeshComponent->SetEnableGravity(false);
		ItemMeshComponent->SetVisibility(true);
		ItemMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// trigger sphere setting
		ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemTriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// visiblity box setting
		ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemWidgetVisibilityBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// widget setting
		ItemWidgetComponent->SetVisibility(false);
		break;
	case EQItemState::EIS_PickedUp:
		// mesh property setting
		ItemMeshComponent->SetSimulatePhysics(false);
		ItemMeshComponent->SetEnableGravity(false);
		ItemMeshComponent->SetVisibility(false);
		ItemMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// trigger sphere setting
		ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemTriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// visiblity box setting
		ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemWidgetVisibilityBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// widget setting
		ItemWidgetComponent->SetVisibility(false);
		break;
	case EQItemState::EIS_Falling:
		// mesh property setting
		ItemMeshComponent->SetSimulatePhysics(true);
		ItemMeshComponent->SetEnableGravity(true);
		ItemMeshComponent->SetVisibility(true);
		ItemMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		// trigger sphere setting
		ItemTriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemTriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// visiblity box setting
		ItemWidgetVisibilityBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemWidgetVisibilityBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// falling 时重置character cache，目前放到这未必是合理的设计
		QPlayerCharacter = nullptr;
		break;
	case EQItemState::EIS_MAX:
		checkNoEntry();
		break;
	default:
		checkNoEntry();
		break;
	}



}

void AQItem::ConfigItemState(EQItemState targetItemState)
{
	if (targetItemState == ItemState)
	{
		return;
	}
	ItemState = targetItemState;
	SetItemProperties(ItemState);

#pragma region ConfigGlowEffect
	if (targetItemState == EQItemState::EIS_Equipped || targetItemState == EQItemState::EIS_Falling)
	{
		DisableGlowEffect();
	}
	else
	{
		EnableGlowEffect();
	}

	if (EQItemState::EIS_ToPickUp == ItemState)
	{
		ResetDynamicGlowTimer_ToPickUp();
	}
	else if (EQItemState::EIS_EquipInterping == ItemState)
	{
		ResetDynamicGlowTimer_Interping();
	}
#pragma endregion


	if (targetItemState != EQItemState::EIS_ToPickUp)
	{
		DisableCustomDepth();
	}
}

void AQItem::ChangeToFalling()
{
	//ItemMeshComponent->DetachFromParent(true);
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, true);
	ItemMeshComponent->DetachFromComponent(detachRules);
	//ItemState = EQItemState::EIS_Falling;
	//SetItemProperties(ItemState);
	ConfigItemState(EQItemState::EIS_Falling);
}

void AQItem::ChangeToPickedUp()
{
	ConfigItemState(EQItemState::EIS_PickedUp);
}

void AQItem::StartCollectLerping(AQShooterCharacter* character)
{
	UE_LOG(LogTemp, Warning, TEXT("StartCollect Item"));

	check(character);
	bIsCollectLerping = true;
	QPlayerCharacter = character;

	// 设置collect 动画用到的参数
	{ 
		CollectLerpStartLocation = GetActorLocation();
		//FInterpLocation interpSlot = character->SelectInterSlot();
		int32 slotIndex = -1;
		CameraTargetLocation = character->CalLocation4ItemCollectAnim(slotIndex, this);
		InterpSlotIndex = slotIndex;

		ItemCameraYawOffset = GetActorRotation().Yaw - character->GetFollowCamera()->GetComponentRotation().Yaw;
	}
	

	//ItemState = EQItemState::EIS_EquipInterping;
	//SetItemProperties(ItemState);
	ConfigItemState(EQItemState::EIS_EquipInterping);

	// Play Collect Sound
	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);
	}

	GetWorld()->GetTimerManager().SetTimer(CollectLerpEndHandle, this, &AQItem::EndCollectLerping, CollectAnimDuraction);
}

void AQItem::EnableCustomDepth()
{
	ItemMeshComponent->SetRenderCustomDepth(true);
}

void AQItem::DisableCustomDepth()
{
	ItemMeshComponent->SetRenderCustomDepth(false);
}

void AQItem::ThrowItem()
{

}

void AQItem::OnItemBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (AQShooterCharacter* shooterCharacter = CheckAndGetQShooterCharacter(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Item begin Overlapped"));
		shooterCharacter->IncreaseOverlapItemCount();
	}
}

void AQItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (AQShooterCharacter* character = CheckAndGetQShooterCharacter(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Item end Overlapped"));
		character->DecreaseOverlapItemCount();
	}
}

AQShooterCharacter* AQItem::CheckAndGetQShooterCharacter(AActor* actor)
{
	AQShooterCharacter* shooterCharacter = nullptr;
	if (actor)
	{
		shooterCharacter = Cast<AQShooterCharacter>(actor);
	}
	return shooterCharacter;
}

void AQItem::EndCollectLerping()
{
	bIsCollectLerping = false;
	ensure(QPlayerCharacter);
	if (QPlayerCharacter)
	{
		QPlayerCharacter->EndCollectItem(this);
	}
	SetActorScale3D(FVector(1.0f));
}

void AQItem::CollectLerping(float deltaTime)
{
	if (!bIsCollectLerping)
		return;
	if (!QPlayerCharacter || !ItemZFloatCurve || !ItemScaleCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemZFloatCuve 或者ItemscaleCurve没设置，清检查"));
		return;
	}

	FVector newLocation = CollectLerpStartLocation;
	// 1. read curve z float
	float elapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(CollectLerpEndHandle);
	//elapsedTime *= 0.1f; //这里是为了测试

	const float deltaZFromeCurve = ItemZFloatCurve->GetFloatValue(elapsedTime);

	// 2. translate into position
	newLocation.Z += deltaZFromeCurve * (CameraTargetLocation - CollectLerpStartLocation).Z;

	const FVector curLocation = GetActorLocation();
	newLocation.X = FMath::FInterpTo(curLocation.X, CameraTargetLocation.X, deltaTime, 30.0f);//30.0是interp speed
	newLocation.Y = FMath::FInterpTo(curLocation.Y, CameraTargetLocation.Y, deltaTime, 30.0f);

	
	// 3. set location
	SetActorLocation(newLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// 4. set rotation 使item在camera space 的rotation 不变
	const float itemYaw = QPlayerCharacter->GetFollowCamera()->GetComponentRotation().Yaw + ItemCameraYawOffset;
	const FRotator itemRotation(0.0f, itemYaw, 0.0f);
	SetActorRotation(itemRotation, ETeleportType::TeleportPhysics);

	// 5. set scale
	const float itemScale = ItemScaleCurve->GetFloatValue(elapsedTime);
	SetActorScale3D(FVector(itemScale));
}

void AQItem::EnableGlowEffect()
{
	if (ItemMaterialDynamic)
	{
		ItemMaterialDynamic->SetScalarParameterValue(FName(TEXT("GlowEffectAlpha")), 5.0f);
	}
}

void AQItem::DisableGlowEffect()
{
	if (ItemMaterialDynamic)
	{
		ItemMaterialDynamic->SetScalarParameterValue(FName(TEXT("GlowEffectAlpha")), 0.0f);
	}
}

void AQItem::ResetDynamicGlowTimer_ToPickUp()
{
	if (EQItemState::EIS_ToPickUp == ItemState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reset DynamicGlowTime ToPickup"));
		GetWorldTimerManager().SetTimer(
			DynamicGlowTimerHandle_ToPickUp, this, &AQItem::ResetDynamicGlowTimer_ToPickUp, PulseCurveTime_ToPickUp, false);
	}
}



void AQItem::ResetDynamicGlowTimer_Interping()
{
	if (EQItemState::EIS_EquipInterping == ItemState)
	{
		GetWorldTimerManager().SetTimer(
			DynamicGlowTimerHandle_Interping, this, &AQItem::ResetDynamicGlowTimer_Interping, PulseCurveTime_Interping, false);
	}
}

void AQItem::UpdateDynamicGlowParams(float DeltaTime)
{
	if (nullptr == ItemMaterialDynamic)
	{
		return;
	}

	float elapseTime; 
	FVector glowEffectParams;
	bool ifGlowParamsFinded = false;
	if (EQItemState::EIS_ToPickUp == ItemState && GlowEffectCurve_ToPickUp)
	{
		elapseTime  = GetWorldTimerManager().GetTimerElapsed(DynamicGlowTimerHandle_ToPickUp);
		glowEffectParams = GlowEffectCurve_ToPickUp->GetVectorValue(elapseTime);
		ifGlowParamsFinded = true;
	} else if (EQItemState::EIS_EquipInterping == ItemState && GlowEffectCurve_Interping)
	{
		elapseTime = GetWorldTimerManager().GetTimerElapsed(DynamicGlowTimerHandle_Interping);
		glowEffectParams = GlowEffectCurve_Interping->GetVectorValue(elapseTime);
		ifGlowParamsFinded = true;
	}

	if (ifGlowParamsFinded)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Update Material"));
		ItemMaterialDynamic->SetScalarParameterValue(
			TEXT("GlowEffectAlpha"),
			glowEffectParams.X * GlowEffectWeight); // curve的x参与glow effect weight
		ItemMaterialDynamic->SetScalarParameterValue(
			TEXT("FresnelExponent"),
			glowEffectParams.Y * FresnelExponent); // curve的y参与fresenelExponent
		ItemMaterialDynamic->SetScalarParameterValue(
			TEXT("FresnelFraction"),
			glowEffectParams.Z * FresnelReflectFraction); // curve的z参与FresnelReflectFraction
	}

}

void AQItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemMaterial)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Update Material"));
		ItemMaterialDynamic = UMaterialInstanceDynamic::Create(ItemMaterial, this);
		ItemMeshComponent->SetMaterial(ItemMaterialIndex, ItemMaterialDynamic);
	}
}


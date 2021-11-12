// Fill out your copyright notice in the Description page of Project Settings.


#include "QShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "QItem.h"
#include "QWeapon.h"
#include "Components/CapsuleComponent.h"
#include "QAmmo.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "QShooter.h"
#include "QBulletHitInterface.h"
#include "QEnemy.h"

// Sets default values
AQShooterCharacter::AQShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 45.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 注意
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* movementCom = GetCharacterMovement();
	movementCom->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	movementCom->bOrientRotationToMovement = false;
	movementCom->JumpZVelocity = 600.0f;

	SceneCom4MoveClip = CreateDefaultSubobject<USceneComponent>(TEXT("ScneeComForMoveClip"));

#pragma region SetupSceneComs4CollectItem
	WeaponSceneCom = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponCollectSceneCom"));
	WeaponSceneCom->SetupAttachment(GetFollowCamera());
	InterpCom1 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom1"));
	InterpCom1->SetupAttachment(GetFollowCamera());
	InterpCom2 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom2"));
	InterpCom2->SetupAttachment(GetFollowCamera());
	InterpCom3 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom3"));
	InterpCom3->SetupAttachment(GetFollowCamera());
	InterpCom4 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom4"));
	InterpCom4->SetupAttachment(GetFollowCamera());
	InterpCom5 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom5"));
	InterpCom5->SetupAttachment(GetFollowCamera());
	InterpCom6 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpCom6"));
	InterpCom6->SetupAttachment(GetFollowCamera());
#pragma endregion

}

void AQShooterCharacter::IncreaseOverlapItemCount()
{
	OverlapItemCount++;
}

void AQShooterCharacter::DecreaseOverlapItemCount()
{
	OverlapItemCount--;
}

void AQShooterCharacter::Jump()
{
	Super::Jump();
	if (bIsCrouching)
	{
		//bIsCrouching = false; // Jump之后不再crouch
		//GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
		ToggleCrouch();
	}
}

float AQShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	float damageTaked = 0.0f;
	if (Health - Damage <= 0)
	{
		damageTaked = Health;
		Health = 0;
		// #TODO Die
	}
	else
	{
		Health -= Damage;
		damageTaked = Damage;
	}

	return damageTaked;
}

// Called when the game starts or when spawned
void AQShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraDefaultFOV = FollowCamera->FieldOfView;
	CameraCurrenFOV = CameraDefaultFOV;

	//init inventory
	InventoryWeapons.Init(nullptr, INVENTORY_CAPCITY);

	SpawnAndEquipDefaultWeapon();

	AmmoAmountInitial();

#pragma region dataValidation
	ensure(2 * FireDuration <= AutoFireDuration);
#pragma endregion


	InitInterpLocations();
}

void AQShooterCharacter::AimButtonPressed()
{
	bIsAimButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Equipping) //Reloading时不能aim
	{
		StartAim();
	}
}

void AQShooterCharacter::AimButtonReleased()
{
	bIsAimButtonPressed = false;
	StopAim();
}

void AQShooterCharacter::StopAim()
{
	bIsAimming = false;
	if (!bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
	}
}

void AQShooterCharacter::ConsumeAmmo(AQAmmo* ammo)
{
#pragma region UpdatePlayerAmmo
	int32 targetTypeAmmoAmount = 0;
	const int32* pAmmoAmount = CurAmmoAmounts.Find(ammo->GetAmmoType());
	if (pAmmoAmount)
	{
		targetTypeAmmoAmount = *pAmmoAmount;
	}
	const int32 resAmmoAmount = targetTypeAmmoAmount + ammo->GetItemAmount();
	CurAmmoAmounts.Add(ammo->GetAmmoType(), resAmmoAmount);
#pragma endregion

#pragma region ReloadIfEquipWeaponIsEmpty
	const bool euipWeaponIsEmpty = EquippedWeapon && (0 == EquippedWeapon->GetAmmoAmount());
	const bool isSameType = ammo->GetAmmoType() == EquippedWeapon->GetAmmoType();
	if (euipWeaponIsEmpty && isSameType) //Equipped Weapon is empty
	{
		StartReloadWeapon();
	}
#pragma endregion

	ammo->Destroy();
}

void AQShooterCharacter::InitInterpLocations()
{
	//WeaponCollectInterpSlot = FInterpLocation(WeaponSceneCom, 0);
	//throw std::logic_error("The method or operation is not implemented.");
	ItemCollectInterpSlots.Add(FInterpLocation(WeaponSceneCom, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom1, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom2, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom3, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom4, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom5, 0));
	ItemCollectInterpSlots.Add(FInterpLocation(InterpCom6, 0));
}

FInterpLocation AQShooterCharacter::GetItemCollectInterpLocation(int32 index)
{
	if (index >= 0 && index < ItemCollectInterpSlots.Num())
	{
		return ItemCollectInterpSlots[index];
	}
	return FInterpLocation();
}

void AQShooterCharacter::UpdateHighlightInventory()
{
	// &&
	if (PreFocusedItem == FocusedItem) //只有当前帧和前一帧不同时，才可能hight/unlight
	{
		return;
	}
	if (Cast<AQWeapon>(FocusedItem))
	{
		//High light an availabe slot
		int32 emptySlot = InventoryWeapons.Find(nullptr);
		if (INDEX_NONE != emptySlot)
		{
			highlightingInventorySlotIndex = emptySlot;
			InventorySlotHighlightDelegate.Broadcast(highlightingInventorySlotIndex, true);
		}
	}
	else
	{
		// Unhighligt slot
		InventorySlotHighlightDelegate.Broadcast(highlightingInventorySlotIndex, false);
		highlightingInventorySlotIndex = -1;
	}
}

EPhysicalSurface AQShooterCharacter::LineTraceSurfaceType()
{
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.bReturnPhysicalMaterial = true;
	const FVector startLocation = GetActorLocation();
	const FVector endLocation = startLocation + FVector(0.0f, 0.0f, -200.0f);
	GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECC_Visibility, queryParams);
	if (hitResult.bBlockingHit)
	{
		
		if (EPS_Grass == UPhysicalMaterial::DetermineSurfaceType(hitResult.PhysMaterial.Get()) )
		{
			UE_LOG(LogTemp, Warning, TEXT("step on grass"));
		}
	}
	

	return UPhysicalMaterial::DetermineSurfaceType(hitResult.PhysMaterial.Get());
}

float AQShooterCharacter::GetCrosshairSpeadMultiplier() const
{
	return CrosshairSpeadMultiplier;
}

FVector AQShooterCharacter::CalLocation4ItemCollectAnim(OUT int& interpSlotIndex, AQItem* targetItem)
{
#pragma region Algorithm1
	/*
	if ((nullptr == GEngine)
		|| (nullptr == GEngine->GameViewport))
	{
		checkNoEntry();
	}

	FVector2D screenSize;
	GEngine->GameViewport->GetViewportSize(screenSize);
	// item目标是显示在屏幕中间上方的位置
	FVector2D itemTargetPoint2D(screenSize.X / 2.0, 0.0f + 100.0f);

	// 从crosshair的位置计算得到world space linetrace的起点和终点
	FVector itemTargetPoint;
	FVector itemTragetDirection;
	bool bScreenToWorldSuccess = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), itemTargetPoint2D, itemTargetPoint, itemTragetDirection);
	ensure(bScreenToWorldSuccess);

	itemTargetPoint += itemTragetDirection * CameraTargetOffset;
	return itemTargetPoint;
	*/
#pragma endregion
	//FInterpLocation interLocation;
	FVector interpPosition = FVector::ZeroVector;
	int32 resSlotIndex = -1;
	/** 根据负载，选择一个item collect动画用到的interp location */
	//targetItem->GetClass()->is
	if (Cast<AQWeapon>(targetItem))
	{
		interpPosition = ItemCollectInterpSlots[0].InterpSceneCom->GetComponentLocation();
		resSlotIndex = 0;
		
	}
	else
	{
#pragma region SelectSlotByWeight
		int32 lowerestCount = INT_MAX;

		for (int32 i = 1; i < ItemCollectInterpSlots.Num(); i++)
		{
			if (ItemCollectInterpSlots[i].InterpNum < lowerestCount)
			{
				lowerestCount = ItemCollectInterpSlots[i].InterpNum;
				resSlotIndex = i;
			}
		}
		interpPosition = ItemCollectInterpSlots[resSlotIndex].InterpSceneCom->GetComponentLocation();
#pragma endregion

	}
	/*FName className = targetItem->GetClass()->GetFName();
	UE_LOG(LogTemp, Warning, TEXT("Collect item class name = %s"), *className.ToString());*/

	//const FVector targetPointCS(CameraLerpPointDeltaX, 0.0f, CameraLerpPointDeltaZ);
	////FollowCamera->world
	//const FVector targetPointWS = FollowCamera->GetComponentTransform().TransformPosition(targetPointCS);

	DrawDebugPoint(GetWorld(), interpPosition, 20.0f, FColor::Red, true);
	//return targetPointWS;
	UE_LOG(LogTemp, Warning, TEXT("item %s use slot index %d"), *targetItem->GetName(), resSlotIndex);

	ItemCollectInterpSlots[resSlotIndex].InterpNum++;

	interpSlotIndex = resSlotIndex;
	return interpPosition;
}

void AQShooterCharacter::EndCollectItem(AQItem* toCollectItem)
{
	if (!toCollectItem)
		return;

	//ItemCollectInterpSlots[]
	// 回收collect slot
	{
		int32 slotIndex = toCollectItem->GetInterpSlotIndex();
		if (slotIndex >= 0 && slotIndex < ItemCollectInterpSlots.Num())
		{
			ItemCollectInterpSlots[slotIndex].InterpNum--;
		}
		else
		{
			checkNoEntry();
		}
	}

	if (AQWeapon* weapon = Cast<AQWeapon>(toCollectItem))
	{
		if (IsInventoryFull())
		{
			SwapWeapon(weapon);
		}
		else
		{
			//使用新获得的weapon
			//EquipedWeaponInventoryIndex = AddItemToInventory(weapon);
			//EquippedWeapon = InventoryWeapons[EquipedWeaponInventoryIndex];
			AddItemToInventory(weapon);
			//EquipWeapon( weapon);
			weapon->ChangeToPickedUp();
		}

	}
	else if (AQAmmo* ammoItem = Cast<AQAmmo>(toCollectItem))
	{
		ConsumeAmmo(ammoItem);
	}
	else
	{
		// #TODO collect item logic
		UE_LOG(LogTemp, Warning, TEXT("Item %s is collected"), *toCollectItem->GetFName().ToString())
	}

}

// Called every frame
void AQShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateCameraZoom(DeltaTime);

	UpdateCameraRotateRateByIsAimming();

#pragma region UpdateMouseTurnLookupRateByAimming
	CurrentMouseTurnRate = bIsAimming ? AimMouseTurnRate : HipMouseTurnRate;
	CurrentMouseLookupRate = bIsAimming ? AimMouseLookupRate : HipMouseLookupRate;
#pragma endregion


	UpdateCrosshairSpeadMultiplier(DeltaTime);


	LineTraceToShowItems();

	UpdateCapsuleHalfHeight(DeltaTime);


	UpdateHighlightInventory();

	//LineTraceFootstep();
}


void AQShooterCharacter::UpdateCameraZoom(float deltaTime)
{
	if (bIsAimming)
	{
		CameraCurrenFOV = FMath::FInterpTo(CameraCurrenFOV, CameraZoomInFOV, deltaTime, CameraZoomInterpSpeed);
	}
	else
	{
		CameraCurrenFOV = FMath::FInterpTo(CameraCurrenFOV, CameraDefaultFOV, deltaTime, CameraZoomInterpSpeed);
	}
	FollowCamera->SetFieldOfView(CameraCurrenFOV);
}

void AQShooterCharacter::UpdateCrosshairSpeadMultiplier(float deltaTime)
{
#pragma region CalVelocityFactor
	FVector2D walkSpeedRange(0.0f, 600.0f);
	FVector2D velocityFactorRange(0.0f, 1.0f);
	FVector velocity = GetVelocity();
	velocity.Z = 0;
	float speed = velocity.Size();
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityFactorRange, speed);
#pragma endregion

#pragma region CalInAirFactor
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,
			2.25f, //在空中的cross factror
			deltaTime, 
			2.25f // cross hair lerp speed in air
			);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,
			0.f, //在地面的cross factror
			deltaTime,
			4.0f // cross hair lerp speed in land
		);
	}
#pragma endregion

#pragma region CalAimFactor
	if (bIsAimming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,
			-0.5f, //瞄准时缩小crosshair,所以这里的值为负
			deltaTime,
			1.0f // cross hair lerp speed when aimming
		);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor,
			0.0f, //不瞄准时的cross factror
			deltaTime,
			4.0f // cross hair lerp speed not aiming
		);
	}
#pragma endregion

#pragma region CalShootingFactor
	if (bIsBulletFiring)
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.3f,//射击一发子弹的crosshair factor,这个一般比较小
			deltaTime,
			60.0f //lerp speed 设置高，快速
			);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,// 
			deltaTime,
			60.0f //lerp speed 设置高，快速
		);
	}
#pragma endregion


	CrosshairSpeadMultiplier = 
		0.5f +  //0.5是默认的crosshair spead的值，因为有需要缩小的时候
		CrosshairVelocityFactor + 
		CrosshairInAirFactor +
		CrosshairAimFactor +
		CrosshairShootingFactor;

	FString debugMsg = FString::Printf(TEXT("Crosshair Spread Multiplier: %f"), CrosshairShootingFactor);
	//GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, debugMsg);
}

// Called to bind functionality to input
void AQShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AQShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AQShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnAtRate"), this, &AQShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpAtRate"), this, &AQShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AQShooterCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AQShooterCharacter::Lookup);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AQShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("AimButton"), IE_Pressed, this, &AQShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("AimButton"), IE_Released, this, &AQShooterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction(TEXT("FireWeapon"), IE_Pressed, this, &AQShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("FireWeapon"), IE_Released, this, &AQShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction(TEXT("SelectButton"), IE_Pressed, this, &AQShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction(TEXT("SelectButton"), IE_Released, this, &AQShooterCharacter::SelectButtonReleased);
	
	PlayerInputComponent->BindAction(TEXT("ThrowWeapon"), IE_Pressed, this, &AQShooterCharacter::DropEquippedWeapon);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AQShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AQShooterCharacter::ToggleCrouch);

#pragma region BindKey4ChangeEquipWeapon
	PlayerInputComponent->BindAction(TEXT("FKey"), IE_Pressed, this, &AQShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction(TEXT("1Key"), IE_Pressed, this, &AQShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction(TEXT("2Key"), IE_Pressed, this, &AQShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction(TEXT("3Key"), IE_Pressed, this, &AQShooterCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction(TEXT("4Key"), IE_Pressed, this, &AQShooterCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction(TEXT("5Key"), IE_Pressed, this, &AQShooterCharacter::FiveKeyPressed);

#pragma endregion

}

void AQShooterCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsFalling()) // 在空中时不能crouch
	{
		return;
	}
	bIsCrouching = !bIsCrouching;
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMaxWalkSpeed;
		GetCharacterMovement()->GroundFriction = CrouchGroundFirction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFirction;
	}
}


void AQShooterCharacter::MoveForward(float value)
{
	if (Controller && (0.0f != value))
	{
		const FRotator controlRoation = Controller->GetControlRotation();
		const FRotator yawRotation(0.0f, controlRoation.Yaw, 0.0f);

		const FVector forwardDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(forwardDir, value);
	}
}

void AQShooterCharacter::MoveRight(float value)
{
	if (Controller && (0.0f != value))
	{
		const FRotator controlRoation = Controller->GetControlRotation();
		const FRotator yawRotation(0.0f, controlRoation.Yaw, 0.0f);

		const FVector rightDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(rightDir, value);
	}
}

void AQShooterCharacter::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AQShooterCharacter::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AQShooterCharacter::Turn(float value)
{
	AddControllerYawInput(value * CurrentMouseTurnRate);
}

void AQShooterCharacter::Lookup(float value)
{
	AddControllerPitchInput(value * CurrentMouseLookupRate);
}


void AQShooterCharacter::FireOneBulletEffects()
{
	if (!EquippedWeapon || !EquippedWeapon->HasAmmo())
	{
		return;
	}

	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
	
	const USkeletalMeshSocket* barrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName(TEXT("BarrelSocket"));
	if (barrelSocket)
	{
		const FTransform barrelSSocketTransform = barrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), barrelSSocketTransform);
		}


		//计算bulletTrailEndPoint
		// 需要line trace 两次，一次以crosshair为起点，一次以barrel socket为起点
		//FVector bulletTrailEndPoint;
		FHitResult hitResult4Bullet;
		LineTrace4Bullet(barrelSSocketTransform, hitResult4Bullet);

#pragma region SpawnHitFXAndTrailFX


		if (hitResult4Bullet.bBlockingHit)
		{
			// 击中粒子特效
			if (IQBulletHitInterface* bulletHitable = Cast<IQBulletHitInterface>(hitResult4Bullet.Actor.Get()))
			{
				//击中bulletHitInterface，由接口处理
				//bulletHitable->BulletHit(hitResult4Bullet);
				bulletHitable->BulletHit_Implementation(hitResult4Bullet);
			}
			else if (ImpactHitFX)  // 否则spawn 默认粒子
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactHitFX, hitResult4Bullet.Location);
			}


			// trail特效
			if (BulletTrailFX)
			{
				UParticleSystemComponent* bulletParticleCom =
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailFX, barrelSSocketTransform);

				bulletParticleCom->SetVectorParameter(FName(TEXT("Target")), hitResult4Bullet.Location);
			}


			//Damage处理
			if (AQEnemy* enemy = Cast<AQEnemy>(hitResult4Bullet.Actor.Get()))
			{
				const bool isHeadshot = enemy->GetHeadBoneName().IsEqual(hitResult4Bullet.BoneName);
				const float damage = isHeadshot ? EquippedWeapon->GetHeadshotDamage() : EquippedWeapon->GetDamage();

				UGameplayStatics::ApplyDamage(
					enemy, 
					damage,
					GetController(), 
					this, 
					UDamageType::StaticClass());

				UE_LOG(LogTemp, Warning,
					TEXT("hitted bone name is %s, is headshot %d"),
					*hitResult4Bullet.BoneName.ToString(),
					isHeadshot);
			
				enemy->ShowHitNumber(damage, hitResult4Bullet.Location, isHeadshot);
			}
		}
#pragma endregion


	}

	UAnimInstance* characterAnimInstance = GetMesh()->GetAnimInstance();
	if (characterAnimInstance && FireAnimMontage)
	{
		characterAnimInstance->Montage_Play(FireAnimMontage);
		characterAnimInstance->Montage_JumpToSection(TEXT("StartFire"));
	}

}

void AQShooterCharacter::ChangeEquipWeapon(int32 newWeaponInventIndex)
{

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}
	bool inChangeableState = (CombatState == ECombatState::ECS_Unoccupied) || (CombatState == ECombatState::ECS_Equipping);
	bool isNewIndexLegal = (newWeaponInventIndex >= 0) && (newWeaponInventIndex < InventoryWeapons.Num());
	bool notSameWithEquiped = (InventoryWeapons[newWeaponInventIndex] != EquippedWeapon);
		//EquippedWeapon && (EquippedWeapon->GetInventoryIndex() != newWeaponInventIndex);
	if (isNewIndexLegal && notSameWithEquiped)
	{
		if (AQWeapon* newWeapon = Cast<AQWeapon>(InventoryWeapons[newWeaponInventIndex]))
		{
			if (bIsAimming)
			{
				StopAim();
			}

			UE_LOG(LogTemp, Warning, TEXT("change to weapon in slot %d"), newWeaponInventIndex);
			EquipWeapon(newWeapon);
		}
	}
	
}

void AQShooterCharacter::EndFireBullet()
{
	bIsBulletFiring = false;
	CombatState = ECombatState::ECS_Unoccupied;
	UE_LOG(LogTemp, Warning, TEXT("End Firing"));
}

void AQShooterCharacter::SelectButtonPressed()
{
	//DropEquippedWeapon();
	if (FocusedItem )
	{
		FocusedItem->StartCollectLerping(this);
	}
}

void AQShooterCharacter::SelectButtonReleased()
{

}

void AQShooterCharacter::AmmoAmountInitial()
{
	CurAmmoAmounts.Reset();
	CurAmmoAmounts.Append(AmmoInitAmounts);
}

bool AQShooterCharacter::DoesEquippedWeaponHasAmmo()
{
	return EquippedWeapon && EquippedWeapon->HasAmmo();
}


void AQShooterCharacter::StartReloadWeapon()
{
	if (ECombatState::ECS_Unoccupied != GetCombatState())
		return;

	if (HasSuitableAmmoPack() && !EquippedWeapon->IsClipFull()) //没有ammo pack，则不能reload
	{
		if (bIsAimming) 
		{
			StopAim();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* characterAnimInstance = GetMesh()->GetAnimInstance();
		if (characterAnimInstance && ReloadAnimMontage)
		{
			characterAnimInstance->Montage_Play(ReloadAnimMontage);
			characterAnimInstance->Montage_JumpToSection(EquippedWeapon->GetReload_AM_SectionName());
			//EquippedWeapon->reloa
		}
	}
}

void AQShooterCharacter::EndReloadWeapon()
{
	CombatState = ECombatState::ECS_Unoccupied;

	
	if (!EquippedWeapon) 
		return;

	//UE_LOG(LogTemp, Warning, TEXT("Finish Reloading"));
	// Update Current Ammo and weapon ammo
#pragma region Update Current Ammo and weapon ammo
	const EAmmoType ammoType = EquippedWeapon->GetAmmoType();

	const int32 emptySize = EquippedWeapon->GetMagazineCapcity() - EquippedWeapon->GetAmmoAmount();
	checkf(emptySize >= 0, TEXT("weapon empty size should be >= 0"));

	const int32 playerAmmoAmount = CurAmmoAmounts[ammoType];
	UE_LOG(LogTemp, Warning, TEXT("playerAmmoAmount = %d , emptySize = %d"), playerAmmoAmount, emptySize);
	if (playerAmmoAmount < emptySize)
	{
		EquippedWeapon->IncreaseAmmo(playerAmmoAmount);
		CurAmmoAmounts[ammoType] = 0;
	}
	else
	{
		EquippedWeapon->IncreaseAmmo(emptySize);
		CurAmmoAmounts[ammoType] -= emptySize;
	}
#pragma endregion

	if (bIsAimButtonPressed)
	{
		StartAim();
	}
}

void AQShooterCharacter::EndEquipping()
{
	UE_LOG(LogTemp, Warning, TEXT("End Equipping weapon"));
	CombatState = ECombatState::ECS_Unoccupied;
	if (bIsAimButtonPressed)
	{
		StartAim();
	}
}

/**
 * 实现的基本思想是，在handl_l 上attach一个scene component, runtime 时修改clip的transform,使其和scene component一致
 */
void AQShooterCharacter::GrabClip()
{
	if (nullptr == EquippedWeapon)
	{
		return;
	}

	//const USkeletalMeshSocket* handLBone = GetMesh()->GetSocketByName(TEXT("hand_l"));
	FAttachmentTransformRules transRules(EAttachmentRule::KeepRelative, true);
	SceneCom4MoveClip->AttachToComponent(GetMesh(), transRules, TEXT("hand_l"));

	//ClipTransWhenDetached = EquippedWeapon->GetItemMesh()->GetSocketTransform("smg_clip");
	int32 boneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipName());
	if (INDEX_NONE == boneIndex)
	{
		UE_LOG(LogTemp, Error, 
			TEXT("Weapon %s cannot find a bone name %s"), *EquippedWeapon->GetFName().ToString(), *EquippedWeapon->GetClipName().ToString());
		return;
	}
	ClipTransWhenDetached = EquippedWeapon->GetItemMesh()->GetBoneTransform(boneIndex);
	SceneCom4MoveClip->SetWorldTransform(ClipTransWhenDetached);

	bIsClipMoving = true;
}

void AQShooterCharacter::InsertClip()
{
	bIsClipMoving = false;
}


void AQShooterCharacter::ReloadButtonPressed()
{
	StartReloadWeapon();
}

void AQShooterCharacter::FKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("FKey Pressed"));
	int targetIndex = 0;
	ChangeEquipWeapon(targetIndex);
}

void AQShooterCharacter::OneKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("1Key Pressed"));
	int targetIndex = 1;
	ChangeEquipWeapon(targetIndex);
}

void AQShooterCharacter::TwoKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("2Key Pressed"));
	int targetIndex = 2;
	ChangeEquipWeapon(targetIndex);
}

void AQShooterCharacter::ThreeKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("3Key Pressed"));
	int targetIndex = 3;
	ChangeEquipWeapon(targetIndex);
}

void AQShooterCharacter::FourKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("4Key Pressed"));
	int targetIndex = 4;
	ChangeEquipWeapon(targetIndex);
}

void AQShooterCharacter::FiveKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("5Key Pressed"));
	int targetIndex = 5;
	ChangeEquipWeapon(targetIndex);
}

bool AQShooterCharacter::HasSuitableAmmoPack()
{
	bool hasAmmo = false;
	if (EquippedWeapon)
	{
		EAmmoType ammoType = EquippedWeapon->GetAmmoType();
		if (CurAmmoAmounts.Contains(ammoType))
		{
			hasAmmo = CurAmmoAmounts[ammoType] > 0;
		}
	}
	return hasAmmo;
}



void AQShooterCharacter::UpdateCapsuleHalfHeight(float DeltaTime)
{
	const float target = bIsCrouching ? CrouchCapsuleHalfHeight : BaseCapsuleHalfHeight;
	const float curHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float resHalfHeight = FMath::FInterpTo(curHalfHeight, target, DeltaTime, 20.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(resHalfHeight);

	// 注意，上面将capsule height 改变之后,capsule的z是正确的贴合地面的，但是动画的基准（mesh component）却不是贴合地面的，这里需要修正
	const float deltaHalfHeight = resHalfHeight - curHalfHeight;
	const FVector deltaVector = FVector(0.0f, 0.0f, -deltaHalfHeight); 
	GetMesh()->AddLocalOffset(deltaVector);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, -1, FColor::Red,
			FString::Printf(TEXT("delta z: %f"), deltaHalfHeight));
	}
}

void AQShooterCharacter::StartAim()
{
	bIsAimming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMaxWalkSpeed; // 这里只是想要一个比BaseSpeed小的数值，用CrouchMaxSpeed是图方便
}


bool AQShooterCharacter::LineTrace4Bullet(const FTransform& socketTransform, OUT FHitResult& bulletTrailEndPoint)
{
	
	//FVector bulletTrailEndPoint;

	FVector startLocation;
	FVector endLocation;
	GenerateCrosshairPoints4LineTrace(startLocation, endLocation);

	//计算bulletTrailEndPoint
	// 需要line trace 两次，一次以crosshair为起点，一次以barrel socket为起点

	FHitResult hitResFromCrosshair;
	GetWorld()->LineTraceSingleByChannel(hitResFromCrosshair, startLocation,
		endLocation, ECollisionChannel::ECC_Visibility, FCollisionQueryParams::DefaultQueryParam);


	FHitResult returnHitResult;
	if (hitResFromCrosshair.bBlockingHit)
	{
		//bulletTrailEndPoint = hitResFromCrosshair.Location;
		returnHitResult = hitResFromCrosshair;

		//第一次line trace碰到东西才进行第二次
		FHitResult hitResFromBarret;
		GetWorld()->LineTraceSingleByChannel(hitResFromBarret,
			socketTransform.GetLocation(),
			hitResFromCrosshair.Location,
			ECollisionChannel::ECC_Visibility);
		if (hitResFromBarret.bBlockingHit)
		{
			//bulletTrailEndPoint = hitResFromBarret.Location;
			returnHitResult = hitResFromBarret;
		}
	}
	bulletTrailEndPoint = returnHitResult;

	//if (bBulletHitSomething) 
	//{
	//	
	//}

	return returnHitResult.bBlockingHit;
}

void AQShooterCharacter::UpdateCameraRotateRateByIsAimming()
{
	if (bIsAimming)
	{
		BaseTurnRate = AimTurnRate;
		BaseLookUpRate = AimLookupRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookupRate;
	}
}

bool AQShooterCharacter::LineTraceFromCrosshair(FHitResult& hitResult)
{

	FVector startLocation;
	FVector endLocation;
	GenerateCrosshairPoints4LineTrace(startLocation, endLocation);

	GetWorld()->LineTraceSingleByChannel(hitResult, startLocation,
		endLocation, ECollisionChannel::ECC_Visibility, FCollisionQueryParams::DefaultQueryParam);


	//DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, false, 2.0f);
	return hitResult.bBlockingHit;
}


void AQShooterCharacter::GenerateCrosshairPoints4LineTrace(OUT FVector& startPoint, OUT FVector& endPoint)
{
	//计算crosshair的屏幕空间位置
	if ((nullptr == GEngine)
		|| (nullptr == GEngine->GameViewport))
	{
		checkNoEntry();
	}

	FVector2D screenSize;
	GEngine->GameViewport->GetViewportSize(screenSize);
	FVector2D crosshairArchorPoint = screenSize / 2.0f;
	//这里减去50是要将crosshair的位置上移50像素
	crosshairArchorPoint.Y -= 50.0f;

	// 从crosshair的位置计算得到world space linetrace的起点和终点
	FVector crosshairLocationWS;
	FVector crosshairDirectionWS;
	bool bScreenToWorldSuccess = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), crosshairArchorPoint, crosshairLocationWS, crosshairDirectionWS);

	check(bScreenToWorldSuccess);

	startPoint = crosshairLocationWS;
	endPoint = startPoint + crosshairDirectionWS * 50000.0f;
}

void AQShooterCharacter::LineTraceToShowItems()
{
	PreFocusedItem = FocusedItem;

	// 没有overlap item则不需要line trace
	if (OverlapItemCount <= 0)
		return;

	
	//重置focus结果，trace有结果后赋值
	FocusedItem = nullptr;

	FHitResult itemTraceRes;
	LineTraceFromCrosshair(itemTraceRes);
	if (itemTraceRes.bBlockingHit)
	{
		AQItem* item = Cast<AQItem>(itemTraceRes.GetActor());
		if (item)
		{
			FocusedItem = item;
			item->ShowItem();
		}
	}
}

void AQShooterCharacter::SpawnAndEquipDefaultWeapon()
{
	if (DefaulWeaponClass)
	{
		AQWeapon* newWeapon = GetWorld()->SpawnActor<AQWeapon>(DefaulWeaponClass);
		AddItemToInventory(newWeapon);
		EquipWeapon(newWeapon);
	}
}

//void AQShooterCharacter::EquipWeapon(AQWeapon* newWeapon)
//{
//	AQWeapon* preWeapon = EquippedWeapon;
//	EquipWeapon(preWeapon, newWeapon);
//}

void AQShooterCharacter::EquipWeapon( AQWeapon* newWeapon)
{
	if (EquippedWeapon == newWeapon || 
		nullptr == newWeapon)
	{
		return;
	}

#pragma region BroadcastEvent4InventoryUI
	if (nullptr == EquippedWeapon)
	{
		// 如果当前EquipWeapon为空，尝试使用preWeaponindx
		if (PreWeaponInventoryIndex != INDEX_NONE)
		{
			EquipItemDelegate.Broadcast(PreWeaponInventoryIndex, newWeapon->GetInventoryIndex());
			//PreWeaponInventoryIndex = INDEX_NONE;
			
		}
		else
		{
			EquipItemDelegate.Broadcast(-1, newWeapon->GetInventoryIndex());
		}
	}
	else
	{
		EquipItemDelegate.Broadcast(EquippedWeapon->GetInventoryIndex(), newWeapon->GetInventoryIndex());
	}
#pragma endregion


	if (EquippedWeapon) 
	{
		EquippedWeapon->ChangeToPickedUp();
	}

	EquipedWeaponInventoryIndex = newWeapon->GetInventoryIndex();
	newWeapon->SetToEquipped(this);

	EquippedWeapon = newWeapon;
	PreWeaponInventoryIndex = EquippedWeapon->GetInventoryIndex();
#pragma region AttachNewWeapon
	const USkeletalMeshSocket* rightHandleSocket = GetMesh()->GetSocketByName(FName(TEXT("right_hand_socket")));
	ensure(rightHandleSocket);

	if (rightHandleSocket)
	{
		rightHandleSocket->AttachActor(newWeapon, GetMesh());
	}
#pragma endregion

	
#pragma region PlayEquipAnim
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (EquipWeaponMontage)
	{
		CombatState = ECombatState::ECS_Equipping;

		animInst->Montage_Play(EquipWeaponMontage);
		animInst->Montage_JumpToSection(TEXT("Equip"));
	}
#pragma endregion

	
}


void AQShooterCharacter::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		//EquippedWeapon->ChangeToFalling();
		//EquippedWeapon->ThrowItem();
		DropItem(EquippedWeapon);
		//Remove from inventory
		EquippedWeapon = nullptr;
	}
}

bool AQShooterCharacter::RemoveFromInventory(AQItem* itemToRemove)
{
	bool success = false;
	//现在只有weapon在inventory中，先用inventoryWeapons
	int32 itemIndex = InventoryWeapons.Find(itemToRemove);
	if (INDEX_NONE != itemIndex)
	{
		InventoryWeapons[itemIndex] = nullptr;
		itemToRemove->SetInventoryIndex(INDEX_NONE);
		success = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Try to drop an item %s not in inventory"), *itemToRemove->GetName());
	}
	return success;
}



void AQShooterCharacter::DropItem(AQItem* itemToDrop)
{
	int32 itemIndex = itemToDrop->GetInventoryIndex();
	bool removeSuccess = RemoveFromInventory(itemToDrop);
	if (removeSuccess)
	{
		//broacast ui events
		//EquipItemDelegate.Broadcast(itemIndex, -1); //用invdex_none -1作为newweapon index表示drop weapon

		itemToDrop->ChangeToFalling();
		itemToDrop->ThrowItem();
	}
}

int32 AQShooterCharacter::AddItemToInventory(AQItem* itemToDrop)
{
	int32 resItemIdex = INDEX_NONE;

	//现在只有weapon在inventory中，先用inventoryWeapons
	if (INDEX_NONE == InventoryWeapons.Find(itemToDrop)) //itemToAdd no exist in inventory
	{
		int32 emptSlot = InventoryWeapons.Find(nullptr);
		if (INDEX_NONE == emptSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("inventory is full"));
		}
		else
		{
			InventoryWeapons[emptSlot] = itemToDrop;
			resItemIdex = emptSlot;

			itemToDrop->SetInventoryIndex(emptSlot);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Try to add an existing item %s not in inventory"), *itemToDrop->GetName());
	}
	return resItemIdex;
}

bool AQShooterCharacter::IsInventoryFull()
{
	//InventoryWeapons.Num() < INVENTORY_CAPCITY;
	return InventoryWeapons.Find(nullptr) == INDEX_NONE;
}

void AQShooterCharacter::SwapWeapon(AQWeapon* targetWeapon)
{
	//if (CombatState != ECombatState::ECS_Unoccupied)
	//{
	//	return;
	//}
	if (targetWeapon &&
		 (targetWeapon != EquippedWeapon))
	{
		DropEquippedWeapon();
		AddItemToInventory(targetWeapon);

		//UE_LOG(LogTemp, Error, TEXT("SwapWepon preWeapon index %d"), preWeapon->GetInventoryIndex());
		EquipWeapon(targetWeapon);
	}
}

void AQShooterCharacter::FireButtonPressed()
{
	if (nullptr == EquippedWeapon)
	{
		return;
	}
	bIsFireButtonPressed = true;
	TryFireWeapon();

	if (EquippedWeapon->IsAutomatic())
	{
		// 启动一个timer来自动射击
		// 这个timer用来处理auto fire的逻辑,要求autofireduration > fireDuration
		GetWorld()->GetTimerManager().SetTimer(
			AutoFireTimerHandle, this, &AQShooterCharacter::AutoFireCheckTimer, EquippedWeapon->GetAutoFireRate(), true);
	}
}

void AQShooterCharacter::FireButtonReleased()
{
	bIsFireButtonPressed = false;

	// 停止自动射击timer
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
}

void AQShooterCharacter::TryFireWeapon()
{
	if ( (nullptr == EquippedWeapon) ||
		(ECombatState::ECS_Unoccupied != GetCombatState()))
	{
		return;
	}
	
	if (EquippedWeapon->HasAmmo())
	{
		CombatState = ECombatState::ECS_FireInProgress;
		FireOneBulletEffects();
		EquippedWeapon->FireOneBullet();
		bIsBulletFiring = true;


		// 设置timer 调用end fire用来处理射击间隔，这个是枪的物理射速上限
		GetWorld()->GetTimerManager().SetTimer(EndBulletFireTimerHandle, this, &AQShooterCharacter::EndFireBullet
			, FireDuration, false);

	}
	else
	{
		StartReloadWeapon();
	}
	
}

void AQShooterCharacter::AutoFireCheckTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("End Firing"));
	
	/*bool isEquipedWeaponHasAmmo = EquippedWeapon && EquippedWeapon->HasAmmo();
	if (bIsFireButtonPressed && isEquipedWeaponHasAmmo)
	{
		FireWeapon();
	}*/
	if (bIsFireButtonPressed)
	{
		TryFireWeapon();
	}
}


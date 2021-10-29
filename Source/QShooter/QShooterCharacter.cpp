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


// Sets default values
AQShooterCharacter::AQShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f);

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

}

void AQShooterCharacter::IncreaseOverlapItemCount()
{
	OverlapItemCount++;
}

void AQShooterCharacter::DecreaseOverlapItemCount()
{
	OverlapItemCount--;
}

// Called when the game starts or when spawned
void AQShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraDefaultFOV = FollowCamera->FieldOfView;
	CameraCurrenFOV = CameraDefaultFOV;

	SpawnAndEquipDefaultWeapon();

	AmmoAmountInitial();
}

void AQShooterCharacter::AimButtonPressed()
{
	bIsAimming = true;
}

void AQShooterCharacter::AimButtonReleased()
{
	bIsAimming = false;
}

float AQShooterCharacter::GetCrosshairSpeadMultiplier() const
{
	return CrosshairSpeadMultiplier;
}

FVector AQShooterCharacter::CalLocation4ItemCollectAnim()
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

	const FVector targetPointCS(CameraLerpPointDeltaX, 0.0f, CameraLerpPointDeltaZ);
	//FollowCamera->world
	const FVector targetPointWS = FollowCamera->GetComponentTransform().TransformPosition(targetPointCS);

	DrawDebugPoint(GetWorld(), targetPointWS, 20.0f, FColor::Red, true);
	return targetPointWS;
}

void AQShooterCharacter::CollectItem(AQItem* toCollectItem)
{
	if (!toCollectItem)
		return;

	if (AQWeapon* weapon = Cast<AQWeapon>(toCollectItem))
	{
		SwapWeapon(weapon);
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
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, debugMsg);
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

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("AimButton"), IE_Pressed, this, &AQShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("AimButton"), IE_Released, this, &AQShooterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction(TEXT("FireWeapon"), IE_Pressed, this, &AQShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("FireWeapon"), IE_Released, this, &AQShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction(TEXT("SelectButton"), IE_Pressed, this, &AQShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction(TEXT("SelectButton"), IE_Released, this, &AQShooterCharacter::SelectButtonReleased);
	
	PlayerInputComponent->BindAction(TEXT("ThrowWeapon"), IE_Pressed, this, &AQShooterCharacter::DropEquippedWeapon);
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

// 现在来看这里的逻辑移动到weapon或许是一个合理的选择，教学视频中没有这么做，先放在这
void AQShooterCharacter::FireOneBullet()
{
	if (!EquippedWeapon || !EquippedWeapon->HasAmmo())
	{
		return;
	}

	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, FireSoundCue);
	}
	
	const USkeletalMeshSocket* barrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName(TEXT("BarrelSocket"));
	if (barrelSocket)
	{
		const FTransform socketTransform = barrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, socketTransform);
		}
#pragma region SpawnHitFXAndTrailFX
		

		//计算bulletTrailEndPoint
		// 需要line trace 两次，一次以crosshair为起点，一次以barrel socket为起点
		FVector bulletTrailEndPoint;
		bool bBulletHitSomething = CalBulletTrailEndPointAndIfHitSth(socketTransform, bulletTrailEndPoint);

		if (ImpactHitFX && bBulletHitSomething)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactHitFX, bulletTrailEndPoint);
		}

		if (BulletTrailFX)
		{
			UParticleSystemComponent* bulletParticleCom =
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailFX, socketTransform);

			bulletParticleCom->SetVectorParameter(FName(TEXT("Target")), bulletTrailEndPoint);
		}
#pragma endregion
	}

	UAnimInstance* characterAnimInstance = GetMesh()->GetAnimInstance();
	if (characterAnimInstance && FireAnimMontage)
	{
		characterAnimInstance->Montage_Play(FireAnimMontage);
		characterAnimInstance->Montage_JumpToSection(TEXT("StartFire"));
	}
	
	EquippedWeapon->FireOneBullet();

}

void AQShooterCharacter::EndFireBullet()
{
	bIsBulletFiring = false;
	UE_LOG(LogTemp, Warning, TEXT("End Firing"));
}

void AQShooterCharacter::SelectButtonPressed()
{
	//DropEquippedWeapon();
	if (FocusedWeapon )
	{
		FocusedWeapon->StartCollectLerping(this);
		
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

bool AQShooterCharacter::CalBulletTrailEndPointAndIfHitSth(const FTransform& socketTransform, OUT FVector& bulletTrailEndPoint)
{
	FVector startLocation;
	FVector endLocation;
	GenerateCrosshairPoints4LineTrace(startLocation, endLocation);

	//计算bulletTrailEndPoint
	// 需要line trace 两次，一次以crosshair为起点，一次以barrel socket为起点
	bulletTrailEndPoint = endLocation;
	bool bBulletHitSomething = false;

	FHitResult fireHitRes;
	GetWorld()->LineTraceSingleByChannel(fireHitRes, startLocation,
		endLocation, ECollisionChannel::ECC_Visibility, FCollisionQueryParams::DefaultQueryParam);

	if (fireHitRes.bBlockingHit)
	{
		bulletTrailEndPoint = fireHitRes.Location;
		bBulletHitSomething = true;
	}

	if (bBulletHitSomething) //第一次line trace碰到东西才进行第二次
	{
		FHitResult bulletHitRes;
		GetWorld()->LineTraceSingleByChannel(bulletHitRes,
			socketTransform.GetLocation(),
			bulletTrailEndPoint,
			ECollisionChannel::ECC_Visibility);
		if (bulletHitRes.bBlockingHit)
		{
			bulletTrailEndPoint = bulletHitRes.Location;
		}
	}

	return bBulletHitSomething;
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
	// 没有overlap item则不需要line trace
	if (OverlapItemCount <= 0)
		return;

	//重置focus结果，trace有结果后赋值
	FocusedWeapon = nullptr;

	FHitResult itemTraceRes;
	LineTraceFromCrosshair(itemTraceRes);
	if (itemTraceRes.bBlockingHit)
	{
		AQItem* item = Cast<AQItem>(itemTraceRes.GetActor());
		if (item)
		{
			item->ShowItem();
		}
		if (AQWeapon* weapon = Cast<AQWeapon>(item))
		{
			FocusedWeapon = weapon;
		}
	}
}

void AQShooterCharacter::SpawnAndEquipDefaultWeapon()
{
	if (DefaulWeaponClass)
	{
		AQWeapon* newWeapon = GetWorld()->SpawnActor<AQWeapon>(DefaulWeaponClass);

		EquipWeapon(newWeapon);
	}
}

void AQShooterCharacter::EquipWeapon(AQWeapon* newWeapon)
{
	newWeapon->SetToEquipped();

	const USkeletalMeshSocket* rightHandleSocket = GetMesh()->GetSocketByName(FName(TEXT("right_hand_socket")));
	ensure(rightHandleSocket);

	if (rightHandleSocket)
	{
		rightHandleSocket->AttachActor(newWeapon, GetMesh());
	}
	EquippedWeapon = newWeapon;
}

void AQShooterCharacter::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ChangeToFalling();
		EquippedWeapon->ThrowWeapon();

		EquippedWeapon = nullptr;
	}
}

void AQShooterCharacter::SwapWeapon(AQWeapon* targetWeapon)
{
	if (targetWeapon &&
		 (targetWeapon != EquippedWeapon))
	{
		DropEquippedWeapon();
		EquipWeapon(targetWeapon);
	}
}

void AQShooterCharacter::FireButtonPressed()
{
	if (!EquippedWeapon) //没有装备武器，不能射击
	{
		return; 
	}

	if (EquippedWeapon->HasAmmo())
	{
		bIsFireButtonPressed = true;
		StartFireTimer();
	}
	else
	{
		// Try to Reload Ammo
	}
}

void AQShooterCharacter::FireButtonReleased()
{
	bIsFireButtonPressed = false;
}

void AQShooterCharacter::StartFireTimer()
{
	if (bIsBulletFiring)
		return;
	bIsBulletFiring = true;

	FireOneBullet();

	// 设置timer 调用end fire用来处理射击间隔
	GetWorld()->GetTimerManager().SetTimer(EndBulletFireTimerHandle, this, &AQShooterCharacter::EndFireBullet
		, FireDuration, false);

	// 这个timer用来处理auto fire的逻辑,要求autofireduration > fireDuration
	GetWorld()->GetTimerManager().SetTimer(
		AutoFireTimerHandle, this, &AQShooterCharacter::AutoFireTimer, AutoFireDuration, false);
}

void AQShooterCharacter::AutoFireTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("End Firing"));
	
	bool isEquipedWeaponHasAmmo = EquippedWeapon && EquippedWeapon->HasAmmo();
	if (bIsFireButtonPressed && isEquipedWeaponHasAmmo)
	{
		StartFireTimer();
	}
}


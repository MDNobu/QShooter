// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "QShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),  //未射击的状态，可以进入到下面两个状态中
	ECS_FireInProgress UMETA(DisplayName = "FireInProgress"), // 正在射击的状态，只能到unoccupied
	ECS_Reloading UMETA(DisplayName = "Reloading"),  //正在reload, 只能到unoccupied

	ECS_MAX  UMETA(DisplayName = "MAX", Hidden)
};


UCLASS()
class QSHOOTER_API AQShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQShooterCharacter();

	void IncreaseOverlapItemCount();
	void DecreaseOverlapItemCount();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void AimButtonPressed();
	void AimButtonReleased();

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateCameraZoom(float deltaTime);

	void UpdateCrosshairSpeadMultiplier(float deltaTime);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool GetIsAimming() const { return bIsAimming; };


	UFUNCTION(BlueprintCallable, Category = "Shooter")
	float GetCrosshairSpeadMultiplier() const;

	/** 计算一个相机前上方的坐标，以便物品收集动画作为lerp target */
	FVector CalLocation4ItemCollectAnim();

	/** 实际完成item收集的方法，按下收集按键时只是开始item过渡动画 */
	void CollectItem(class AQItem* toCollectItem);

	FORCEINLINE class UCameraComponent* GetFollowCamera() { return FollowCamera; };

	/**  must be called after HasSuitableAmmoPack */
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void EndReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void GrabClip();
	
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void InsertClip();

#pragma region GetAnSetters
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetIsCrouching() const { return bIsCrouching; }
#pragma endregion



protected:

	void FireButtonPressed();
	void FireButtonReleased();

	/** 进行射击，可以是不停点击射击的点射调用的 或者按住射击键的连射调用的 */
	void TryFireWeapon();
	void AutoFireCheckTimer();

	void EndFireBullet();


private:
	void CrouchButtonPressed();

	void JumpButtonPressed();

	void MoveForward(float value);
	void MoveRight(float value);

	/**
	 * @param rate表示的是-1.0到1.0的数字
	 */
	void TurnAtRate(float rate);

	/**
	 * @param rate表示的是-1.0到1.0的数字
	 */
	void LookUpAtRate(float rate);

	void Turn(float value);
	void Lookup(float value);

	/**
	 * 开一发枪的表现相关的逻辑，包括特效、生效的spawn等等
	 * 现在来看这里的逻辑移动到weapon或许是一个合理的选择，教学视频中没有这么做，先放在这
	 */
	void FireOneBulletEffects();

	//void EndFireBullet();

	bool CalBulletTrailEndPointAndIfHitSth(const FTransform& socketTransform, OUT FVector& bulletTrailEndPoint);
	void UpdateCameraRotateRateByIsAimming();
	
	bool LineTraceFromCrosshair(FHitResult& hitResult);

	/**
	 * 从准星生成WS两个点，以进行linetrace
	 * startPoint 用于linetrace的起点， 
	 * endPoint 拥有linetrace 的终点
	 */
	void GenerateCrosshairPoints4LineTrace(OUT FVector& startPoint,OUT FVector& endPoint);
	void LineTraceToShowItems();

	void SpawnAndEquipDefaultWeapon();

	void EquipWeapon(class AQWeapon* newWeapon);

	void DropEquippedWeapon();

	void SwapWeapon(AQWeapon* targetWeapon);

	void SelectButtonPressed();
	void SelectButtonReleased();
	void AmmoAmountInitial();

	bool DoesEquippedWeaponHasAmmo();

	void StartReloadWeapon();
	

	void ReloadButtonPressed();
	bool HasSuitableAmmoPack();
	/** 更新clip的位置，主要是处理换弹夹时clip的位置随手而移动的问题 */
	void UpdateClipTransform();
private:


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true, ClampMax = 12.00))
	class UCameraComponent* FollowCamera = nullptr;

#pragma region Keyboard CameraRotateRateParams
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseLookUpRate = 45.0f;

	/** Hip 指没有瞄准的状态， HipTurnRate表示未瞄准状态下的相机旋转速度-用方向键或者手柄的情况下 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HipTurnRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HipLookupRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimTurnRate = 25.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AimLookupRate = 25.0f;
#pragma endregion

#pragma region MouseCameraRotateRateParams
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 1.00, ClampMin = 0.00, ContentDir, UIMax = 1.00, UIMin = 0.00))
	float HipMouseTurnRate = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 1.00, ClampMin = 0.00, ContentDir, UIMax = 1.00, UIMin = 0.00))
	float HipMouseLookupRate = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 1.00, ClampMin = 0.00, ContentDir, UIMax = 1.00, UIMin = 0.00))
	float AimMouseTurnRate = 0.3f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 1.00, ClampMin = 0.00, ContentDir, UIMax = 1.00, UIMin = 0.00))
	float AimMouseLookupRate = 0.3f;

	float CurrentMouseTurnRate = 0.0f;
	float CurrentMouseLookupRate = 0.0f;
#pragma endregion

#pragma region CrosshairSpreadParams
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrosshairSpeadMultiplier = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrosshairVelocityFactor = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrosshairInAirFactor = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrosshairAimFactor = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrosshairShootingFactor = 0.0f;
#pragma endregion


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* FireSoundCue = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UAnimMontage* FireAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UParticleSystem* ImpactHitFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UParticleSystem* BulletTrailFX = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInFOV = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInterpSpeed = 20.0f;

	float CameraDefaultFOV = 90.0f;

	float CameraCurrenFOV = 0.0f;

	bool bIsAimming = false;

	/** 表示当前是否有子弹正在fire,主要用来射击时 crosshair的缩放 */
	bool bIsBulletFiring = false;

	/**  */
	//bool bCanFire = true;

	/** 子弹的射击间隔，射速的倒数，枪的物理射速 */
	float FireDuration = 60.0f / 600.0f;

	/**  处理子弹射击间隔的handle */
	FTimerHandle EndBulletFireTimerHandle;


	bool bIsFireButtonPressed = false;

	/** 处理automatic fire的handle */
	FTimerHandle AutoFireTimerHandle;

	/** 自动射击的射击间隔， 下面的表示每分钟120发， 注意这个值要大于FireDuration */
	float AutoFireDuration = 60.0f / 300.0f;


	/** 当前角色和多少item的trigger volume重叠，主要用于决定是否line trace, 一般来说游戏中item不会很多，不需要关注越界问题 */
	int16 OverlapItemCount = 0;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQWeapon* EquippedWeapon = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQWeapon* FocusedWeapon = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TSubclassOf<AQWeapon> DefaulWeaponClass;

	/**  这个是用来调整item 收集动画的target point的 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraTargetOffset = 60.0f;

	/**  这个是用来调整item 收集动画的target point的 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraLerpPointDeltaX = 50.0f;
	/**  这个是用来调整item 收集动画的target point的 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraLerpPointDeltaZ = 50.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TMap<EAmmoType, int32> CurAmmoAmounts;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ToolTip = "ammo initial amount"))
	TMap<EAmmoType, int32> AmmoInitAmounts;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

#pragma region VariableForMoveClip
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsClipMoving = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* SceneCom4MoveClip = nullptr;

	FTransform ClipTransWhenDetached;

#pragma endregion

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsCrouching = false;
};

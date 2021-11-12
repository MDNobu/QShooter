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
	ECS_Equipping UMETA(DisplayName = "Equiping"),

	ECS_MAX  UMETA(DisplayName = "MAX", Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, curSlotIndex, int32, newSlotIndex);
/** bIsStart表示 开始动画还是停止动画， true == start , false == stop */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightInventorySlotDelegate, int32, slotIndex, bool, bIsStart);


USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()
public:
	FInterpLocation() : InterpSceneCom(nullptr), InterpNum(0)
	{

	}
	FInterpLocation(USceneComponent* sceneCom, int32 num) : InterpSceneCom(sceneCom), InterpNum(num)
	{
	}
	/** 作为collect item popup 动画,interp target的scenecom */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter")
	USceneComponent* InterpSceneCom;

	/** 朝这个scene interp的item数量 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter")
	int32 InterpNum;
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
	

	void Jump() override;


	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	FVector CalLocation4ItemCollectAnim(OUT int& interpSlotIndex,class AQItem* targetItem);

	/** 实际完成item收集的方法，按下收集按键时只是开始item过渡动画 */
	void EndCollectItem(class AQItem* toCollectItem);

	FORCEINLINE class UCameraComponent* GetFollowCamera() { return FollowCamera; };

	/**  must be called after HasSuitableAmmoPack */
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void EndReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void EndEquipping();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void GrabClip();
	
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void InsertClip();




protected:
#pragma region InputBinds
	void FireButtonPressed();
	void FireButtonReleased();

	void AimButtonPressed();
	void AimButtonReleased();
#pragma endregion

	/** 进行射击，可以是不停点击射击的点射调用的 或者按住射击键的连射调用的 */
	void TryFireWeapon();
	void AutoFireCheckTimer();

	void EndFireBullet();

	UFUNCTION(BlueprintPure, Category = "QShooter")
	bool IsInventoryFull();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	EPhysicalSurface LineTraceSurfaceType();
private:

#pragma region InputBinds
	void SelectButtonPressed();
	void SelectButtonReleased();
	void ReloadButtonPressed();

	// Key binding function to handle switching equiping weapon
	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();
#pragma endregion
	void ToggleCrouch();

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
	void ChangeEquipWeapon(int32 newWeaponInventIndex);

	/** 进行两次linetrace， 一次从crosshair出发， 一次从barrel到第一次的终点， 结果取靠后的 */
	bool LineTrace4Bullet(const FTransform& socketTransform, OUT FHitResult& bulletTrailEndPoint);
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

	/** 必须在newWeapon添加到inventory之后才能调用 */
	//void EquipWeapon(class AQWeapon* newWeapon);
	void EquipWeapon(class AQWeapon* newWeapon);

	void DropEquippedWeapon();

	void DropItem(AQItem* itemToDrop);

	/**
	 * 添加item到inventory，1.如果已经存在不添加 2.inventory已满不添加
	 * 成功添加时返回item所在的index,否则返回 INDEX_NONE
	 */  
	int32 AddItemToInventory(AQItem* itemToDrop);
	bool RemoveFromInventory(AQItem* itemToRemove);

	

	void SwapWeapon(AQWeapon* targetWeapon);

	void AmmoAmountInitial();

	bool DoesEquippedWeaponHasAmmo();

	void StartReloadWeapon();
	

	bool HasSuitableAmmoPack();


	/** 根据crouch 与否，lerp capsule half height */
	void UpdateCapsuleHalfHeight(float DeltaTime);


	void StartAim();
	void StopAim();
	void ConsumeAmmo(class AQAmmo* ammo);
	void InitInterpLocations();

	FInterpLocation GetItemCollectInterpLocation(int32 index);
	void UpdateHighlightInventory();
	
private:
#pragma region Components
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true, ClampMax = 12.00))
	class UCameraComponent* FollowCamera = nullptr;

#pragma endregion



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

#pragma region Variable4Anim
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UAnimMontage* EquipWeaponMontage = nullptr;

#pragma endregion


	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	//class USoundCue* FireSoundCue = nullptr;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	//class UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UAnimMontage* FireAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UParticleSystem* ImpactHitFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UParticleSystem* BulletTrailFX = nullptr;

	/** 武器collider碰到player时的sound */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ImpactSoundCue = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInFOV = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInterpSpeed = 20.0f;

	float CameraDefaultFOV = 90.0f;

	float CameraCurrenFOV = 0.0f;


	bool bIsAimButtonPressed = false;
	/** 注意AimButtonPress为true，不一定IsAim为true */
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


	/** 只是给inventory动画用的 */
	int32 PreWeaponInventoryIndex = INDEX_NONE;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQItem* FocusedItem = nullptr;
	/** 前一帧focused item*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQItem* PreFocusedItem = nullptr;

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

	/** 角色身上装的弹药,不包括当前武器中的 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TMap<EAmmoType, int32> CurAmmoAmounts;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ToolTip = "ammo initial amount"))
	TMap<EAmmoType, int32> AmmoInitAmounts;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float Health = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float MaxHealth = 100.0f;


#pragma region VariableForMoveClip
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsClipMoving = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* SceneCom4MoveClip = nullptr;

	FTransform ClipTransWhenDetached;

#pragma endregion

#pragma region Params4CrouchOrNot
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseMaxWalkSpeed = 650.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrouchMaxWalkSpeed = 300.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseGroundFirction = 8.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrouchGroundFirction = 16.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseCapsuleHalfHeight = 88.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CrouchCapsuleHalfHeight = 44.0f;
#pragma endregion



	
	/** 这部分主要是为了收集到weapon/ammo之类的item的pop up动画，应该有更好的实现，先这么做 */
#pragma region Components4ItemCollectAnim
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponSceneCom = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom1 = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom2 = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom3 = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom4 = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom5 = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpCom6 = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TArray<FInterpLocation> ItemCollectInterpSlots;
#pragma endregion
	
#pragma region InventoryParams
	/** 现在inventory限制在6 */
	const int32 INVENTORY_CAPCITY = 6;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TArray<AQItem*> InventoryWeapons;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 EquipedWeaponInventoryIndex = 0;

	/** 装备武器时inventory收到slot information的delegate */
	UPROPERTY(BlueprintAssignable, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FEquipItemDelegate EquipItemDelegate;

	/** delegate to send slot information to inventory ui */
	UPROPERTY(BlueprintAssignable, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FHighlightInventorySlotDelegate InventorySlotHighlightDelegate;

	int32 highlightingInventorySlotIndex = INDEX_NONE;
#pragma endregion

public:
#pragma region GetterAnSetters
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetIsCrouching() const { return bIsCrouching; }
	FORCEINLINE class AQWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	FORCEINLINE USoundCue* GetImpactSoundCue() const { return ImpactSoundCue; }
#pragma endregion
};

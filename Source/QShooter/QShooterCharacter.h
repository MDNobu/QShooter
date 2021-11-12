// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "QShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),  //δ�����״̬�����Խ��뵽��������״̬��
	ECS_FireInProgress UMETA(DisplayName = "FireInProgress"), // ���������״̬��ֻ�ܵ�unoccupied
	ECS_Reloading UMETA(DisplayName = "Reloading"),  //����reload, ֻ�ܵ�unoccupied
	ECS_Equipping UMETA(DisplayName = "Equiping"),

	ECS_MAX  UMETA(DisplayName = "MAX", Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, curSlotIndex, int32, newSlotIndex);
/** bIsStart��ʾ ��ʼ��������ֹͣ������ true == start , false == stop */
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
	/** ��Ϊcollect item popup ����,interp target��scenecom */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter")
	USceneComponent* InterpSceneCom;

	/** �����scene interp��item���� */
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

	/** ����һ�����ǰ�Ϸ������꣬�Ա���Ʒ�ռ�������Ϊlerp target */
	FVector CalLocation4ItemCollectAnim(OUT int& interpSlotIndex,class AQItem* targetItem);

	/** ʵ�����item�ռ��ķ����������ռ�����ʱֻ�ǿ�ʼitem���ɶ��� */
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

	/** ��������������ǲ�ͣ�������ĵ�����õ� ���߰�ס�������������õ� */
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
	 * @param rate��ʾ����-1.0��1.0������
	 */
	void TurnAtRate(float rate);

	/**
	 * @param rate��ʾ����-1.0��1.0������
	 */
	void LookUpAtRate(float rate);

	void Turn(float value);
	void Lookup(float value);

	/**
	 * ��һ��ǹ�ı�����ص��߼���������Ч����Ч��spawn�ȵ�
	 * ��������������߼��ƶ���weapon������һ�������ѡ�񣬽�ѧ��Ƶ��û����ô�����ȷ�����
	 */
	void FireOneBulletEffects();

	//void EndFireBullet();
	void ChangeEquipWeapon(int32 newWeaponInventIndex);

	/** ��������linetrace�� һ�δ�crosshair������ һ�δ�barrel����һ�ε��յ㣬 ���ȡ����� */
	bool LineTrace4Bullet(const FTransform& socketTransform, OUT FHitResult& bulletTrailEndPoint);
	void UpdateCameraRotateRateByIsAimming();
	
	bool LineTraceFromCrosshair(FHitResult& hitResult);

	/**
	 * ��׼������WS�����㣬�Խ���linetrace
	 * startPoint ����linetrace����㣬 
	 * endPoint ӵ��linetrace ���յ�
	 */
	void GenerateCrosshairPoints4LineTrace(OUT FVector& startPoint,OUT FVector& endPoint);
	void LineTraceToShowItems();

	void SpawnAndEquipDefaultWeapon();

	/** ������newWeapon��ӵ�inventory֮����ܵ��� */
	//void EquipWeapon(class AQWeapon* newWeapon);
	void EquipWeapon(class AQWeapon* newWeapon);

	void DropEquippedWeapon();

	void DropItem(AQItem* itemToDrop);

	/**
	 * ���item��inventory��1.����Ѿ����ڲ���� 2.inventory���������
	 * �ɹ����ʱ����item���ڵ�index,���򷵻� INDEX_NONE
	 */  
	int32 AddItemToInventory(AQItem* itemToDrop);
	bool RemoveFromInventory(AQItem* itemToRemove);

	

	void SwapWeapon(AQWeapon* targetWeapon);

	void AmmoAmountInitial();

	bool DoesEquippedWeaponHasAmmo();

	void StartReloadWeapon();
	

	bool HasSuitableAmmoPack();


	/** ����crouch ���lerp capsule half height */
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

	/** Hip ָû����׼��״̬�� HipTurnRate��ʾδ��׼״̬�µ������ת�ٶ�-�÷���������ֱ�������� */
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

	/** ����collider����playerʱ��sound */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ImpactSoundCue = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInFOV = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraZoomInterpSpeed = 20.0f;

	float CameraDefaultFOV = 90.0f;

	float CameraCurrenFOV = 0.0f;


	bool bIsAimButtonPressed = false;
	/** ע��AimButtonPressΪtrue����һ��IsAimΪtrue */
	bool bIsAimming = false;

	/** ��ʾ��ǰ�Ƿ����ӵ�����fire,��Ҫ�������ʱ crosshair������ */
	bool bIsBulletFiring = false;

	/**  */
	//bool bCanFire = true;

	/** �ӵ��������������ٵĵ�����ǹ���������� */
	float FireDuration = 60.0f / 600.0f;

	/**  �����ӵ���������handle */
	FTimerHandle EndBulletFireTimerHandle;


	bool bIsFireButtonPressed = false;

	/** ����automatic fire��handle */
	FTimerHandle AutoFireTimerHandle;

	/** �Զ�������������� ����ı�ʾÿ����120���� ע�����ֵҪ����FireDuration */
	float AutoFireDuration = 60.0f / 300.0f;


	/** ��ǰ��ɫ�Ͷ���item��trigger volume�ص�����Ҫ���ھ����Ƿ�line trace, һ����˵��Ϸ��item����ܶ࣬����Ҫ��עԽ������ */
	int16 OverlapItemCount = 0;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQWeapon* EquippedWeapon = nullptr;


	/** ֻ�Ǹ�inventory�����õ� */
	int32 PreWeaponInventoryIndex = INDEX_NONE;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQItem* FocusedItem = nullptr;
	/** ǰһ֡focused item*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQItem* PreFocusedItem = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TSubclassOf<AQWeapon> DefaulWeaponClass;

	/**  �������������item �ռ�������target point�� */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraTargetOffset = 60.0f;

	/**  �������������item �ռ�������target point�� */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraLerpPointDeltaX = 50.0f;
	/**  �������������item �ռ�������target point�� */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float CameraLerpPointDeltaZ = 50.0f;

	/** ��ɫ����װ�ĵ�ҩ,��������ǰ�����е� */
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



	
	/** �ⲿ����Ҫ��Ϊ���ռ���weapon/ammo֮���item��pop up������Ӧ���и��õ�ʵ�֣�����ô�� */
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
	/** ����inventory������6 */
	const int32 INVENTORY_CAPCITY = 6;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TArray<AQItem*> InventoryWeapons;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 EquipedWeaponInventoryIndex = 0;

	/** װ������ʱinventory�յ�slot information��delegate */
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

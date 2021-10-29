// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_AR UMETA(DisplayName = "AR"),

	EAT_MAX  UMETA(DisplayName = "MAX", Hidden)
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

	/** ����һ�����ǰ�Ϸ������꣬�Ա���Ʒ�ռ�������Ϊlerp target */
	FVector CalLocation4ItemCollectAnim();

	/** ʵ�����item�ռ��ķ����������ռ�����ʱֻ�ǿ�ʼitem���ɶ��� */
	void CollectItem(class AQItem* toCollectItem);

	FORCEINLINE class UCameraComponent* GetFollowCamera() { return FollowCamera; };

protected:

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();
	void AutoFireTimer();

	void EndFireBullet();



private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom  = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true, ClampMax = 12.00))
	class UCameraComponent* FollowCamera = nullptr;

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
	 * ��һ��ǹ���߼���������Ч����Ч��spawn�ȵ�
	 */
	void FireOneBullet();

	//void EndFireBullet();

	bool CalBulletTrailEndPointAndIfHitSth(const FTransform& socketTransform, OUT FVector& bulletTrailEndPoint);
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

	void EquipWeapon(class AQWeapon* newWeapon);

	void DropEquippedWeapon();

	void SwapWeapon(AQWeapon* targetWeapon);

	void SelectButtonPressed();
	void SelectButtonReleased();
	void AmmoAmountInitial();
private:

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


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* FireSoundCue = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UAnimMontage* FireAnimMontage = nullptr;

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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQWeapon* FocusedWeapon = nullptr;

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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TMap<EAmmoType, int32> CurAmmoAmounts;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ToolTip = "ammo initial amount"))
	TMap<EAmmoType, int32> AmmoInitAmounts;
};

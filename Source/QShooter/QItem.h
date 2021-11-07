// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "QItem.generated.h"


USTRUCT(BlueprintType)
struct FItemRarityTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		FLinearColor GlowColor;

	/** item widget用的 light color */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		FLinearColor LightColor;

	/** item widget用的 dark color */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		FLinearColor DarkColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		int32 NumOfStars = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		UTexture2D* RarityImage;
};



UENUM(BlueprintType)
enum class EQItemRarity : uint8
{
	EIR_MIN = 0  UMETA(DisplayName = "MIN", Hidden),
	EIR_Damaged = 1 UMETA(DisplayName = "Damaged"),
	EIR_Common = 2 UMETA(DisplayName = "Common"),
	EIR_UnCommon = 3 UMETA(DisplayName = "UnCommon"),
	EIR_Rare = 4 UMETA(DisplayName = "Rare"),
	EIR_Mythic =5 UMETA(DisplayName = "Mythic"),

	EIR_MAX =6 UMETA(DisplayName = "MAX", Hidden)
};

UENUM(BlueprintType)
enum class EQItemState : uint8
{
	EIS_ToPickUp UMETA(DisplayName = "ToPickUp"), //待拾起的状态
	EIS_Equipped UMETA(DisplayName = "Equipped"), //被装备状态
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"), //从待捡起到被捡起的过渡
	EIS_PickedUp UMETA(DisplayName = "PickedUp"), //已捡起在仓库中未装备状态
	EIS_Falling UMETA(DisplayName = "Falling"), //丢弃到待捡起的过渡状态

	EIS_MAX  UMETA(DisplayName = "MAX", Hidden)
};

UCLASS()
class QSHOOTER_API AQItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

	void ShowItem();
	void ChangeToFalling();
	void ChangeToPickedUp();

	void StartCollectLerping(class AQShooterCharacter* character);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMeshComponent; };

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	virtual void ThrowItem();
protected:
	/** 注意这个方法只在状态变化时调用不要重复调用 */
	virtual void SetItemProperties(EQItemState targetItemState);

	/** 注意这里不仅仅是一个simple Setter的操作，还有setItemProperties,enable glow/disable glow等操作 */
	void ConfigItemState(EQItemState targetItemState);

private:
	UFUNCTION()
	void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnItemEndOverlap(
			UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
		);

	class AQShooterCharacter* CheckAndGetQShooterCharacter(AActor* actor);

	void EndCollectLerping();

	void CollectLerping(float deltaTime);

	void EnableGlowEffect();
	void DisableGlowEffect();

	void ResetDynamicGlowTimer_ToPickUp();
	void ResetDynamicGlowTimer_Interping();
	void UpdateDynamicGlowParams(float DeltaTime);
protected:

#pragma region Components
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 用来line trace以显示UI Widget, 角色是否line trace和ItemTriggerBox相关 */
	class UBoxComponent* ItemWidgetVisibilityBox = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 角色是否line trace和ItemTriggerBox相关  */
	class USphereComponent* ItemTriggerSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 用skeletal mesh是因为武器通常是skeletal mesh */
	USkeletalMeshComponent* ItemMeshComponent = nullptr;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Shooter", meta = (AllowPrivateAccess = true))
	class UWidgetComponent* ItemWidgetComponent = nullptr;
#pragma endregion


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EQItemState ItemState = EQItemState::EIS_ToPickUp;
private:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FString ItemName {TEXT("Default Gun") };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 999.00, ClampMin = 0.00, UIMax = 999.00, UIMin = 0.00))
	int32 ItemAmount = 60;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EQItemRarity ItemRarity = EQItemRarity::EIR_Common;

	
	/** item被收集时过渡UI动画用到的Z方向的Curve */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UCurveFloat* ItemZFloatCurve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UCurveFloat* ItemScaleCurve = nullptr;

	FVector CollectLerpStartLocation;

	/** item收集动画开始时得到的相机目标，暂时没考虑相机运动的问题 */
	FVector CameraTargetLocation;



	/** 当前帧是否应该显示 */
	bool bIsVisibleCurFrame = false;

	/** 前一帧是否显示 */
	bool bIsVisibleLastFrame = false;

	/** collect动画结束的handle */
	FTimerHandle CollectLerpEndHandle;

	/** collect动画的时间，用于time handle的设置和z curve的时间 */
	float CollectAnimDuraction = 2.0f;

	bool bIsCollectLerping = false;

	/** player character 的cache, item开始收集时获取， 丢弃时重置 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQShooterCharacter* QPlayerCharacter = nullptr;


	float ItemCameraYawOffset = 40.0f;

	/** sound played when picked up */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* PickupSound = nullptr;

	/** 装备武器时用的sound */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USoundCue* EquipSound = nullptr;

	/** collect popup动画时用的slot的cache,用-1表示为初始化的，因为0是有效的slot index */
	int32 InterpSlotIndex = -1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* ItemBackground = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UTexture2D* ItemIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 InventoryIndex = INDEX_NONE;
#pragma region Params4Material
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	int32 ItemMaterialIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UMaterialInstanceDynamic* ItemMaterialDynamic = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UMaterialInstance* ItemMaterial = nullptr;

	/** x,y,z 分别表示不同的意义，x表示glowEffectWeight, */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UCurveVector* GlowEffectCurve_ToPickUp = nullptr;

	/**  */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UCurveVector* GlowEffectCurve_Interping = nullptr;

	UPROPERTY(EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float GlowEffectWeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float FresnelExponent = 3.0f;

	UPROPERTY(EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float FresnelReflectFraction = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float PulseCurveTime_ToPickUp = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float PulseCurveTime_Interping = 0.7f;

	FTimerHandle DynamicGlowTimerHandle_ToPickUp;
	FTimerHandle DynamicGlowTimerHandle_Interping;
#pragma endregion

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UDataTable* RarityDataTable = nullptr;

public:
#pragma region GetterAndSetter
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemAmount() const { return ItemAmount; }
	FORCEINLINE  int32 GetInterpSlotIndex() const { return InterpSlotIndex; }
	FORCEINLINE int32 GetInventoryIndex() const { return InventoryIndex; }
	void SetInventoryIndex(int32 val) { InventoryIndex = val; }
#pragma endregion



	

};

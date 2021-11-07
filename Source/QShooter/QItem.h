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

	/** item widget�õ� light color */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooterr")
		FLinearColor LightColor;

	/** item widget�õ� dark color */
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
	EIS_ToPickUp UMETA(DisplayName = "ToPickUp"), //��ʰ���״̬
	EIS_Equipped UMETA(DisplayName = "Equipped"), //��װ��״̬
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"), //�Ӵ����𵽱�����Ĺ���
	EIS_PickedUp UMETA(DisplayName = "PickedUp"), //�Ѽ����ڲֿ���δװ��״̬
	EIS_Falling UMETA(DisplayName = "Falling"), //������������Ĺ���״̬

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
	/** ע���������ֻ��״̬�仯ʱ���ò�Ҫ�ظ����� */
	virtual void SetItemProperties(EQItemState targetItemState);

	/** ע�����ﲻ������һ��simple Setter�Ĳ���������setItemProperties,enable glow/disable glow�Ȳ��� */
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
	/** ����line trace����ʾUI Widget, ��ɫ�Ƿ�line trace��ItemTriggerBox��� */
	class UBoxComponent* ItemWidgetVisibilityBox = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** ��ɫ�Ƿ�line trace��ItemTriggerBox���  */
	class USphereComponent* ItemTriggerSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** ��skeletal mesh����Ϊ����ͨ����skeletal mesh */
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

	
	/** item���ռ�ʱ����UI�����õ���Z�����Curve */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UCurveFloat* ItemZFloatCurve = nullptr;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UCurveFloat* ItemScaleCurve = nullptr;

	FVector CollectLerpStartLocation;

	/** item�ռ�������ʼʱ�õ������Ŀ�꣬��ʱû��������˶������� */
	FVector CameraTargetLocation;



	/** ��ǰ֡�Ƿ�Ӧ����ʾ */
	bool bIsVisibleCurFrame = false;

	/** ǰһ֡�Ƿ���ʾ */
	bool bIsVisibleLastFrame = false;

	/** collect����������handle */
	FTimerHandle CollectLerpEndHandle;

	/** collect������ʱ�䣬����time handle�����ú�z curve��ʱ�� */
	float CollectAnimDuraction = 2.0f;

	bool bIsCollectLerping = false;

	/** player character ��cache, item��ʼ�ռ�ʱ��ȡ�� ����ʱ���� */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	AQShooterCharacter* QPlayerCharacter = nullptr;


	float ItemCameraYawOffset = 40.0f;

	/** sound played when picked up */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* PickupSound = nullptr;

	/** װ������ʱ�õ�sound */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USoundCue* EquipSound = nullptr;

	/** collect popup����ʱ�õ�slot��cache,��-1��ʾΪ��ʼ���ģ���Ϊ0����Ч��slot index */
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

	/** x,y,z �ֱ��ʾ��ͬ�����壬x��ʾglowEffectWeight, */
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

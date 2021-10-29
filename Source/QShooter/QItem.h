// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QItem.generated.h"

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

	void ShowItem();
	void ChangeToFalling();

	void StartCollectLerping(class AQShooterCharacter* character);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMeshComponent; };
protected:
	void SetItemProperties(EQItemState targetItemState);
	void SetItemState(EQItemState targetItemState);

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
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 用来line trace以显示UI Widget, 角色是否line trace和ItemTriggerBox相关 */
	class UBoxComponent* ItemWidgetVisibilityBox = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 角色是否line trace和ItemTriggerBox相关  */
	class USphereComponent* ItemTriggerSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EQItemState ItemState = EQItemState::EIS_ToPickUp;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** 用skeletal mesh是因为武器通常是skeletal mesh */
	USkeletalMeshComponent* ItemMeshComponent = nullptr;
private:


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Shooter", meta = (AllowPrivateAccess = true))
	class UWidgetComponent* ItemWidgetComponent = nullptr;


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
};

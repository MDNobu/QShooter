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
	/** ����line trace����ʾUI Widget, ��ɫ�Ƿ�line trace��ItemTriggerBox��� */
	class UBoxComponent* ItemWidgetVisibilityBox = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** ��ɫ�Ƿ�line trace��ItemTriggerBox���  */
	class USphereComponent* ItemTriggerSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	EQItemState ItemState = EQItemState::EIS_ToPickUp;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	/** ��skeletal mesh����Ϊ����ͨ����skeletal mesh */
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
};

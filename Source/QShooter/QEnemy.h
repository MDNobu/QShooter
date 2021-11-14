// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QBulletHitInterface.h"
#include "QEnemy.generated.h"

UCLASS()
class QSHOOTER_API AQEnemy : public ACharacter, public IQBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQEnemy();


	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent, Category = "QShooter")
	void ShowHealthBar();
	void BulletHit_Implementation(FHitResult hitResult) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "QShooter")
	void HideHealthBar();


	UFUNCTION(BlueprintImplementableEvent, Category = "QShooter")
	void ShowHitNumber(int32 damage, FVector demageLocation, bool bIsHeadshot);
	
	/**  注意这个函数虽然叫play attack montage，但不止包括表现的意义，相当于attack */
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void PlayAttackMontage(FName attackSectionName, float playRate = 1.0f);

	UFUNCTION(BlueprintPure, Category = "QShooter")
	FName SelectAttackSectionName() const;

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void ActivateLeftWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void DeactivateLeftWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void ActivateRightWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void DeactivateRightWeapon();

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void FinishDeath();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void StoreHitNumberLocation(class UUserWidget* widget, FVector location);

	UFUNCTION()
	void DestroyHitNumberWidget(UUserWidget* widget);

	/** 注意，这个set 不是一个simple setter */
	UFUNCTION(BlueprintCallable, Category = "QShooter")
	void SetIsStunning(bool isStunning);

	/** 注意，这个set 不是一个simple setter */
	void SetIsInCombatRange(bool isInCombatRange);
private:

	UFUNCTION()
	void OnLeftWeaponBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PlayBloodFX(class AQShooterCharacter* playerCharacter, FName socketName);

	void DoDamaget2Character(AQShooterCharacter* OtherActor);

	UFUNCTION()
	void OnRightWeaponBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Die();

	void PlayHitMontage(FName hitSectionName, float playRate = 1.0f);

	void ResetHitDelay();
	
	void UpdateHitNumberWidgets();


	UFUNCTION()
	void OnAggroSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCombatRangeSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCombatRangeSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
		);

	void DestoryEnemy();

	void EnableAttack();

	void SetCanAttackInBlackboard(bool canAttack);

	UFUNCTION()
	void OnPlayerCharacterDie();
private:
#pragma region Components
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USphereComponent* AggroSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	USphereComponent* CombatRangeSphere = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UBoxComponent* LeftWeaponCollisionBox = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UBoxComponent* RightWeaponCollisionBox = nullptr;
#pragma endregion

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsInCombatRange = false;

	

#pragma region Params4AI
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UBehaviorTree* BehaviorTree;

	/**  注意MakedEditWidget会使该location变成local space */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true, MakeEditWidget))
	FVector PatrolPointLS;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true, MakeEditWidget))
	FVector PatrolPoint2LS;

	/** Controller Cache */
	UPROPERTY(VisibleAnywhere, Category = "QShooter")
	class AQEnemyController* EnemyController = nullptr;
#pragma endregion



	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	bool bIsStunning = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true, ClampMax = 1.00, ClampMin = 0.00, UIMax = 1.00, UIMin = 0.00))
	float StunChance = 0.5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* ImpactFX = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ImpactSound = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float Health = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float MaxHealth = 100.0f;

	/** 一次武器击中造成的伤害 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseDamage = 20.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName HeadBoneName{ TEXT("head") };
	
	/** 敌人被击中后显示health bar的时间 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HealthBarShowDuration = 1.5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UAnimMontage* HitAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathAnimMontage = nullptr;

	bool bIsDying = false;

	FTimerHandle DeathDestoryTimer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float DestroyTimeAfterDie = 3.0f;

#pragma region AttackAnimParams
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName AttackLFast_SectionName{ TEXT("AttackLFast") };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName AttackRFast_SectionName {TEXT("AttackRFast")};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName AttackL_SectionName{ TEXT("AttackL") };

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName AttackR_SectionName{ TEXT("AttackR") };
#pragma endregion


	

	FTimerHandle HealthBarShowTimerHandle;
#pragma region Params4HitNumber
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	TMap<UUserWidget*, FVector> HitNumberWidgetLocations;


	//FTimerHandle HitNumberDestroyTimerHandle;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HitNumberWidgetShowTime = 2.0f;
#pragma endregion





	

#pragma region Param4HitDelay
	FTimerHandle HitDelayTimerHandle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HitDelayMin = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HitDelaMax = 2.0f;

	bool bCanPlayHitMontage = true;
#pragma endregion

	bool bCanAttack = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float AttackCD = 3.0f;

	FTimerHandle AttackCDTimer;
public:

#pragma region GetterAndSetters
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE FName GetHeadBoneName() const { return HeadBoneName; }

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
#pragma endregion
};

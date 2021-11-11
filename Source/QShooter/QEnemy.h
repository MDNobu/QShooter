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
private:

	void Die();

	void PlayHitMontage(FName hitSectionName, float playRate = 1.0f);

	void ResetHitDelay();
	
	void UpdateHitNumberWidgets();
private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* ImpactFX = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ImpactSound = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float Health = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	FName HeadBoneName{ TEXT("head") };
	
	/** 敌人被击中后显示health bar的时间 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float HealthBarShowDuration = 1.5f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UAnimMontage* HitAnimMontage = nullptr;

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

public:

#pragma region GetterAndSetters
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE FName GetHeadBoneName() const { return HeadBoneName; }
#pragma endregion
};

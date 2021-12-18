// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QBulletHitInterface.h"
#include "QExplosive.generated.h"

UCLASS()
class QSHOOTER_API AQExplosive : public AActor, public IQBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQExplosive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void BulletHit_Implementation(FHitResult hitResult, AActor* Shooter, AController* ShooterController) override;

private:

#pragma region Components
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* ExplosiveMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USphereComponent* DamageSphere = nullptr;
#pragma endregion


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* ExlpodeParticle = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ExplodeSound = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	float BaseDamage = 100.0f;
};

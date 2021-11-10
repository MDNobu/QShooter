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




	void BulletHit_Implementation(FHitResult hitResult) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* ImpactFX = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ImpactSound = nullptr;
};

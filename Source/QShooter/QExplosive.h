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
	void BulletHit_Implementation(FHitResult hitResult) override;

private:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UParticleSystem* ExlpodeParticle = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class USoundCue* ExplodeSound = nullptr;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "QExplosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AQExplosive::AQExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AQExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQExplosive::BulletHit_Implementation(FHitResult hitResult)
{
	if (ExlpodeParticle)
	{
		// Play FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExlpodeParticle, hitResult.Location, FRotator::ZeroRotator, true);
	}

	if (ExplodeSound)
	{
		// Play Sound
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}

	// #TODO apply damage


	Destroy();
}


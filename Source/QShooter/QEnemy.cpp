// Fill out your copyright notice in the Description page of Project Settings.


#include "QEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AQEnemy::AQEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AQEnemy::BulletHit_Implementation(FHitResult hitResult)
{
	if (ImpactFX)
	{
		// Play FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, hitResult.Location, FRotator::ZeroRotator, true);
	}

	if (ImpactSound)
	{
		// Play Sound
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

// Called when the game starts or when spawned
void AQEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called every frame
void AQEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AQEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


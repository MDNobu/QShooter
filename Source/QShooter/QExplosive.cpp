// Fill out your copyright notice in the Description page of Project Settings.


#include "QExplosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AQExplosive::AQExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(GetRootComponent());
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

void AQExplosive::BulletHit_Implementation(FHitResult hitResult, AActor* Shooter, AController* ShooterController)
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
	TArray<AActor*> overlappingCharacters;
	GetOverlappingActors(overlappingCharacters, ACharacter::StaticClass());

	for (AActor* overlapCharacter : overlappingCharacters)
	{
		//UE_LOG(LogTemp, Warning, TEXT("explosive overlapping character :  %s"), *overlapCharacter->GetName());
		UGameplayStatics::ApplyDamage(overlapCharacter, BaseDamage, ShooterController, Shooter, UDamageType::StaticClass());
	}

	Destroy();
}


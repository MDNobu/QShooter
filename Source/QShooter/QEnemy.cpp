// Fill out your copyright notice in the Description page of Project Settings.


#include "QEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AQEnemy::AQEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


float AQEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float damageTaked = 0.0f;
	if (Health - DamageAmount <= 0)
	{
		damageTaked = Health;
		Health = 0;
		Die();
	}
	else
	{
		damageTaked = DamageAmount;
		Health -= DamageAmount;
	}

	return damageTaked;
}

void AQEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarShowTimerHandle);
	GetWorldTimerManager().SetTimer(HealthBarShowTimerHandle, this, &AQEnemy::HideHealthBar, HealthBarShowDuration, false);
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

	ShowHealthBar();

	PlayHitMontage(TEXT("HitReactFront"));
}

// Called when the game starts or when spawned
void AQEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	Health = MaxHealth;
}

// Called every frame
void AQEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UpdateHitNumberWidgets();
}

// Called to bind functionality to input
void AQEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AQEnemy::StoreHitNumberLocation(UUserWidget* widget, FVector location)
{
	if (nullptr == widget)
	{
		return;
	}
	HitNumberWidgetLocations.Add(widget, location);

	FTimerHandle hitNumberDestroyTimer;
	FTimerDelegate timerDelegate;

	timerDelegate.BindUFunction(this, FName(TEXT("DestroyHitNumberWidget")), widget);
	//timerDelegate.bind
	GetWorldTimerManager().SetTimer(hitNumberDestroyTimer, timerDelegate, HitNumberWidgetShowTime, false);
}

void AQEnemy::Die()
{
	HideHealthBar();
}

void AQEnemy::PlayHitMontage(FName hitSectionName, float playRate /*= 1.0f*/)
{

	if (HitAnimMontage && bCanPlayHitMontage)
	{
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst)
		{
			animInst->Montage_Play(HitAnimMontage, playRate);
			animInst->Montage_JumpToSection(hitSectionName);
		}
		bCanPlayHitMontage = false;
		const float delayTime = FMath::RandRange(HitDelayMin, HitDelaMax);
		GetWorldTimerManager().SetTimer(HitDelayTimerHandle, this, &AQEnemy::ResetHitDelay, delayTime, false);
	}
}

void AQEnemy::ResetHitDelay()
{
	bCanPlayHitMontage = true;
}

void AQEnemy::UpdateHitNumberWidgets()
{
	for (auto& hitPair : HitNumberWidgetLocations)
	{
		UUserWidget* hitNumber = hitPair.Key;
		const FVector hitLocation = hitPair.Value;
		FVector2D hitScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(), 
			hitLocation, 
			hitScreenPosition);

		hitNumber->SetPositionInViewport(hitScreenPosition);
	}
}

void AQEnemy::DestroyHitNumberWidget(UUserWidget* widget)
{
	if (widget)
	{
		HitNumberWidgetLocations.Remove(widget);
		widget->RemoveFromParent();

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Detroy Hit Number Widget should not pass in a nullptr"));
	}

}


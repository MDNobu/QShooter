// Fill out your copyright notice in the Description page of Project Settings.


#include "QEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "QEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "QShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AQEnemy::AQEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgggroSphereComponent"));
	AggroSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollisionBox"));
	LeftWeaponCollisionBox->SetupAttachment(GetMesh(), TEXT("LeftWeaponSocket"));

	RightWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollisionBox"));
	RightWeaponCollisionBox->SetupAttachment(GetMesh(), TEXT("RightWeaponSocket"));


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


	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(DamageCauser))
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), playerCharacter);
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
	

	bool isStunned = FMath::RandRange(0.0f, 1.0f) < StunChance;
	if (isStunned)
	{
		PlayHitMontage(TEXT("HitReactFront"));
		//bIsStunning = isStunned;
		SetIsStunning(true);
	}

}

// Called when the game starts or when spawned
void AQEnemy::BeginPlay()
{
	Super::BeginPlay();

#pragma region SetupBoxCollision
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftWeaponCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LeftWeaponCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);

	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightWeaponCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RightWeaponCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
#pragma endregion InitBoxCollision

	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AQEnemy::OnAggroSphereBeginOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AQEnemy::OnCombatRangeSphereBeginOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AQEnemy::OnCombatRangeSphereEndOverlap);

	LeftWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AQEnemy::OnLeftWeaponBoxBeginOverlap);
	RightWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AQEnemy::OnRightWeaponBoxBeginOverlap);


	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);


	Health = MaxHealth;

#pragma region SetupBehaviorTree

	const FVector patroPointWS = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPointLS);
	const FVector patrollPoint2WS = GetActorTransform().TransformPosition(PatrolPoint2LS);

	DrawDebugSphere(GetWorld(), patroPointWS, 20.0f, 10, FColor::Red, true);
	EnemyController = Cast<AQEnemyController>(GetController());
	if (EnemyController)
	{
		//Init BB values
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), patroPointWS);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), patrollPoint2WS);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDied"), false);
		SetCanAttackInBlackboard(true);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QEnemy %s should have a AQEnemeyController "), *GetName());
	}
#pragma endregion



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

void AQEnemy::DoDamaget2Character(AQShooterCharacter* playerCharacter)
{
	if (nullptr == playerCharacter)
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("LeftWeapon Begin Overlap"));

	UGameplayStatics::ApplyDamage(playerCharacter, BaseDamage, GetController(), this, UDamageType::StaticClass());

	if (playerCharacter->GetImpactSoundCue())
	{
		//UGameplayStatics::PlaySound2D(this, playerCharacter->GetImpactSoundCue(), 0.30f);
		UGameplayStatics::PlaySoundAtLocation(this, playerCharacter->GetImpactSoundCue(), GetActorLocation(), 0.3f);
	}

	playerCharacter->TryStun();
}

void AQEnemy::PlayBloodFX(AQShooterCharacter* playerCharacter, FName socketName)
{
	if (playerCharacter && playerCharacter->GetBloodParticle())
	{
		//这里用weapon socket的位置来spawn socket
		const USkeletalMeshSocket* socket4Blood = GetMesh()->GetSocketByName(socketName);
		if (socket4Blood)
		{
			FTransform weaponBloodSocket_Transform = socket4Blood->GetSocketTransform(GetMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), playerCharacter->GetBloodParticle(), weaponBloodSocket_Transform, true);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("%s do not have a socket %s, please check"), *GetName(), *socketName.ToString());
		}
	}
}

void AQEnemy::OnLeftWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(OtherActor))
	{
		DoDamaget2Character(playerCharacter);
		PlayBloodFX(playerCharacter, TEXT("FX_Trail_L_01"));
	}
}



void AQEnemy::OnRightWeaponBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(OtherActor))
	{
		DoDamaget2Character(playerCharacter);
		PlayBloodFX(playerCharacter, TEXT("FX_Trail_R_01"));
	}
}





void AQEnemy::Die()
{
	if (bIsDying)
	{
		return;
	}
	bIsDying = true;

	HideHealthBar();

	// Play death montage
	if (DeathAnimMontage)
	{
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst)
		{
			animInst->Montage_Play(DeathAnimMontage, 1.0f);
		}
	}

	// set is death in BB
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDied"), true);
		
		EnemyController->StopMovement();
	}

	// disable collision
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


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

void AQEnemy::PlayAttackMontage(FName attackSectionName, float playRate /*= 1.0f*/)
{
	
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && AttackAnimMontage)
	{
		animInst->Montage_Play(AttackAnimMontage, playRate);
		animInst->Montage_JumpToSection(attackSectionName);

		//bCanAttack = false;
		SetCanAttackInBlackboard(false);
		UE_LOG(LogTemp, Warning, TEXT("Set can attack value %d"), bCanAttack);
		GetWorldTimerManager().SetTimer(AttackCDTimer, this, &AQEnemy::EnableAttack, AttackCD, false);
	}
}

FName AQEnemy::SelectAttackSectionName() const
{
	FName sectionName;
	const int randomNum = FMath::RandRange(1, 4);
	
	switch (randomNum)
	{
	case 1:
		sectionName = AttackL_SectionName;
		break;
	case 2:
		sectionName = AttackR_SectionName;
		break;
	case 3:
		sectionName = AttackLFast_SectionName;
		break;
	case 4:
		sectionName = AttackRFast_SectionName;
		break;
	}
	return sectionName;
}

void AQEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AQEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AQEnemy::ActivateRightWeapon()
{
	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AQEnemy::DeactivateRightWeapon()
{
	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AQEnemy::FinishDeath()
{
	//bIsDying = false;
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(DeathDestoryTimer, this, &AQEnemy::DestoryEnemy, DestroyTimeAfterDie, false);
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

void AQEnemy::OnAggroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(OtherActor))
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), playerCharacter);

		//playerCharacter->OnPlayerCharacterDie.Add()
		playerCharacter->OnPlayerCharacterDie.AddUObject(this, &AQEnemy::OnPlayerCharacterDie);
	}
}

void AQEnemy::OnCombatRangeSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(OtherActor))
	{
		SetIsInCombatRange(true);
	}
}

void AQEnemy::OnCombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AQShooterCharacter* playerCharacter = Cast<AQShooterCharacter>(OtherActor))
	{
		SetIsInCombatRange(false);
	}
}

void AQEnemy::DestoryEnemy()
{
	Destroy();
}

void AQEnemy::EnableAttack()
{
	//bCanAttack = true;
	SetCanAttackInBlackboard(true);
}

void AQEnemy::SetCanAttackInBlackboard(bool canAttack)
{
	bCanAttack = canAttack;
	//UE_LOG(LogTemp, Warning, TEXT("Set can attack value %b"), canAttack);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
}

void AQEnemy::OnPlayerCharacterDie()
{
	EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
	//playerCharacter->OnPlayerCharacterDie.Add()
	//playerCharacter->OnPlayerCharacterDie.AddUObject(this, &AQEnemy::OnPlayerCharacterDie);
	UE_LOG(LogTemp, Warning, TEXT("%s OnPlayerCharacter Die"), *GetName());
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

void AQEnemy::SetIsStunning(bool isStunning)
{
	bIsStunning = isStunning;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsStunning"), bIsStunning);
	}
}

void AQEnemy::SetIsInCombatRange(bool isInCombatRange)
{
	bIsInCombatRange = isInCombatRange;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsInCombatRange"), bIsInCombatRange);
	}
}


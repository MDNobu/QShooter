// Fill out your copyright notice in the Description page of Project Settings.


#include "QEnemyController.h"
#include "QEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AQEnemyController::AQEnemyController()
{
	BlackboardCom = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	BehaviorTreeCom = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	check(BlackboardCom);
	check(BehaviorTreeCom);
}

void AQEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AQEnemy* enemyCharacter = Cast<AQEnemy>(InPawn))
	{
		if (UBehaviorTree* enemyBT = enemyCharacter->GetBehaviorTree())
		{
			BlackboardCom->InitializeBlackboard(*(enemyBT->BlackboardAsset));
		}
		//BlackboardCom->InitializeBlackboard()
	}
	//Init blackboard component
}

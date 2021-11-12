// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "QEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class QSHOOTER_API AQEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AQEnemyController();


protected:
	void OnPossess(APawn* InPawn) override;


private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UBlackboardComponent* BlackboardCom;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QShooter", meta = (AllowPrivateAccess = true))
	class UBehaviorTreeComponent* BehaviorTreeCom;


};

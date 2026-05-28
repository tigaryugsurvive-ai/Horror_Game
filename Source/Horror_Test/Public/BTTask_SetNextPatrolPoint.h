#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetNextPatrolPoint.generated.h"

UCLASS()
class HORROR_TEST_API UBTTask_SetNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetNextPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolPointKey;

	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool bLoop = true;
};

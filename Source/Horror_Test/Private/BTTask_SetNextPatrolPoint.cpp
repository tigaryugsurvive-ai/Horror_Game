#include "BTTask_SetNextPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"

/*
    ÉâÉìÉ_ÉÄÇ»èÑâÒ
*/


UBTTask_SetNextPatrolPoint::UBTTask_SetNextPatrolPoint()
{
    NodeName = "Set Next Patrol Point (Route)";
}

EBTNodeResult::Type UBTTask_SetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AI->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    if (Enemy->PatrolPoints.Num() == 0) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    AActor* NextPoint = Enemy->GetNextPatrolPointRandom();
    if (!IsValid(NextPoint))
    {
        return EBTNodeResult::Failed;
    }

    BB->SetValueAsObject(PatrolPointKey.SelectedKeyName, NextPoint);
    return EBTNodeResult::Succeeded;
}

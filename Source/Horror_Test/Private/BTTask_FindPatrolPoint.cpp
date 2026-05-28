#include "BTTask_FindPatrolPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnemyCharacter.h"

// èáî‘í ÇËÇÃèÑâÒ

UBTTask_FindPatrolPoint::UBTTask_FindPatrolPoint()
{
    NodeName = "Set Next Patrol Point (Route -> Vector)";
}

EBTNodeResult::Type UBTTask_FindPatrolPoint::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AI->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    if (Enemy->PatrolPoints.Num() == 0) return EBTNodeResult::Failed;

    // îzóÒÇèáî‘Ç…âÒÇ∑
    int32 Safe = 0;
    while (Safe < Enemy->PatrolPoints.Num())
    {
        if (Enemy->PatrolIndex >= Enemy->PatrolPoints.Num())
        {
            if (!bLoop) return EBTNodeResult::Failed;
            Enemy->PatrolIndex = 0;
        }

        AActor* Point = Enemy->PatrolPoints[Enemy->PatrolIndex];
        Enemy->PatrolIndex++;

        if (IsValid(Point))
        {
            if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
            {
                BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Point->GetActorLocation());
                return EBTNodeResult::Succeeded;
            }
            return EBTNodeResult::Failed;
        }

        Safe++;
    }

    return EBTNodeResult::Failed;
}

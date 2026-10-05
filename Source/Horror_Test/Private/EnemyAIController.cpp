#include "EnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "TimerManager.h"
#include "DrawDebugHelpers.h"

#include "BreakDebugDraw.h"

static const FName KEY_TargetActor(TEXT("TargetActor"));

AEnemyAIController::AEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    ApplySightConfigFromProperties();

    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(SightConfig->GetSenseImplementation());
    Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
}

void AEnemyAIController::ApplySightConfigFromProperties()
{
    if (!SightConfig) return;

    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    SightConfig->SetMaxAge(SightMaxAge);

    SightConfig->DetectionByAffiliation.bDetectEnemies = bDetectEnemies;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = bDetectFriendlies;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = bDetectNeutrals;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ApplySightConfigFromProperties();

    if (BlackboardAsset)
    {
        UseBlackboard(BlackboardAsset, MyBlackboard);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AI] BlackboardAsset is NULL (set in BP_EnemyAIController)"));
    }

    // BT開始
    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AI] BehaviorTreeAsset is NULL (set in BP_EnemyAIController)"));
    }
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // Blackboardがない場合は処理を行わない
    if (!MyBlackboard) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        // 発見・再発見した場合は、追跡対象の解除を取り消す
        GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);

        // 検知したActorをBlackboardに追跡対象として登録する
        MyBlackboard->SetValueAsObject(KEY_TargetActor, Actor);
    }
    else
    {
        // 既存のタイマーを取り消し、見失った時点から待ち時間を計り直す
        GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);

        // 一瞬見失っても追跡対象をすぐに解除しないよう、猶予時間を設ける
        GetWorldTimerManager().SetTimer(LoseSightTimerHandle, [this]()
            {
                if (MyBlackboard)
                {
                    // 猶予時間内に再発見できなかった場合、追跡対象を解除する
                    MyBlackboard->ClearValue(KEY_TargetActor);
                }
            },
            LoseSightGraceSeconds, // 追跡対象を解除するまでの待ち時間
            false                  // タイマーは繰り返さず、一度だけ実行する
        );
    }
}
/*
    デバッグ処理
*/
void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bDrawAIDebug) return;
    if (!BreakDebugDraw::IsEnabled()) return;

    APawn* P = GetPawn();
    if (!P) return;

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector Center = P->GetActorLocation();
    const FVector Forward = P->GetActorForwardVector();

    BreakDebugDraw::Circle(World, Center, SightRadius, 0.0f, 1.5f);
    BreakDebugDraw::Circle(World, Center, LoseSightRadius, 0.0f, 3.0f);

    const float ConeLen = SightRadius;
    const float ConeHalfAngleRad = FMath::DegreesToRadians(PeripheralVisionAngleDegrees);

    DrawDebugCone(World, Center, Forward, ConeLen, ConeHalfAngleRad, ConeHalfAngleRad, 24,
        FColor::Cyan, false, 0.0f, 0, 1.5f);

    if (MyBlackboard)
    {
        if (AActor* Target = Cast<AActor>(MyBlackboard->GetValueAsObject(KEY_TargetActor)))
        {
            BreakDebugDraw::Line(World, Center, Target->GetActorLocation(), 0.0f, 2.0f);
        }
    }

    BreakDebugDraw::DrawText(World, Center + FVector(0, 0, 80),
        FString::Printf(TEXT("Sight=%.0f Lose=%.0f FOV=%.0f"), SightRadius, LoseSightRadius, PeripheralVisionAngleDegrees),
        0.0f);

}


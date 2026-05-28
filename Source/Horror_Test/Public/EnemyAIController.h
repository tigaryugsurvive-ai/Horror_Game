#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class HORROR_TEST_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardData* BlackboardAsset = nullptr;

    //å©Ç¶ÇÈãóó£
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight", meta = (ClampMin = "0.0"))
    float SightRadius = 2000.f;

    //å©é∏Ç§ãóó£
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight", meta = (ClampMin = "0.0"))
    float LoseSightRadius = 1200.f;

    //éãñÏäp
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float PeripheralVisionAngleDegrees = 60.f;

    //å©é∏Ç¡ÇƒÇ©ÇÁÇ«ÇÍÇ≠ÇÁÇ¢íTçıÇ∑ÇÈÇ©
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight", meta = (ClampMin = "0.0"))
    float SightMaxAge = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight", meta = (ClampMin = "0.0"))
    float LoseSightGraceSeconds = 2.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
    bool bDetectEnemies = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
    bool bDetectFriendlies = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
    bool bDetectNeutrals = true;

    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Debug")
    bool bDrawAIDebug = true;  

private:
    UPROPERTY()
    UAIPerceptionComponent* Perception = nullptr;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig = nullptr;

    UPROPERTY()
    UBlackboardComponent* MyBlackboard = nullptr;

    FTimerHandle LoseSightTimerHandle;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    void ApplySightConfigFromProperties();
};

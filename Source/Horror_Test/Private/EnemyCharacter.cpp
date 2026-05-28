#include "EnemyCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAIController.h"
#include "BreakableUtils.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "NavigationSystem.h"


AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    DeathGC = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("DeathGC"));
    DeathGC->SetupAttachment(GetRootComponent()); // RootはCapsuleのまま
    DeathGC->SetSimulatePhysics(false);
    DeathGC->SetNotifyBreaks(true);
    DeathGC->SetVisibility(false, true);
    DeathGC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 巡回の初期化
void AEnemyCharacter::ResetPatrolRandomOrder()
{
    PatrolOrder.Reset();
    PatrolIndex = 0;

    for (int32 i = 0; i < PatrolPoints.Num(); ++i)
    {
        if (IsValid(PatrolPoints[i]))
        {
            PatrolOrder.Add(i);
        }
    }

    const int32 N = PatrolOrder.Num();
    if (N <= 1)
    {
        return;
    }

    for (int32 i = N - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        PatrolOrder.Swap(i, j);
    }
}

// 次の巡回ポイントをランダムに取得
AActor* AEnemyCharacter::GetNextPatrolPointRandom()
{
    if (PatrolOrder.Num() == 0)
    {
        ResetPatrolRandomOrder();
    }

    /*
    if (PatrolOrder.Num() == 0)
    {
        return nullptr;
    }
    */

    if (PatrolIndex >= PatrolOrder.Num())
    {
        ResetPatrolRandomOrder();
    }

    for (int32 Guard = 0; Guard < PatrolOrder.Num(); ++Guard)
    {
        const int32 PointIndex = PatrolOrder[PatrolIndex];
        ++PatrolIndex;

        if (PatrolPoints.IsValidIndex(PointIndex) && IsValid(PatrolPoints[PointIndex]))
        {
            return PatrolPoints[PointIndex];
        }
    }

    return nullptr;
}

void AEnemyCharacter::BreakEnemy()
{
    static const FName DeadTag(TEXT("BrokenEnemy"));

    if (Tags.Contains(DeadTag)) return;
    Tags.Add(DeadTag);

    UE_LOG(LogTemp, Warning, TEXT("[Enemy] BreakEnemy called: %s"), *GetName());

    // 見た目を消す
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetVisibility(false, false);
        MeshComp->SetHiddenInGame(true, false);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComp->SetCastShadow(false);
        UE_LOG(LogTemp, Warning, TEXT("[Enemy] Mesh hidden"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Enemy] MeshComp is null"));
    }

    // 当たり判定を消す
    if (UCapsuleComponent* Cap = GetCapsuleComponent())
    {
        Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        UE_LOG(LogTemp, Warning, TEXT("[Enemy] Capsule collision off"));
    }

    {
        //コンポーネントだけ残す
        TArray<UPrimitiveComponent*> PrimComps;
        GetComponents<UPrimitiveComponent>(PrimComps);

        for (UPrimitiveComponent* P : PrimComps)
        {
            if (!P) continue;

            if (P == DeathGC) continue;

            P->SetVisibility(false, true);
            P->SetHiddenInGame(true, true);
            P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            P->SetCastShadow(false);
        }

        // 移動停止
        if (UCharacterMovementComponent* Move = GetCharacterMovement())
        {
            Move->StopMovementImmediately();
            Move->DisableMovement();
        }

        // AI停止
        if (AAIController* AIC = Cast<AAIController>(GetController()))
        {
            AIC->StopMovement();
            if (UBrainComponent* Brain = AIC->BrainComponent)
            {
                Brain->StopLogic(TEXT("Dead"));
            }
        }

        SetActorTickEnabled(false);
    }


    if (!DeathGC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Enemy] DeathGC is null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Enemy] DeathGC valid. HiddenInGame(before)=%d Visible(before)=%d"),
        DeathGC->bHiddenInGame ? 1 : 0,
        DeathGC->IsVisible() ? 1 : 0
    );

    // Mesh位置に合わせる
    FTransform T = GetActorTransform();
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        T = MeshComp->GetComponentTransform();
    }
    T.AddToTranslation(FVector(0, 0, 30.f));
    DeathGC->SetWorldTransform(T);

    // スケール
    DeathGC->SetWorldScale3D(GetActorScale3D());

    // 表示
    DeathGC->SetHiddenInGame(false);
    DeathGC->SetVisibility(true, true);

    DeathGC->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
  
    // 衝突
    DeathGC->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    DeathGC->SetCollisionObjectType(ECC_PhysicsBody);

    DeathGC->SetCollisionResponseToAllChannels(ECR_Block);

    DeathGC->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    DeathGC->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    DeathGC->SetSimulatePhysics(true);
    DeathGC->RecreatePhysicsState();
    DeathGC->WakeAllRigidBodies();

    UE_LOG(LogTemp, Warning, TEXT("[Enemy] DeathGC shown. HiddenInGame(after)=%d Visible(after)=%d Sim=%d"),
        DeathGC->bHiddenInGame ? 1 : 0,
        DeathGC->IsVisible() ? 1 : 0,
        DeathGC->IsSimulatingPhysics() ? 1 : 0
    );
}

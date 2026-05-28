#include "PlayerCharacter.h"
#include "HammerActor.h"
#include "CubeActor.h"
#include "BreakDebugDraw.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemObjects.h"  
#include "Field/FieldSystemTypes.h"    

APlayerCharacter::APlayerCharacter()
{
    // 初期状態：ハンマー未所持
    bHasHammer = false;

    PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 入力
    PlayerInputComponent->BindAction("PickupHammer", IE_Pressed, this, &APlayerCharacter::PickupHammer);
    PlayerInputComponent->BindAction("BreakCube", IE_Pressed, this, &APlayerCharacter::TryBreakCube);
    PlayerInputComponent->BindAction("ToggleDebug", IE_Pressed, this, &APlayerCharacter::ToggleBreakDebug);
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // レベルにあるハンマーを1個拾って参照する
    if (!HammerRef)
    {
        HammerRef = Cast<AHammerActor>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AHammerActor::StaticClass())
        );
    }
    // レベルにある敵を参照
    if (!EnemyRef)
    {
        EnemyRef = Cast<AEnemyCharacter>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass())
        );
    }
}


void APlayerCharacter::PickupHammer()
{
    if (bHasHammer || !HammerRef) return;

    // プレイヤーとハンマーの距離が近い時だけ拾える
    const float DistSq = FVector::DistSquared(HammerRef->GetActorLocation(), GetActorLocation());
    if (DistSq > FMath::Square(InteractDistance)) return;

    // ここに来たら拾える条件を満たしている
    bHasHammer = true;

    // ハンマーを消す
    HammerRef->Destroy();
    HammerRef = nullptr;

    EnableAllCubesBreakable();
}

ACubeActor* APlayerCharacter::FindNearestCube(float MaxDistance) const
{
    // 全Cubeを取得
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACubeActor::StaticClass(), Found);

    ACubeActor* Best = nullptr;
    float BestDistSq = FMath::Square(MaxDistance);

    const FVector MyLoc = GetActorLocation();

    // 一番近いCubeだけを返す
    for (AActor* A : Found)
    {
        ACubeActor* Cube = Cast<ACubeActor>(A);
        if (!Cube) continue;

        const float DistSq = FVector::DistSquared(Cube->GetActorLocation(), MyLoc);
        if (DistSq <= BestDistSq)
        {
            BestDistSq = DistSq;
            Best = Cube;
        }
    }

    return Best;
}

// ハンマー取得後に全Cubeを破壊可能化
void APlayerCharacter::EnableAllCubesBreakable() const
{
    TArray<AActor*> FoundCubes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACubeActor::StaticClass(), FoundCubes);

    for (AActor* A : FoundCubes)
    {
        if (ACubeActor* Cube = Cast<ACubeActor>(A))
        {
            Cube->OnCubeBroken.AddDynamic(const_cast<APlayerCharacter*>(this), &APlayerCharacter::OnCubeActuallyBroken);

            Cube->EnableBreakable();
        }
    }
}

void APlayerCharacter::ApplyStrainField(UGeometryCollectionComponent* GC, const FVector& Origin) const
{
    // 破壊用の「放射状フィールド」を作る（中心Origin、半径StrainRadius）
    URadialFalloff* Field = NewObject<URadialFalloff>(GetTransientPackage());
    Field->Magnitude = StrainMagnitude;                    // 強さ
    Field->Radius = StrainRadius;                          // 範囲
    Field->Position = Origin;                             // 中心
    Field->Falloff = EFieldFalloffType::Field_FallOff_None; // 減衰なし（一定）

    UFieldSystemMetaDataIteration* Meta = NewObject<UFieldSystemMetaDataIteration>(GetTransientPackage());
    Meta->Iterations = StrainIterations;

    // GeometryCollection に対して「クラスタStrain」を外部から与える
    GC->ApplyPhysicsField(
        true,
        EGeometryCollectionPhysicsTypeEnum::Chaos_ExternalClusterStrain,
        Meta,
        Field
    );
}

void APlayerCharacter::TryBreakCube()
{
    if (!bHasHammer) return;

    //まずCubeを探して壊す
    if (ACubeActor* Cube = FindNearestCube(InteractDistance))
    {
        UGeometryCollectionComponent* GC = Cast<UGeometryCollectionComponent>(Cube->GetRootComponent());
        if (!GC) return;

        ApplyStrainField(GC, Cube->GetActorLocation());
        return;
    }

    //Cubeが全て無いなら、敵を壊す
    if (!bCanBreakEnemy) return;

    if (AEnemyCharacter* Enemy = FindNearestEnemy(InteractDistance))
    {
        Enemy->BreakEnemy();
        if (UGeometryCollectionComponent* GC = Enemy->GetDeathGC())
        {
            ApplyStrainField(GC, Enemy->GetActorLocation());
        }
    }

}

void APlayerCharacter::OnCubeActuallyBroken(ACubeActor* Cube)
{
    BrokenCubeCount++;

    if (!bCanBreakEnemy && BrokenCubeCount >= RequiredBrokenCubes)
    {
        bCanBreakEnemy = true;

        UE_LOG(LogTemp, Warning, TEXT("Enemy break unlocked! (bCanBreakEnemy = true)"));
    }
}


AEnemyCharacter* APlayerCharacter::FindNearestEnemy(float MaxDistance) const
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Found);

    AEnemyCharacter* Best = nullptr;
    float BestDistSq = FMath::Square(MaxDistance);
    const FVector MyLoc = GetActorLocation();

    for (AActor* A : Found)
    {
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(A);
        if (!Enemy) continue;

        const float DistSq = FVector::DistSquared(Enemy->GetActorLocation(), MyLoc);
        if (DistSq <= BestDistSq)
        {
            BestDistSq = DistSq;
            Best = Enemy;
        }
    }
    return Best;
}


/*
   デバック処理(Bキーで切り替え)
*/

void APlayerCharacter::ToggleBreakDebug()
{
    BreakDebugDraw::Toggle();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!BreakDebugDraw::IsEnabled()) return;

    const FVector Center = GetActorLocation();

    BreakDebugDraw::DrawBreakRadius(GetWorld(), Center, StrainRadius, 0.0f, 2.0f);

    const FString Params = FString::Printf(
        TEXT("Magnitude: %.0f  Iter: %d  Interact: %.1f"),
        StrainMagnitude, StrainIterations, InteractDistance
    );
    BreakDebugDraw::DrawText(GetWorld(), Center + FVector(0, 0, 60), Params, 0.0f);
}


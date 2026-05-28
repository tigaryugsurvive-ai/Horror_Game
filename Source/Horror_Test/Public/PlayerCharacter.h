#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AHammerActor;
class ACubeActor;
class AEnemyCharacter; 
class UGeometryCollectionComponent;

/*
  プレイヤーキャラクター
  ハンマーを拾う（Eキー）
  ハンマー所持中のみ、近くのCubeを破壊（左クリック）
*/

UCLASS()
class HORROR_TEST_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // 現在ハンマーを所持しているか
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bHasHammer = false;

    // ハンマーを参照する
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Hammer")
    AHammerActor* HammerRef = nullptr;

    // 拾える/壊せる距離
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning", meta = (ClampMin = "0.0"))
    float InteractDistance = 200.f;

    // 破壊の強さ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning|Break", meta = (ClampMin = "0.0"))
    float StrainMagnitude = 50000.f;

    // 破壊できるの半径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning|Break", meta = (ClampMin = "0.0"))
    float StrainRadius = 300.f;

    // 反復回数：基本は1
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning|Break", meta = (ClampMin = "1"))
    int32 StrainIterations = 1;
    // 何個壊したら敵を壊せるか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
    int32 RequiredBrokenCubes = 3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
    int32 BrokenCubeCount = 0;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Progress")
    bool bCanBreakEnemy = false;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Progress")
    AEnemyCharacter* EnemyRef = nullptr;

    UFUNCTION()
    void OnCubeActuallyBroken(ACubeActor* Cube);


    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    // Bキーで切替
    void ToggleBreakDebug();

    void EnableAllCubesBreakable() const;

    // ハンマーを拾う
    void PickupHammer();

    // Cubeを壊す
    void TryBreakCube();

private:
    // 近くにあるCubeを1つだけ探す
    ACubeActor* FindNearestCube(float MaxDistance) const;

    void ApplyStrainField(UGeometryCollectionComponent* GC, const FVector& Origin) const;

    AEnemyCharacter* FindNearestEnemy(float MaxDistance) const;
};
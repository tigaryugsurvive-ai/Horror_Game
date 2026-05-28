#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class HORROR_TEST_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	//èÑâÒÉãÅ[Ég
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Patrol")
	TArray<AActor*> PatrolPoints;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	int32 PatrolIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<int32> PatrolOrder;

	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	void ResetPatrolRandomOrder();

	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	AActor* GetNextPatrolPointRandom();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Break")
	UGeometryCollectionComponent* DeathGC = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Break")
	void BreakEnemy();

	UFUNCTION(BlueprintCallable, Category = "Break")
	UGeometryCollectionComponent* GetDeathGC() const { return DeathGC; }
private:
	bool bIsBroken = false;


};
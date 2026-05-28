#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

class UGeometryCollectionComponent;
struct FChaosBreakEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCubeBrokenSignature, ACubeActor*, Cube);

UCLASS()
class HORROR_TEST_API ACubeActor : public AActor
{
    GENERATED_BODY()

public:
    ACubeActor();

    UFUNCTION(BlueprintCallable, Category = "Breakable")
    void EnableBreakable();

    UPROPERTY(BlueprintAssignable, Category = "Breakable")
    FCubeBrokenSignature OnCubeBroken;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UGeometryCollectionComponent* GeometryCollection;

    UFUNCTION()
    void HandleChaosBreak(const FChaosBreakEvent& BreakEvent);

private:
    bool bReportedBroken = false;
};

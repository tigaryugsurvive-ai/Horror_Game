#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BreakableUtils.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class HORROR_TEST_API UBreakableUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Breakable")
    static bool EnableBreakableOnActor(AActor* TargetActor, bool bSimulatePhysics = true, bool bNotifyBreaks = true);

    UFUNCTION(BlueprintCallable, Category = "Breakable")
    static bool EnableBreakableOnGC(UGeometryCollectionComponent* GC, bool bSimulatePhysics = true, bool bNotifyBreaks = true);
};

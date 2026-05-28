#include "BreakableUtils.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

bool UBreakableUtils::EnableBreakableOnActor(AActor* TargetActor, bool bSimulatePhysics, bool bNotifyBreaks)
{
    if (!TargetActor) return false;

    UGeometryCollectionComponent* GC = Cast<UGeometryCollectionComponent>(TargetActor->GetRootComponent());
    return EnableBreakableOnGC(GC, bSimulatePhysics, bNotifyBreaks);
}

bool UBreakableUtils::EnableBreakableOnGC(UGeometryCollectionComponent* GC, bool bSimulatePhysics, bool bNotifyBreaks)
{
    if (!GC) return false;

    GC->SetSimulatePhysics(bSimulatePhysics);
    GC->SetNotifyBreaks(bNotifyBreaks);
    return true;
}

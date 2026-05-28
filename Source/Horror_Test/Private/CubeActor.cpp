#include "CubeActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "BreakableUtils.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"
#include "Chaos/ChaosEngineInterface.h"

ACubeActor::ACubeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
    RootComponent = GeometryCollection;

    GeometryCollection->SetNotifyBreaks(true);
    GeometryCollection->SetSimulatePhysics(false);
}

void ACubeActor::BeginPlay()
{
    Super::BeginPlay();

    if (GeometryCollection)
    {
        GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ACubeActor::HandleChaosBreak);
    }
}

void ACubeActor::HandleChaosBreak(const FChaosBreakEvent& BreakEvent)
{
    if (bReportedBroken) return;
    bReportedBroken = true;

    OnCubeBroken.Broadcast(this);
}

void ACubeActor::EnableBreakable()
{
    UBreakableUtils::EnableBreakableOnGC(GeometryCollection, true, true);
}

#include "HammerActor.h"
#include "Components/StaticMeshComponent.h"

AHammerActor::AHammerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HammerMesh"));
    RootComponent = Mesh;
}
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LookEnamy.generated.h"

UCLASS()
class HORROR_TEST_API ALookEnamy : public ACharacter
{
    GENERATED_BODY()

public:
    ALookEnamy();

protected:
    virtual void Tick(float DeltaTime) override;
};
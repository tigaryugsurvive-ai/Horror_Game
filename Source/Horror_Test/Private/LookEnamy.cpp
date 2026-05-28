#include "LookEnamy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ALookEnamy::ALookEnamy()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ALookEnamy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //プレイヤーの取得
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); 
    if (!PlayerCharacter) return;

    //自分とプレイヤーの位置
    FVector MyLocation = GetActorLocation();
    FVector TargetLocation = PlayerCharacter->GetActorLocation();

    //プレイヤー方向の回転を計算
    FRotator LookAtRotation = (TargetLocation - MyLocation).Rotation();
    FRotator CurrentRotation = GetActorRotation();

    //左右の更新
    FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);
    SetActorRotation(NewRotation);
}
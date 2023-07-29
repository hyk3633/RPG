

#include "DamageType/DamageTypeStunAndPush.h"
#include "GameFramework/Character.h"

void UDamageTypeStunAndPush::GetPushed(AActor* Pusher, ACharacter* Pushed)
{
	if (IsValid(Pusher) && IsValid(Pushed))
	{
		const FVector LaunchDirection = (Pushed->GetActorLocation() - Pusher->GetActorLocation()).GetSafeNormal();
		Pushed->LaunchCharacter(LaunchDirection * 700, false, true);
	}
}

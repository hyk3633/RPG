
#include "GameSystem/ProjectilePoolerComponent.h"
#include "Projectile/RPGBaseProjectile.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"

UProjectilePoolerComponent::UProjectilePoolerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UProjectilePoolerComponent::CreatePool(TSubclassOf<ARPGBaseProjectile> ProjClass, const int32 Size, const EProjectileType Type)
{
	ProjectileClass = ProjClass;

	PoolSize = ActivatedNum = DeactivatedNum = Size;
	ProjectileArr.Init(nullptr, Size);

	const FProjectileInfo& NewProjectileInfo = *GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetProjectileInfo(Type);

	const FProjectileAssets& NewProjectileAssets = *GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetProjectileAssets(Type);

	if (ProjectileClass)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
		for (int16 Idx = 0; Idx < PoolSize; Idx++)
		{
			ARPGBaseProjectile* Projectile = World->SpawnActorDeferred<ARPGBaseProjectile>(ProjectileClass, FTransform(FRotator().ZeroRotator, FVector().ZeroVector));
			Projectile->SetProjectileInfo(NewProjectileInfo);
			Projectile->SetProjectileAssets(NewProjectileAssets);
			Projectile->DDeactivateProjectile.BindUObject(this, &UProjectilePoolerComponent::AddDeactivatedNum);
			Projectile->FinishSpawning(FTransform(FRotator().ZeroRotator, FVector().ZeroVector));
			ProjectileArr[Idx] = Projectile;
		}
	}
}

void UProjectilePoolerComponent::DestroyPool()
{
	for (ARPGBaseProjectile* Projectile : ProjectileArr)
	{
		Projectile->Destroy();
	}
	ProjectileArr.Empty();
}

ARPGBaseProjectile* UProjectilePoolerComponent::GetPooledProjectile(APawn* CallerPawn, const float Damage)
{
	if (DeactivatedNum == 0) return nullptr;
	if (ActivatedNum == 0) ActivatedNum = PoolSize;

	DeactivatedNum--;
	ActivatedNum--;

	ProjectileArr[ActivatedNum]->SetOwner(CallerPawn);
	ProjectileArr[ActivatedNum]->SetInstigator(CallerPawn);
	ProjectileArr[ActivatedNum]->SetProjectileDamage(Damage);
	return ProjectileArr[ActivatedNum];
}

void UProjectilePoolerComponent::AddDeactivatedNum()
{
	DeactivatedNum++;
}



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

	ProjectileType = Type;
	PoolSize = Size;
	ProjectileArr.Init(nullptr, Size);

	ProjectileInfo = *GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetProjectileInfo(Type);
	ProjectileAssets = *GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetProjectileAssets(Type);

	if (ProjectileClass)
	{
		for (uint32 Idx = 0; Idx < PoolSize; Idx++)
		{
			ProjectileArr[Idx] = CreateProjectile();
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
	for (uint32 Idx = 0; Idx < PoolSize; Idx++)
	{
		if (ProjectileArr[Idx]->GetActivated() == false)
		{
			ProjectileArr[Idx]->SetOwner(CallerPawn);
			ProjectileArr[Idx]->SetInstigator(CallerPawn);
			ProjectileArr[Idx]->SetProjectileDamage(Damage);

			return ProjectileArr[Idx];
		}
	}

	ARPGBaseProjectile* NewProjectile = CreateProjectile();
	ProjectileArr.Add(NewProjectile);
	PoolSize++;
	return NewProjectile;
}

ARPGBaseProjectile* UProjectilePoolerComponent::CreateProjectile()
{
	ARPGBaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARPGBaseProjectile>(ProjectileClass, FTransform(FRotator().ZeroRotator, FVector().ZeroVector));
	Projectile->SetProjectileInfo(ProjectileInfo);
	Projectile->SetProjectileAssets(ProjectileAssets);
	Projectile->FinishSpawning(FTransform(FRotator().ZeroRotator, FVector().ZeroVector));
	return Projectile;
}

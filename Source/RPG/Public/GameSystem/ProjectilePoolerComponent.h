
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/ProjectileType.h"
#include "ProjectilePoolerComponent.generated.h"

class ARPGBaseProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UProjectilePoolerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UProjectilePoolerComponent();

	void CreatePool(TSubclassOf<ARPGBaseProjectile> ProjClass, const int32 Size, const EProjectileType Type);

	void DestroyPool();

	ARPGBaseProjectile* GetPooledProjectile(APawn* CallerPawn, const float Damage);

protected:

	UFUNCTION()
	void AddDeactivatedNum();

private:

	UPROPERTY()
	TSubclassOf<ARPGBaseProjectile> ProjectileClass;

	UPROPERTY()
	TArray<ARPGBaseProjectile*> ProjectileArr;

	int32 PoolSize;

	int32 ActivatedNum;

	int32 DeactivatedNum;
};

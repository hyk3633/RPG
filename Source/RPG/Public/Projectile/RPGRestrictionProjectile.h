
#pragma once

#include "CoreMinimal.h"
#include "Projectile/RPGBaseProjectile.h"
#include "RPGRestrictionProjectile.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGRestrictionProjectile : public ARPGBaseProjectile
{
	GENERATED_BODY()
	
public:

	ARPGRestrictionProjectile();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	virtual void ProcessHitEvent(const FHitResult& HitResult) override;

};

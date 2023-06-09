
#pragma once

#include "CoreMinimal.h"
#include "Projectile/RPGBaseProjectile.h"
#include "RPGSpeedDownProjectile.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGSpeedDownProjectile : public ARPGBaseProjectile
{
	GENERATED_BODY()

public:

	ARPGSpeedDownProjectile();

	virtual void Tick(float DeltaTime) override;

	void SetHomingTarget(ACharacter* HomingTarget);

protected:

	virtual void BeginPlay() override;

	virtual void ProcessHitEvent(const FHitResult& HitResult) override;
	
	float VelocityLastFrame;

};

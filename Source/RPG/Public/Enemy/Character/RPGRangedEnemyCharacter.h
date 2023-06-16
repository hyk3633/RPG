
#pragma once

#include "CoreMinimal.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "RPGRangedEnemyCharacter.generated.h"

/**
 * 
 */

class ARPGBaseProjectile;

UCLASS()
class RPG_API ARPGRangedEnemyCharacter : public ARPGBaseEnemyCharacter
{
	GENERATED_BODY()

public:

	ARPGRangedEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Attack() override;

	void AnimNotify_LineTraceOnSocket();

	void LineTraceOnSocket();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectileMulticast(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	void SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

private:

	UPROPERTY(EditAnywhere, Category = "Bow")
	USkeletalMeshComponent* BowMesh;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ARPGBaseProjectile> ProjectileClass;

};

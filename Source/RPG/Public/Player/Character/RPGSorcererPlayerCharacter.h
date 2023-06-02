
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Projectile/RPGProjectile.h"
#include "RPGSorcererPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGProjectile;

UCLASS()
class RPG_API ARPGSorcererPlayerCharacter : public ARPGBasePlayerCharacter
{
	GENERATED_BODY()
	
public:

	ARPGSorcererPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	void DrawTargetingCursor();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

protected:

	virtual void CastNormalAttack() override;

	void SpawnProjectile();

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> ProjetileClass;
};

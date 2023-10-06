
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGBlackhole.generated.h"

class USphereComponent;

UCLASS()
class RPG_API ARPGBlackhole : public AActor
{
	GENERATED_BODY()
	
public:	

	ARPGBlackhole();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetDamage(const float& NewDamage) { Damage = NewDamage; }

protected:

	void SuckingUpEnemies();

	virtual void BeginPlay() override;

	void InitiateSuckingUpEnemies();

	void ApplyDamageToEnemies();

	void ExpireBlackhole();

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComponent;

	TSet<AActor*> OverlappingEnemies;

	FTimerHandle ExpireTimer;

	float ExpireTime = 4.f;

	bool bBlackholeOn = true;

	FTimerHandle DamageTimer;

	float Damage = 1.f;
};

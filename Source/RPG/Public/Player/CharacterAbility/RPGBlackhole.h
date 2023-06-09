
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

	void SuckingUpEnemies();

protected:

	virtual void BeginPlay() override;

	void InitiateSuckingUpEnemies();

	void ExpireBlackhole();

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* BlackholeParticle;

	TSet<AActor*> OverlappingEnemies;

	FTimerHandle ExpireTimer;

	float ExpireTime = 2.f;

	bool bBlackholeOn = true;
};

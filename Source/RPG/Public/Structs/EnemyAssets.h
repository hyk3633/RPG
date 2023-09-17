#pragma once

#include "Enums/ProjectileType.h"
#include "Enums/EnemyAttackType.h"
#include "Engine/DataTable.h"
#include "EnemyAssets.generated.h"

class ARPGBaseProjectile;
class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class USoundCue;

USTRUCT(BlueprintType)
struct FEnemyAssets : public FTableRowBase
{
	GENERATED_BODY()

public:

	FEnemyAssets() : BodyMesh(nullptr), WeaponMesh_Skeletal(nullptr), WeaponMesh_Static(nullptr), ProjectileClass(nullptr), AIController(nullptr), AnimInstance(nullptr) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	USkeletalMesh* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	USkeletalMesh* WeaponMesh_Skeletal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UStaticMesh* WeaponMesh_Static;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<ARPGBaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<ARPGEnemyAIController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<URPGEnemyAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EProjectileType ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EEnemyAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	USoundCue* MeleeHitSound;
};
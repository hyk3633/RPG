
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/ItemType.h"
#include "Containers/Union.h"
#include "RPGItem.generated.h"

class UWidgetComponent;

USTRUCT(Atomic)
struct FArmourStat
{
    GENERATED_BODY()
public:

    UPROPERTY()
    float DefensivePower;

    UPROPERTY()
    int16 ExtraHP;

    UPROPERTY()
    int16 ExtraMP;

    UPROPERTY()
    float Dexterity;

    FArmourStat() : DefensivePower(0), ExtraHP(0), ExtraMP(0), Dexterity(0) {}

};

USTRUCT(Atomic)
struct FWeaponStat
{
    GENERATED_BODY()
public:

    UPROPERTY()
    float StrikingPower;

    UPROPERTY()
    float SkillPower;

    UPROPERTY()
    float AttackSpeed;

    FWeaponStat() : StrikingPower(0), SkillPower(0), AttackSpeed(0) {}

};

union ItemStatUnion
{
    int32 CoinAmount;
    int16 RecoveryAmount;
    FArmourStat ArmourStat;
    FWeaponStat WeaponStat;

    ItemStatUnion(int32 CAmount) : CoinAmount(CAmount) {}
    ItemStatUnion(int16 RAmount) : RecoveryAmount(RAmount) {}
    ItemStatUnion(FArmourStat AStat) : ArmourStat(AStat) {}
    ItemStatUnion(FWeaponStat WStat) : WeaponStat(WStat) {}
};

USTRUCT(Atomic)
struct FItemInfo
{
    GENERATED_BODY()
public:

    UPROPERTY()
    EItemType ItemType;

    UPROPERTY()
    FString ItemName;

    ItemStatUnion ItemStat;

    FItemInfo() : ItemType(EItemType::EIT_Coin), ItemName(FString(TEXT(""))), ItemStat(0) {}
};

UCLASS()
class RPG_API ARPGItem : public AActor
{
	GENERATED_BODY()
	
public:	

	ARPGItem();

	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

    void SetItemInfo(FItemInfo NewItemInfo);

    FORCEINLINE const FItemInfo GetItemInfo() const { return ItemInfo; }

    void SetItemMesh(UStaticMesh* NewMesh);

protected:

    UFUNCTION(NetMulticast, Reliable)
    void SetItemMeshMulticast(UStaticMesh* NewMesh);

public:

	void SetItemNameTagVisibility(const bool bVisible);

	void DestroyFromAllClients();

private:

	UFUNCTION(NetMulticast, Reliable)
	void DestroyMulticast();

    UFUNCTION()
    void OnRep_ItemInfo();

protected:

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:

	UPROPERTY()
	USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* NameTagWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInfo)
	FItemInfo ItemInfo;
};

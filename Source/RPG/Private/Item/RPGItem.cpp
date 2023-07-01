
#include "Item/RPGItem.h"
#include "UI/RPGItemNameTag.h"
#include "../RPG.h"
#include "Components/WidgetComponent.h"

ARPGItem::ARPGItem()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(SceneComp);
	ItemMesh->SetCollisionObjectType(ECC_ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToChannel(ECC_PlayerBody, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_ItemTrace, ECollisionResponse::ECR_Block);

	NameTagWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTag Widget"));
	NameTagWidget->SetupAttachment(ItemMesh);
	NameTagWidget->SetVisibility(false);
}

void ARPGItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGItem::BeginPlay()
{
	Super::BeginPlay();
	
	URPGItemNameTag* NameTagText = Cast<URPGItemNameTag>(NameTagWidget->GetWidget());
	if (NameTagText)
	{
		NameTagText->SetNameTagText(ItemName);
	}
}

void ARPGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGItem::DestroyFromAllClients()
{
	DestroyMulticast();
}

void ARPGItem::DestroyMulticast_Implementation()
{
	Destroy();
}

void ARPGItem::SetItemNameTagVisibility(const bool bVisible)
{
	NameTagWidget->SetVisibility(bVisible);
}


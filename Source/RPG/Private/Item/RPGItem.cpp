
#include "Item/RPGItem.h"
#include "UI/RPGItemNameTag.h"
#include "../RPG.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ARPGItem::ARPGItem()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(SceneComp);
	ItemMesh->SetEnableGravity(true);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetCollisionObjectType(ECC_ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPAsset(TEXT("WidgetBlueprint'/Game/_Assets/Blueprints/HUD/WBP_ItemNameTag.WBP_ItemNameTag_C'"));
	if (WidgetBPAsset.Succeeded()) { NameTagWidget->SetWidgetClass(WidgetBPAsset.Class); }
	NameTagWidget->SetupAttachment(ItemMesh);
	NameTagWidget->SetWidgetSpace(EWidgetSpace::Screen);
	NameTagWidget->SetDrawSize(FVector2D(200, 50));
	NameTagWidget->SetVisibility(false);
}

void ARPGItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARPGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGItem::SetItemInfo(FItemInfo NewItemInfo)
{
	ItemInfo = NewItemInfo;
}

void ARPGItem::SetItemMesh(UStaticMesh* NewMesh)
{
	SetItemMeshMulticast(NewMesh);
}

void ARPGItem::SetItemMeshMulticast_Implementation(UStaticMesh* NewMesh)
{
	ItemMesh->SetStaticMesh(NewMesh);
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

void ARPGItem::OnRep_ItemInfo()
{
	URPGItemNameTag* NameTagText = Cast<URPGItemNameTag>(NameTagWidget->GetWidget());
	if (NameTagText)
	{
		NameTagText->SetNameTagText(ItemInfo.ItemName);
	}
}

void ARPGItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGItem, ItemInfo);
}
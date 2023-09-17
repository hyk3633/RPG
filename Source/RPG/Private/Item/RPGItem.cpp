
#include "Item/RPGItem.h"
#include "UI/RPGItemNameTag.h"
#include "../RPG.h"
#include "Components/WidgetComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "Net/UnrealNetwork.h"

ARPGItem::ARPGItem()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetVisibility(false);
	ItemMesh->SetupAttachment(SceneComp);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionObjectType(ECC_ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_ItemMesh, ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_ItemTrace, ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_IsSafeToSpawn, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_ObstacleCheck, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_HeightCheck, ECollisionResponse::ECR_Ignore);
	ItemMesh->bHiddenInSceneCapture = true;

	ItemIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite"));
	ItemIconSprite->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UPaperSprite> ItemIconAsset(TEXT("PaperSprite'/Game/_Assets/Texture2D/Minimap/ItemIcon_Sprite.ItemIcon_Sprite'"));
	if (ItemIconAsset.Succeeded()) { ItemIconSprite->SetSprite(ItemIconAsset.Object); }
	ItemIconSprite->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	ItemIconSprite->SetRelativeLocation(FVector(0, 0, 299));
	ItemIconSprite->SetRelativeScale3D(FVector(0.2f));
	ItemIconSprite->bVisibleInSceneCaptureOnly = true;
	ItemIconSprite->SetVisibility(false);

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

void ARPGItem::DeactivateItemFromAllClients()
{
	DeactivateItemMulticast();
}

void ARPGItem::DeactivateItemMulticast_Implementation()
{
	DeactivateItem();
}

void ARPGItem::DeactivateItem()
{
	ItemMesh->SetVisibility(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemIconSprite->SetVisibility(false);
	NameTagWidget->SetVisibility(false);
	if (HasAuthority()) DDeactivateItem.Broadcast();
}

void ARPGItem::ActivateItemFromAllClients(const FTransform& SpawnTransform)
{
	ActivateItemMulticast(SpawnTransform);
}

void ARPGItem::ActivateItemMulticast_Implementation(const FTransform& SpawnTransform)
{
	ActivateItem(SpawnTransform);
}

void ARPGItem::ActivateItem(const FTransform& SpawnTransform)
{
	SetActorTransform(SpawnTransform);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetEnableGravity(true);
	if (HasAuthority() == false)
	{
		ItemMesh->SetVisibility(true);
		ItemIconSprite->SetVisibility(true);
	}
}

void ARPGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGItem::SetItemInfo(const FItemInfo& NewItemInfo)
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

void ARPGItem::SetItemNameTagVisibility(const bool bVisible)
{
	NameTagWidget->SetVisibility(bVisible);
}

void ARPGItem::SetRenderCustomDepthOn(const int32 StencilValue)
{
	ItemMesh->SetRenderCustomDepth(true);
	ItemMesh->SetCustomDepthStencilValue(StencilValue);
}

void ARPGItem::SetRenderCustomDepthOff()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void ARPGItem::OnRep_ItemInfo()
{
	if (ItemInfo.ItemType == EItemType::EIT_Coin)
	{
		ItemMesh->SetWorldScale3D(FVector(0.75f));
	}
	else
	{
		ItemMesh->SetWorldScale3D(FVector(2.f));
		ItemMesh->SetMassScale(NAME_None, 10.f);
		ItemMesh->SetLinearDamping(2.f);
		ItemMesh->SetAngularDamping(0.5f);
	}
	SetItemTagText();
}

void ARPGItem::SetItemTagText()
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
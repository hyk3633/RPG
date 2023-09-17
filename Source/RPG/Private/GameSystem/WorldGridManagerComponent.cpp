
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/EnemySpawner.h"
#include "DataAsset/MapNavDataAsset.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DrawDebugHelpers.h"

UWorldGridManagerComponent::UWorldGridManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UWorldGridManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitWorldGrid();
}

void UWorldGridManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//DrawGrid();
}



void UWorldGridManagerComponent::DrawGrid()
{
	int32 Count = 0;
	while (Count < GridWidthSize * GridLengthSize)
	{
		int32 X = FieldLocations[Count].X;
		int32 Y = FieldLocations[Count].Y;
		if (ObstacleExtraCost[Count] == 0)
			DrawDebugPoint(GetWorld(), FVector(X, Y, FieldHeights[Count] + 10.f), 5.f, FColor::Green, true);
		else if (ObstacleExtraCost[Count] < 8)
			DrawDebugPoint(GetWorld(), FVector(X, Y, FieldHeights[Count] + 10.f), 5.f, FColor::Yellow, true);
		else if (ObstacleExtraCost[Count] < 12)
			DrawDebugPoint(GetWorld(), FVector(X, Y, FieldHeights[Count] + 10.f), 5.f, FColor::Orange, true);
		else
			DrawDebugPoint(GetWorld(), FVector(X, Y, FieldHeights[Count] + 10.f), 5.f, FColor::Red, true);
		Count++;
	}
}

void UWorldGridManagerComponent::InitWorldGrid()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*MapNavDataReference);
	if (AssetData.IsValid())
	{
		MapNavDataAsset = Cast<UMapNavDataAsset>(AssetData.GetAsset());
		
		bMapNavDataUsable = true;

		NavOrigin = MapNavDataAsset->NavOrigin;

		GridDist = MapNavDataAsset->GridDist;
		GridWidthSize = MapNavDataAsset->GridWidthSize;
		GridLengthSize = MapNavDataAsset->GridLengthSize;

		BiasX = MapNavDataAsset->BiasX;
		BiasY = MapNavDataAsset->BiasY;

		FieldLocations = MapNavDataAsset->FieldLocations;
		FieldHeights = MapNavDataAsset->FieldHeights;
		IsMovableArr = MapNavDataAsset->IsMovableArr;
		ObstacleExtraCost = MapNavDataAsset->ExtraCost;
	}
	else
	{
		ELOG(TEXT("Failed to load nav data asset!"));
		bMapNavDataUsable = false;
	}
}

void UWorldGridManagerComponent::AStarPlayer(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	double start = FPlatformTime::Seconds();

	PathToDest.Empty();

	const int32 DY = VectorToCoordinatesY(Dest.Y);
	const int32 DX = VectorToCoordinatesX(Dest.X);

	if (CanGo(FPos(DY, DX)) == false)
	{
		ELOG(TEXT("Can not move that point."));
		return;
	}

	const int32 SY = VectorToCoordinatesY(Start.Y);
	const int32 SX = VectorToCoordinatesX(Start.X);

	// 목적지가 현재 위치 보다 높은 곳 인지? (높은 곳이면 true)
	const bool bIsGoingUp = FieldHeights[DY * GridWidthSize + DX] - FieldHeights[SY * GridWidthSize + SX] > 0.f ? true : false;

	FPos StartPos(SY, SX);
	TArray<bool> Visited;
	Visited.Init(false, GridWidthSize * GridLengthSize);

	TArray<int32> Best;
	Best.Init(INT32_MAX, GridWidthSize * GridLengthSize);

	TMap<FPos, FPos> Parent;

	TArray<FAStarNode> HeapArr;
	HeapArr.Heapify();

	FPos DestPos(DY, DX);
	{
		int32 G = 0;
		int32 H = 10 * (abs(DestPos.Y - StartPos.Y) + abs(DestPos.X - StartPos.X));
		HeapArr.HeapPush(FAStarNode{ G + H, G, StartPos });
		Best[StartPos.Y * GridWidthSize + StartPos.X] = G + H;
		Parent.Add(StartPos, StartPos);
	}

	while (HeapArr.Num())
	{
		FAStarNode Node;
		HeapArr.HeapPop(Node);
		const int32 NodeIdx = Node.Pos.Y * GridWidthSize + Node.Pos.X;

		if (Visited[NodeIdx])
			continue;
		if (Best[NodeIdx] < Node.F)
			continue;

		Visited[NodeIdx] = true;

		if (Node.Pos == DestPos)
			break;

		for (int32 Dir = 0; Dir < 8; Dir++)
		{
			FPos NextPos = Node.Pos + Front[Dir];
			const int32 NextIdx = NextPos.Y * GridWidthSize + NextPos.X;

			if (CanGo(NextPos) == false)
				continue;
			if (Visited[NextIdx])
				continue;

			// 현재 그리드와 다음 그리드의 높이 차를 가중치로 부여
			const int32 HeightConst = FMath::TruncToInt(FieldHeights[NodeIdx] - FieldHeights[NextIdx]) * (bIsGoingUp ? 100.f : -100.f);
			const int32 G = Node.G + Cost[Dir] + ObstacleExtraCost[NextIdx] + HeightConst;

			const int32 H = 10 * (abs(DestPos.Y - NextPos.Y) + abs(DestPos.X - NextPos.X));
			if (Best[NextIdx] <= G + H)
				continue;

			Best[NextIdx] = G + H;
			HeapArr.HeapPush(FAStarNode{ G + H, G, NextPos });
			Parent.Add(NextPos, Node.Pos);
		}
	}

	FPos NextPos = DestPos;

	while (true)
	{
		PathToDest.Add(FieldLocations[NextPos.Y * GridWidthSize + NextPos.X]);

		if (NextPos == *Parent.Find(NextPos)) break;

		NextPos = Parent[NextPos];
	}

	Algo::Reverse(PathToDest);

	double end = FPlatformTime::Seconds();
	//PLOG(TEXT("time : %f"), end - start);

	// 경로 디버그
	for (int32 i = 0; i < PathToDest.Num(); i++)
	{
		int32 Y = VectorToCoordinatesY(PathToDest[i].Y);
		int32 X = VectorToCoordinatesY(PathToDest[i].X);
		DrawDebugPoint(GetWorld(), FVector(PathToDest[i].X, PathToDest[i].Y, FieldHeights[Y * GridWidthSize + X] + 10.f), 10.f, FColor::Blue, false, 2.f);
	}
}

int32 UWorldGridManagerComponent::VectorToCoordinatesY(const double& VectorComponent)
{
	return FMath::Floor(((VectorComponent - NavOrigin.Y + BiasY) / GridDist) + 0.5f);
}

int32 UWorldGridManagerComponent::VectorToCoordinatesX(const double& VectorComponent)
{
	return FMath::Floor(((VectorComponent - NavOrigin.X + BiasX) / GridDist) + 0.5f);
}

int32 UWorldGridManagerComponent::CoordinatesToVectorY(const int32 Coordinates)
{
	return (GridDist * Coordinates) - BiasY;
}

int32 UWorldGridManagerComponent::CoordinatesToVectorX(const int32 Coordinates)
{
	return (GridDist * Coordinates) - BiasX;
}

bool UWorldGridManagerComponent::CanGo(const FPos& _Pos)
{
	if (_Pos.Y >= 0 && _Pos.Y < GridLengthSize && _Pos.X >= 0 && _Pos.X < GridWidthSize)
	{
		if(IsMovableArr[_Pos.Y * GridWidthSize + _Pos.X]) return true;
	}
	return false;
}


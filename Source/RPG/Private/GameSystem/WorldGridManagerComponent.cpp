
#include "GameSystem/WorldGridManagerComponent.h"
#include "DataAsset/MapNavDataAsset.h"
#include "../RPG.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "DrawDebugHelpers.h"

UWorldGridManagerComponent::UWorldGridManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWorldGridManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitWorldGrid();
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

		WorldOffsetX = MapNavDataAsset->WorldOffsetX;
		WorldOffsetY = MapNavDataAsset->WorldOffsetY;

		FieldLocations = MapNavDataAsset->FieldLocations;
		IsMovableArr = MapNavDataAsset->IsMovableArr;
	}
	else
	{
		bMapNavDataUsable = false;
	}
}

void UWorldGridManagerComponent::AStar(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	//PLOG(TEXT("vec - Y : %f, X : %f"), Dest.Y, Dest.X);
	//int32 nY = FMath::Floor(((Dest.Y - NavOrigin.Y + FMath::TruncToInt(937.5)) / 25) + 0.5f);
	//int32 nX = FMath::Floor(((Dest.X - NavOrigin.X + FMath::TruncToInt(937.5)) / 25) + 0.5f);
	//PLOG(TEXT("Dest - Y : %d, X : %d"), nY, nX);
	//PLOG(TEXT("Arr - Y : %d, X : %d"), FieldLocations[nY * GridLengthSize + nX].Y, FieldLocations[nY * GridLengthSize + nX].X);
	//
	//return;

	double start = FPlatformTime::Seconds();

	PathToDest.Empty();

	const int32 Dy = VectorToCoordinatesY(Dest.Y);
	const int32 Dx = VectorToCoordinatesX(Dest.X);

	if (Dy >= GridLengthSize || Dx >= GridWidthSize)
	{
		ELOG(TEXT("Can not move that point."));
		return;
	}

	const int32 SY = VectorToCoordinatesY(Start.Y);
	const int32 SX = VectorToCoordinatesX(Start.X);

	FPos StartPos(SY, SX);
	TArray<bool> Visited;
	Visited.Init(false, GridWidthSize * GridLengthSize);

	TArray<int32> Best;
	Best.Init(INT32_MAX, GridWidthSize * GridLengthSize);

	TMap<FPos, FPos> Parent;

	TArray<FAStarNode> HeapArr;
	HeapArr.Heapify();

	FPos DestPos(Dy, Dx);
	{
		int32 G = 0;
		int32 H = 10 * (abs(DestPos.Y - StartPos.Y) + abs(DestPos.X - StartPos.X));
		HeapArr.HeapPush(FAStarNode{ G + H, G, StartPos });
		Best[StartPos.Y * GridLengthSize + StartPos.X] = G + H;
		Parent.Add(StartPos, StartPos);
	}

	while (HeapArr.Num())
	{
		FAStarNode Node;
		HeapArr.HeapPop(Node);

		if (Visited[Node.Pos.Y * GridLengthSize + Node.Pos.X])
			continue;
		if (Best[Node.Pos.Y * GridLengthSize + Node.Pos.X] < Node.F)
			continue;

		Visited[Node.Pos.Y * GridLengthSize + Node.Pos.X] = true;

		if (Node.Pos == DestPos)
			break;

		for (int32 Dir = 0; Dir < 8; Dir++)
		{
			FPos NextPos = Node.Pos + Front[Dir];
			if (CanGo(NextPos) == false)
				continue;
			if (Visited[NextPos.Y * GridLengthSize + NextPos.X])
				continue;

			int32 G = Node.G + Cost[Dir];
			int32 H = 10 * (abs(DestPos.Y - NextPos.Y) + abs(DestPos.X - NextPos.X));
			if (Best[NextPos.Y * GridLengthSize + NextPos.X] <= G + H)
				continue;

			Best[NextPos.Y * GridLengthSize + NextPos.X] = G + H;
			HeapArr.HeapPush(FAStarNode{ G + H, G, NextPos });
			Parent.Add(NextPos, Node.Pos);
		}
	}

	FPos NextPos = DestPos;

	while (true)
	{
		//const int32 Y = CoordinatesToVectorY(NextPos.Y);
		//const int32 X = CoordinatesToVectorX(NextPos.X);

		//PathToDest.Add(FPos(Y,X));
		PathToDest.Add(FieldLocations[NextPos.Y * GridLengthSize + NextPos.X]);

		if (NextPos == *Parent.Find(NextPos))
		{
			break;
		}

		NextPos = Parent[NextPos];
	}

	Algo::Reverse(PathToDest);

	double end = FPlatformTime::Seconds();
	PLOG(TEXT("time : %f"), end - start);
}

int32 UWorldGridManagerComponent::VectorToCoordinatesY(const double& VectorComponent)
{
	return FMath::Floor(((VectorComponent - NavOrigin.Y + FMath::TruncToInt(WorldOffsetY)) / GridDist) + 0.5f);
}

int32 UWorldGridManagerComponent::VectorToCoordinatesX(const double& VectorComponent)
{
	return FMath::Floor(((VectorComponent - NavOrigin.X + FMath::TruncToInt(WorldOffsetX)) / GridDist) + 0.5f);
}

int32 UWorldGridManagerComponent::CoordinatesToVectorY(const int32 Coordinates)
{
	return (GridDist * Coordinates) - FMath::TruncToInt(WorldOffsetY);
}

int32 UWorldGridManagerComponent::CoordinatesToVectorX(const int32 Coordinates)
{
	return (GridDist * Coordinates) - FMath::TruncToInt(WorldOffsetX);
}

bool UWorldGridManagerComponent::CanGo(const FPos& _Pos)
{
	if (_Pos.Y >= 0 && _Pos.Y < GridLengthSize && _Pos.X >= 0 && _Pos.X < GridWidthSize)
	{
		if(IsMovableArr[_Pos.Y * GridLengthSize + _Pos.X]) return true;
	}
	return false;
}


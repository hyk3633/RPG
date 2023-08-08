
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
	DrawGrid();
}

void UWorldGridManagerComponent::DrawGrid()
{
	int32 Count = 0;
	while (Count < GridWidthSize * GridLengthSize)
	{
		int32 X = FieldLocations[Count].X;
		int32 Y = FieldLocations[Count].Y;
		if (ExtraCost[Count] == 0)
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Green, true);
		else if (ExtraCost[Count] < 8)
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Yellow, true);
		else if (ExtraCost[Count] < 12)
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Orange, true);
		else
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Red, true);
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

		WorldOffsetX = MapNavDataAsset->WorldOffsetX;
		WorldOffsetY = MapNavDataAsset->WorldOffsetY;

		FieldLocations = MapNavDataAsset->FieldLocations;
		IsMovableArr = MapNavDataAsset->IsMovableArr;
		ExtraCost = MapNavDataAsset->ExtraCost;
	}
	else
	{
		bMapNavDataUsable = false;
	}
}

void UWorldGridManagerComponent::AStar(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	double start = FPlatformTime::Seconds();

	PathToDest.Empty();

	const int32 Dy = VectorToCoordinatesY(Dest.Y);
	const int32 Dx = VectorToCoordinatesX(Dest.X);

	if (CanGo(FPos(Dy,Dx)) == false)
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

			int32 G = Node.G + Cost[Dir] + ExtraCost[NextPos.Y * GridLengthSize + NextPos.X];
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
		PathToDest.Add(FieldLocations[NextPos.Y * GridLengthSize + NextPos.X]);

		if (NextPos == *Parent.Find(NextPos)) break;

		NextPos = Parent[NextPos];
	}

	Algo::Reverse(PathToDest);

	double end = FPlatformTime::Seconds();
	PLOG(TEXT("time : %f"), end - start);

	for (int32 i = 0; i < PathToDest.Num(); i++)
	{
		DrawDebugPoint(GetWorld(), FVector(PathToDest[i].X, PathToDest[i].Y, 10.f), 10.f, FColor::Blue, false, 2.f);
	}
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



#include "GameSystem/WorldGridManagerComponent.h"
#include "../RPG.h"
#include <map>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

UWorldGridManagerComponent::UWorldGridManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWorldGridManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWorldGridManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWorldGridManagerComponent::InitWorldGrid(int32 Number, int32 Interval)
{
	GridSize = Number;
	GridDist = Interval;
	WorldOffset = ((GridSize * GridDist) / 2.f) - (GridDist / 2.f);
}

void UWorldGridManagerComponent::AStar(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	double start = FPlatformTime::Seconds();

	PathToDest.Empty();

	const int32 Dy = VectorToCoordinates(Dest.Y);
	const int32 Dx = VectorToCoordinates(Dest.X);

	if (Dy * GridSize + Dx > GridSize * GridSize)
	{
		ELOG(TEXT("Can not move that point."));
		return;
	}

	const int32 SY = VectorToCoordinates(Start.Y);
	const int32 SX = VectorToCoordinates(Start.X);

	FPos StartPos(SY, SX);
	TArray<bool> Visited;
	Visited.Init(false, GridSize * GridSize);

	TArray<int32> Best;
	Best.Init(INT32_MAX, GridSize * GridSize);

	TMap<FPos, FPos> Parent;

	TArray<FAStarNode> HeapArr;
	HeapArr.Heapify();

	FPos DestPos(Dy, Dx);
	{
		int32 G = 0;
		int32 H = 10 * (abs(DestPos.Y - StartPos.Y) + abs(DestPos.X - StartPos.X));
		HeapArr.HeapPush(FAStarNode{ G + H, G, StartPos });
		Best[StartPos.Y * GridSize + StartPos.X] = G + H;
		Parent.Add(StartPos, StartPos);
	}

	while (HeapArr.Num())
	{
		FAStarNode Node;
		HeapArr.HeapPop(Node);

		if (Visited[Node.Pos.Y * GridSize + Node.Pos.X])
			continue;
		if (Best[Node.Pos.Y * GridSize + Node.Pos.X] < Node.F)
			continue;

		Visited[Node.Pos.Y * GridSize + Node.Pos.X] = true;

		if (Node.Pos == DestPos)
			break;

		for (int32 Dir = 0; Dir < 8; Dir++)
		{
			FPos NextPos = Node.Pos + Front[Dir];
			if (CanGo(NextPos) == false)
				continue;
			if (Visited[NextPos.Y * GridSize + NextPos.X])
				continue;

			int32 G = Node.G + Cost[Dir];
			int32 H = 10 * (abs(DestPos.Y - NextPos.Y) + abs(DestPos.X - NextPos.X));
			if (Best[NextPos.Y * GridSize + NextPos.X] <= G + H)
				continue;

			Best[NextPos.Y * GridSize + NextPos.X] = G + H;
			HeapArr.HeapPush(FAStarNode{ G + H, G, NextPos });
			Parent.Add(NextPos, Node.Pos);
		}
	}

	FPos NextPos = DestPos;

	while (true)
	{
		const int32 X = (GridDist * NextPos.X) - FMath::TruncToInt(WorldOffset);
		const int32 Y = (GridDist * NextPos.Y) - FMath::TruncToInt(WorldOffset);

		PathToDest.Add(FPos(Y,X));

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

int32 UWorldGridManagerComponent::VectorToCoordinates(const double& VectorComponent)
{
	return FMath::Floor(((VectorComponent + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);;
}

bool UWorldGridManagerComponent::CanGo(const FPos& _Pos)
{
	if (_Pos.Y >= 0 && _Pos.Y < GridSize && _Pos.X >= 0 && _Pos.X < GridSize)
		return true;
	return false;
}



#include "GameSystem/WorldGridManagerComponent.h"
#include "../RPG.h"

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

void UWorldGridManagerComponent::AStar(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY)
{
	const int32 SY = FMath::Floor(((Start.Y + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	const int32 SX = FMath::Floor(((Start.X + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);

	FPos StartPos(SY, SX);
	TArray<bool> Visited;
	Visited.Init(false, GridSize * GridSize);

	TArray<int32> Best;
	Best.Init(INT32_MAX, GridSize * GridSize);

	TMap<FPos, FPos> Parent;

	TArray<FAStarNode> HeapArr;
	HeapArr.Heapify();

	const int32 Dy = FMath::Floor(((Dest.Y + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	const int32 Dx = FMath::Floor(((Dest.X + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	FPos DestPos(Dy, Dx);
	{
		int32 G = 0;
		int32 H = 10 * (abs(DestPos.Y - StartPos.Y) + abs(DestPos.X - StartPos.X));
		HeapArr.HeapPush(FAStarNode{ G + H, G, StartPos });
		Best[StartPos.Y * GridSize + StartPos.X];
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
	PathToDestX.Empty();
	PathToDestY.Empty();

	while (true)
	{
		const float X = (GridDist * NextPos.X) - FMath::TruncToInt(WorldOffset);
		const float Y = (GridDist * NextPos.Y) - FMath::TruncToInt(WorldOffset);

		PathToDestX.Add(X);
		PathToDestY.Add(Y);

		if (NextPos == *Parent.Find(NextPos))
		{
			break;
		}

		NextPos = Parent[NextPos];
	}

	Algo::Reverse(PathToDestX);
	Algo::Reverse(PathToDestY);
}

bool UWorldGridManagerComponent::CanGo(const FPos& _Pos)
{
	if (_Pos.Y >= 0 && _Pos.Y < GridSize && _Pos.X >= 0 && _Pos.X < GridSize)
		return true;
	return false;
}


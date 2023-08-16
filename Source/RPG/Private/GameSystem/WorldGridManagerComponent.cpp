
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
	
	DrawGrid();
}

void UWorldGridManagerComponent::DrawGrid()
{
	int32 Count = 0;
	while (Count < GridWidthSize * GridLengthSize)
	{
		int32 X = FieldLocations[Count].X;
		int32 Y = FieldLocations[Count].Y;
		if (ObstacleExtraCost[Count] == 0)
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Green, true);
		else if (ObstacleExtraCost[Count] < 8)
			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Yellow, true);
		else if (ObstacleExtraCost[Count] < 12)
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

		BiasX = MapNavDataAsset->BiasX;
		BiasY = MapNavDataAsset->BiasY;

		FieldLocations = MapNavDataAsset->FieldLocations;
		IsMovableArr = MapNavDataAsset->IsMovableArr;
		ObstacleExtraCost = MapNavDataAsset->ExtraCost;

		CharacterExtraCost.Init(0, GridWidthSize * GridLengthSize);
		EnemyPathExtraCost.Init(0, GridWidthSize * GridLengthSize);
	}
	else
	{
		bMapNavDataUsable = false;
	}
}

void UWorldGridManagerComponent::UpdateCharacterExtraCost(int32& CoordinateY, int32& CoordinateX, const FVector& Location)
{
	// 기존 위치 지우기
	if (CoordinateY != -1 && CoordinateX != -1)
	{
		//FlushPersistentDebugLines(GetWorld());
		UpdateCharacterExtraCostValue(CoordinateY, CoordinateX, 0);
	}

	// 새로 위치 기록
	CoordinateY = VectorToCoordinatesY(Location.Y);
	CoordinateX = VectorToCoordinatesX(Location.X);
	if (CanGo(FPos(CoordinateY, CoordinateX)))
	{
		UpdateCharacterExtraCostValue(CoordinateY, CoordinateX, 0);
	}
}

void UWorldGridManagerComponent::UpdateCharacterExtraCostValue(const int32& CoordinateY, const int32& CoordinateX, const int8 Value)
{
	const int32 Sy = CoordinateY - 2;
	const int32 Sx = CoordinateX - 2;
	for (int32 Y = Sy; Y < Sy + 5; Y++)
	{
		for (int32 X = Sx; X < Sx + 5; X++)
		{
			// IsMovableArr로 옮기기
			CharacterExtraCost[Y * GridLengthSize + X] = Value;
			if(Value > 0)
			{
				DrawDebugPoint(GetWorld(),
					FVector(FieldLocations[Y * GridLengthSize + X].X, FieldLocations[Y * GridLengthSize + X].Y, 15.f), 5.f, FColor::Purple, false, 5.f);
			}
		}
	}
}

void UWorldGridManagerComponent::AStarPlayer(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	double start = FPlatformTime::Seconds();

	PathToDest.Empty();

	const int32 Dy = VectorToCoordinatesY(Dest.Y);
	const int32 Dx = VectorToCoordinatesX(Dest.X);

	if (CanGo(FPos(Dy, Dx)) == false)
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
		const int32 NodeIdx = Node.Pos.Y * GridLengthSize + Node.Pos.X;

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
			const int32 NextIdx = NextPos.Y * GridLengthSize + NextPos.X;

			if (CanGo(NextPos) == false)
				continue;
			if (Visited[NextIdx])
				continue;

			int32 G = Node.G + Cost[Dir] + ObstacleExtraCost[NextIdx];

			int32 H = 10 * (abs(DestPos.Y - NextPos.Y) + abs(DestPos.X - NextPos.X));
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

void UWorldGridManagerComponent::AStarEnemy(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest, AEnemySpawner* Spawner)
{
	double start = FPlatformTime::Seconds();

	for (FPos& Pos : PathToDest)
	{
		const int32 Y = VectorToCoordinatesY(Pos.Y);
		const int32 X = VectorToCoordinatesX(Pos.X);
		EnemyPathExtraCost[Y * GridLengthSize + X] = 0;
	}
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

	int8 CurrentCost = 14;
	TArray<FPos> EnemyPathArr;
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
			const int32 NextIdx = NextPos.Y * GridLengthSize + NextPos.X;

			if (!IsMovableArr[NextIdx])
				continue;
			if (Visited[NextIdx])
				continue;
			if (!Spawner->IsGridInArea(NextPos))
				continue;

			int32 G = Node.G + Cost[Dir] + ObstacleExtraCost[NextIdx] + EnemyPathExtraCost[NextIdx];

			int32 H = 10 * (abs(DestPos.Y - NextPos.Y) + abs(DestPos.X - NextPos.X));
			if (Best[NextIdx] <= G + H)
				continue;

			EnemyPathArr.Add(NextPos);
			EnemyPathExtraCost[NextIdx] = CurrentCost;

			Best[NextIdx] = G + H;
			HeapArr.HeapPush(FAStarNode{ G + H, G, NextPos });
			Parent.Add(NextPos, Node.Pos);
		}
	}

	while (!EnemyPathArr.IsEmpty() && CurrentCost >= 10)
	{
		FPos CPos;
		TArray<FPos> TempArr;
		for (int32 Layer = 0; Layer < EnemyPathArr.Num(); Layer++)
		{
			int32 CY = EnemyPathArr[Layer].Y , CX = EnemyPathArr[Layer].X;
			for (int8 Idx = 0; Idx < 4; Idx++)
			{
				int32 NY = CY + Front[Idx].Y, NX = CX + Front[Idx].X;
				FPos NextPos = FPos(NY, NX);
				const int32 NextIdx = NextPos.Y * GridLengthSize + NextPos.X;
				if (Spawner->IsGridInArea(NextPos) && EnemyPathExtraCost[NextIdx] < CurrentCost)
				{
					EnemyPathExtraCost[NextIdx] = CurrentCost;
					if(CurrentCost > 10) TempArr.Add(NextPos);
				
					if (CurrentCost == 12)
					{
						DrawDebugPoint(GetWorld(), FVector(FieldLocations[NextIdx].X, FieldLocations[NextIdx].Y, 15), 10.f, FColor::Purple, false, 5.f);
					}
					else
					{
						DrawDebugPoint(GetWorld(), FVector(FieldLocations[NextIdx].X, FieldLocations[NextIdx].Y, 15), 10.f, FColor::Cyan, false, 5.f);
					}
				}
			}
		}
		EnemyPathArr = TempArr;
		CurrentCost -= 2;
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
		DrawDebugPoint(GetWorld(), FVector(PathToDest[i].X, PathToDest[i].Y, 10.f), 10.f, FColor::Magenta, false, 5.f);
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
		if(IsMovableArr[_Pos.Y * GridLengthSize + _Pos.X]) return true;
	}
	return false;
}


#pragma once

#include "Pos.generated.h"

USTRUCT()
struct FPos
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 Y;
	UPROPERTY()
	int32 X;

	FPos(int32 _Y, int32 _X) : Y(_Y), X(_X) {}
	FPos() : Y(0), X(0) {}
	FPos operator+(FPos& _Pos)
	{
		FPos Ret{ Y + _Pos.Y , X + _Pos.X };
		return Ret;
	}
	bool operator<(const FPos& Other) const
	{
		if (Y != Other.Y)
			return Y < Other.Y;
		return X < Other.X;
	}
	bool operator!=(const FPos& Other) const
	{
		return (Y != Other.Y || X != Other.X);
	}
	float GetDistance(FPos& Other)
	{
		return FMath::Sqrt(FMath::Pow(X - Other.X, 2.f)) + FMath::Sqrt(FMath::Pow(Y - Other.Y, 2.f));
	}

	friend uint32 GetTypeHash(const FPos& Other)
	{
		return GetTypeHash(Other.Y) + GetTypeHash(Other.X);
	}
};

inline bool operator==(const FPos& L, const FPos& R) {
	return (L.Y == R.Y && L.X == R.X);
}

inline FPos operator+(const FPos& APos, FPos& BPos)
{
	FPos Ret{ APos.Y + BPos.Y, APos.X + BPos.X };
	return Ret;
}
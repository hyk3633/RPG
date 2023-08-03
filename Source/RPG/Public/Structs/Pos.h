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
		FPos Ret;
		Ret.Y = Y + _Pos.Y;
		Ret.X = X + _Pos.X;
		return Ret;
	}
	bool operator<(const FPos& Other) const
	{
		if (Y != Other.Y)
			return Y < Other.Y;
		return X < Other.X;
	}

	friend uint32 GetTypeHash(const FPos& Other)
	{
		return GetTypeHash(Other.Y) + GetTypeHash(Other.X);
	}
};

inline bool operator==(const FPos& L, const FPos& R) {
	return (L.Y == R.Y && L.X == R.X);
}

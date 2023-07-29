

#include "DamageType/DamageTypeBase.h"

int32 UDamageTypeBase::CalculateDamage(const float& Damage, const float& DefensivePower)
{
	return FMath::CeilToInt32((Damage * (FMath::RandRange(70, 100))) * (1 - ((DefensivePower * (FMath::RandRange(30, 60) / 10)) / 100)));
}

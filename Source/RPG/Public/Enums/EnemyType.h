#pragma once

UENUM()
enum class EEnemyType : uint8
{
	EET_Skeleton,
	EET_SkeletonArcher,
	EET_SkeletonSword,
	EET_ArmouredSkeleton,
	EET_ArmouredSkeletonArcher,
	EET_ArmouredSkeletonSword,
	EET_Ghoul,
	EET_ArmouredGhoul,
	EET_Lich,
	EET_ArmouredLich,
	EET_Boss,

	EET_Max
	
};
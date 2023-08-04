// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define ECC_PlayerBody			ECollisionChannel::ECC_GameTraceChannel1
#define ECC_EnemyBody			ECollisionChannel::ECC_GameTraceChannel2
#define ECC_PlayerAttack		ECollisionChannel::ECC_GameTraceChannel3
#define ECC_EnemyAttack			ECollisionChannel::ECC_GameTraceChannel4
#define ECC_PlayerProjectile	ECollisionChannel::ECC_GameTraceChannel5
#define ECC_EnemyProjectile		ECollisionChannel::ECC_GameTraceChannel6
#define ECC_ItemMesh			ECollisionChannel::ECC_GameTraceChannel7
#define ECC_ItemTrace			ECollisionChannel::ECC_GameTraceChannel8
#define ECC_CursorTrace			ECollisionChannel::ECC_GameTraceChannel9
#define ECC_GroundTrace			ECollisionChannel::ECC_GameTraceChannel10
#define ECC_ObstacleCheck		ECollisionChannel::ECC_GameTraceChannel11


#define CALLINFO			(FString(__FUNCTION__) + TEXT("(")+FString::FromInt(__LINE__) + TEXT(")")) 

#define CF()				UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO)
#define WLOG(format)		UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *CALLINFO, *FString::Printf(format))
#define ELOG(format)		UE_LOG(LogTemp, Error, TEXT("%s : %s"), *CALLINFO, *FString::Printf(format))
#define PLOG(format, ...)	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))

static const int32 MAX_DEFENSIVE = 5;
static const int32 MAX_DEXTERITY = 2;
static const int32 MAX_MAXHP = 200;
static const int32 MAX_MAXMP = 300;

static const int32 MIN_DEFENSIVE = 0;
static const int32 MIN_DEXTERITY = 1;
static const int32 MIN_MAXHP = 0;
static const int32 MIN_MAXMP = 0;

static const int32 MAX_STRIKINGPOWER = 5;
static const int32 MAX_SKILLPOWER = 5;
static const int32 MAX_ATTACKSPEED = 2;

static const int32 MIN_STRIKINGPOWER = 0;
static const int32 MIN_SKILLPOWER = 0;
static const int32 MIN_ATTACKSPEED = 1;
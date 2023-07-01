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

#define CALLINFO			(FString(__FUNCTION__) + TEXT("(")+FString::FromInt(__LINE__) + TEXT(")")) 

#define CF()				UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO)
#define WLOG(format)		UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *CALLINFO, *FString::Printf(format))
#define ELOG(format)		UE_LOG(LogTemp, Error, TEXT("%s : %s"), *CALLINFO, *FString::Printf(format))
#define PLOG(format, ...)	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
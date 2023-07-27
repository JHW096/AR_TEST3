// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/AICharacter.h"
#include <Global/GlobalEnums.h>
#include <Global/Data/MonsterData.h>
#include "Monster.generated.h"

/**
 * 
 */
UCLASS()
class AE_UNREAL_API AMonster : public AAICharacter
{
	GENERATED_BODY()

	const struct FMonsterData* CurMonsterData;

	void BeginPlay() override;

private:
	UPROPERTY(Category = "Animation", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName DataName = "NONE";
};

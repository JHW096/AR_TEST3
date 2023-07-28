// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster.h"
#include <Global/GlobalGameInstance.h>
#include <Global/Data/MonsterData.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"


void AMonster::BeginPlay()
{
	UGlobalGameInstance* Inst = GetWorld()->GetGameInstance<UGlobalGameInstance>();

	if (nullptr != Inst)
	{
		CurMonsterData = Inst->GetMonsterData(DataName);

		SetAllAnimation(CurMonsterData->MapAnimation);
		SetAniState(AIState::DEATH);
	}

	Super::BeginPlay();

	GetBlackboardComponent()->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(AIState::IDLE));
	GetBlackboardComponent()->SetValueAsString(TEXT("TargetTag"), TEXT("Player"));
	GetBlackboardComponent()->SetValueAsFloat(TEXT("SearchRange"), 1500.0f);
	GetBlackboardComponent()->SetValueAsFloat(TEXT("AttackRange"), 200.0f);

	float RangeOver = GetBlackboardComponent()->GetValueAsFloat(TEXT("SearchRange"));
	
	//TEST추가 코드
	GetBlackboardComponent()->SetValueAsVector(TEXT("SpawnPos"), GetActorLocation());
	GetBlackboardComponent()->SetValueAsFloat(TEXT("RangeOver"), RangeOver * 1.5f);

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsHitted"), false);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMonster::BeginOverLap);
	
	
	
}

void AMonster::BeginOverLap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if(OtherComp->GetCollisionProfileName() == TEXT("PlayerAttack"))
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsHitted"), true);
	};
}

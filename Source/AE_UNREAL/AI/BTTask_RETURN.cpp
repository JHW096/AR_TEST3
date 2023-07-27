// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RETURN.h"


UBTTask_RETURN::UBTTask_RETURN()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}


EBTNodeResult::Type UBTTask_RETURN::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	GetGlobalCharacter(OwnerComp)->SetAniState(AIState::RETURN);

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_RETURN::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelataSeconds)
{
	//TEST Code Ãß°¡
	Super::TickTask(OwnerComp, NodeMemory, DelataSeconds);

	int a = 0;

	{
		FVector CurrentMonsterPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
		CurrentMonsterPos.Z = 0.0f;

		FVector MonsterSpawnPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("SpawnPos"));
		MonsterSpawnPos.Z = 0.0f;

		FVector Dir = MonsterSpawnPos - CurrentMonsterPos;
		Dir.Normalize();

		FVector OtherForward = GetGlobalCharacter(OwnerComp)->GetActorForwardVector();
		OtherForward.Normalize();

		FVector Cross = FVector::CrossProduct(OtherForward, Dir);


		float Angle0 = Dir.Rotation().Yaw;
		float Angle1 = OtherForward.Rotation().Yaw;

		if (FMath::Abs(Angle0 - Angle1) >= 10.0f)
		{
			FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 1000.0f * DelataSeconds });
			GetGlobalCharacter(OwnerComp)->AddActorWorldRotation(Rot);
		}
		else
		{
			FRotator Rot = Dir.Rotation();
			GetGlobalCharacter(OwnerComp)->SetActorRotation(Rot);
		}
	}

	{
		FVector CurrentMonsterPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
		CurrentMonsterPos.Z = 0.0f;

		FVector MonsterSpawnPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("SpawnPos"));
		MonsterSpawnPos.Z = 0.0f;

		FVector Dir = MonsterSpawnPos - CurrentMonsterPos;

		Dir.Normalize();

		GetGlobalCharacter(OwnerComp)->AddMovementInput(Dir);

		float Distance = (MonsterSpawnPos - CurrentMonsterPos).Size();

		float OverRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("RangeOver"));

		if (Distance <= 5.0f)
		{
			SetStateChange(OwnerComp, AIState::IDLE);
		}
	}
}

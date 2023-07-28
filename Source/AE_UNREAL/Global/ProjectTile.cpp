// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/ProjectTile.h"
#include <Global/GlobalEnums.h>
#include "Global/AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <AI/AICon.h>

// Sets default values
AProjectTile::AProjectTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionProfileName(TEXT("NoCollision"), true);
}

// Called when the game starts or when spawned
void AProjectTile::BeginPlay()
{
	Super::BeginPlay();

	OnDestroyed.AddDynamic(this, &AProjectTile::DestroyProjectile);
	// SphereComponent->SetCollisionProfileName(TEXT("MonsterAttack"), true);

	//GetSphereComponent()->OnComponentBeginOverlap.AddDynamic(this, &AProjectTile::BeginOverLap);
}

void AProjectTile::DestroyProjectile(AActor* _Destroy)
{
	if (nullptr == DeathCreateObject)
	{
		return;
	}

	AActor* Actor = GetWorld()->SpawnActor<AActor>(DeathCreateObject);

	Actor->SetActorLocation(GetActorLocation());
	Actor->SetActorRotation(GetActorRotation());
}


// DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(AActor*, DestroyedActor);

// Called every frame
void AProjectTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeathTime -= DeltaTime;

	if (DeathTime < 0.0f)
	{
		Destroy();
		return;
	}

	AddActorWorldOffset(GetActorForwardVector() * DeltaTime * Speed);

}


//void AProjectTile::BeginOverLap(
//	UPrimitiveComponent* OverlappedComponent, 
//	AActor* OtherActor, 
//	UPrimitiveComponent* OtherComp, 
//	int32 OtherBodyIndex, 
//	bool bFromSweep, 
//	const FHitResult& SweepResult)
//{
//	if (OtherActor->ActorHasTag(TEXT("Monster")))
//	{
//		int a = 0;
//		AAICharacter* AIMonster = Cast<AAICharacter>(OtherActor);
//		AIMonster->GetBlackboardComponent()->SetValueAsBool(TEXT("IsHitted"), true);
//	}
//
//}


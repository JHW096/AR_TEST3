// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include <Global/GlobalGameInstance.h>
#include "Camera/CameraComponent.h"
#include <Global/ProjectTile.h>
#include "AIEnum.h"

AAIPlayerCharacter::AAIPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->SetupAttachment(RootComponent);


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->FieldOfView = 45.0f;
	CameraComponent->SetupAttachment(SpringArmComponent);
	// SpringArmComponent
}

void AAIPlayerCharacter::Tick(float _Delta)
{
	// SpringArmComponent->AddLocalRotation();
	// AddControllerYawInput(100);
}

void AAIPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		// 여기의 내용은 뭐냐?
		// DefaultPawn_MoveForward 추가되는것 뿐
		// 축매핑만 하고 있스니다.
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerMoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerMoveForward", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerMoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerMoveRight", EKeys::D, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerTurn", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerLookUp", EKeys::MouseY, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerLookUp", EKeys::MouseY, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PlayerJumpAxis", EKeys::E, -1.f));


		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(TEXT("PlayerAttack"), EKeys::LeftMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(TEXT("PlayerJumpAction"), EKeys::SpaceBar));
	}

	// 키와 함수를 연결합니다.
	// 이 키가 눌리면 이 함수를 실행시켜줘인데.
	// 축일때는 일단 계속 실행시켜줘.
	PlayerInputComponent->BindAxis("PlayerMoveForward", this, &AAIPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("PlayerMoveRight", this, &AAIPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("PlayerTurn", this, &AAIPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("PlayerTurnRate", this, &AAIPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("PlayerLookUp", this, &AAIPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("PlayerLookUpRate", this, &AAIPlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("PlayerAttack", EInputEvent::IE_Pressed, this, &AAIPlayerCharacter::AttackAction);
	PlayerInputComponent->BindAction("PlayerJumpAction", EInputEvent::IE_Pressed, this, &AAIPlayerCharacter::JumpAction);
}

void AAIPlayerCharacter::BeginPlay()
{
	SetAllAnimation(MapAnimation);

	Super::BeginPlay();

	GetGlobalAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &AAIPlayerCharacter::MontageEnd);

	GetGlobalAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AAIPlayerCharacter::AnimNotifyBegin);

	SetAniState(AIAniState::Idle);
}

void AAIPlayerCharacter::AttackAction()
{
	// 여기가 아니라 몽타주가 책임져줘야 한다.

	// 이걸 여기에서 하면 안된다.


	SetAniState(AIAniState::Attack);
}
void AAIPlayerCharacter::JumpAction()
{

	Jump();
	SetAniState(AIAniState::Jump);
}
void AAIPlayerCharacter::MoveRight(float Val)
{
	AIAniState AniStateValue = GetAniState<AIAniState>();

	if (AniStateValue == AIAniState::Attack || AniStateValue == AIAniState::Jump)
	{
		return;
	}

	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();
			// transform to world space and add it
			// 현재 내 회전을 가져와서 y축에 해당하는 축벡터를 얻어오는 겁니다.
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val);

			SetAniState(Val > 0.f ? AIAniState::RightMove : AIAniState::LeftMove);
			return;
		}
	}
	else
	{
		if (AniStateValue == AIAniState::RightMove || AniStateValue == AIAniState::LeftMove)
		{
			SetAniState(AIAniState::Idle);
		}
	}
}
void AAIPlayerCharacter::MoveForward(float Val)
{
	AIAniState AniStateValue = GetAniState<AIAniState>();

	if (AniStateValue == AIAniState::Attack || AniStateValue == AIAniState::Jump)
	{
		return;
	}

	if (Val != 0.f)
	{
		if (Controller)
		{
			// 컨트롤러는 기본적으로
			// charcter 하나씩 붙어 있습니다.
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// 이건 방향일 뿐입니다.
			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);
			// 탑뷰게임이면 다르게 나오게 되는데.
			// 지금은 TPS를 하고 있기 때문에 컨트롤러의 회전이나 액터의 회전이나 같아요.
			// AddMovementInput(GetActorForwardVector(), Val);

			SetAniState(Val > 0.f ? AIAniState::ForwardMove : AIAniState::BackwardMove);
			return;
		}
	}
	else
	{
		if (AniStateValue == AIAniState::ForwardMove || AniStateValue == AIAniState::BackwardMove)
		{
			SetAniState(AIAniState::Idle);
		}
	}
}
void AAIPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void AAIPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void AAIPlayerCharacter::MontageEnd(UAnimMontage* Anim, bool _Inter)
{
	TSubclassOf<UAnimInstance> Inst = AAIPlayerCharacter::StaticClass();

	// Anim 종료된 몽타주
	if (MapAnimation[AIAniState::Attack] == Anim)
	{
		SetAniState(AIAniState::Idle);
	}

	if (MapAnimation[AIAniState::Jump] == Anim)
	{
		SetAniState(AIAniState::Idle);
	}
}

void AAIPlayerCharacter::AnimNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	UGlobalGameInstance* Inst = GetWorld()->GetGameInstance<UGlobalGameInstance>();

	TSubclassOf<UObject> Effect = Inst->GetSubClass(TEXT("Effect"));

	TSubclassOf<UObject> RangeAttack = Inst->GetSubClass(TEXT("PlayerRangeAttack"));

	if (nullptr != Effect)
	{
		FTransform Trans;
		FVector Pos;
		TArray<UActorComponent*> MeshEffects = GetComponentsByTag(USceneComponent::StaticClass(), TEXT("WeaponEffect"));
		TArray<UActorComponent*> StaticMeshs = GetComponentsByTag(USceneComponent::StaticClass(), TEXT("WeaponMesh"));

		USceneComponent* EffectCom = Cast<USceneComponent>(MeshEffects[0]);
		Pos = EffectCom->GetComponentToWorld().GetLocation();

		// 이펙트 만들기
		{
			AActor* Actor = GetWorld()->SpawnActor<AActor>(Effect);
			// 본의 위치를 얻어오는 법.
			// 내가 얻어오고 싶은 정보가 있다. 누가 가지고 있을까?
			//USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			//Trans = SkeletalMeshComponent->GetSocketTransform(TEXT("B_R_Weapon"));
			//Pos = Trans.GetLocation();
			// UActorComponent* ActorComponent = GetComponentByClass(UStaticMeshComponent::StaticClass());
			// TArray<UActorComponent*> ActorComponents = GetComponentsByClass(UStaticMeshComponent::StaticClass());
			// UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponents[0]);
			// Pos = StaticMeshComponent->GetComponentToWorld().GetLocation();
			Actor->SetActorLocation(Pos);
			Actor->SetActorRotation(GetActorRotation());
			Actor->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
		}

		// 발사체 만들기
		{
			AActor* Actor = GetWorld()->SpawnActor<AActor>(RangeAttack);

			AProjectTile* ProjectTile = Cast<AProjectTile>(Actor);

			ProjectTile->SetActorLocation(Pos);
			ProjectTile->SetActorRotation(GetActorRotation());
			ProjectTile->GetSphereComponent()->SetCollisionProfileName(TEXT("PlayerAttack"), true);
		}
	}
}
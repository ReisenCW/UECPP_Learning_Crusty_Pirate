#include "Enemy.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	IsAlive = true;
	CanMove = true;
	SphereDetector = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetector"));
	SphereDetector->SetupAttachment(RootComponent);
	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(RootComponent);
	HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextHP"));
	HPText->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	SphereDetector->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnSphereOverlapBegin);
	SphereDetector->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnSphereOverlapEnd);
	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackBoxOverlapBegin);
	UpdateHP(HitPoints);
	OnAttackOverrideAnimEndDelegate.BindUObject(this, &AEnemy::OnAttackOverrideAnimEnd);
	EnableAttackBox(false);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && Target && !IsStunned) {
		float Direction = (Target->GetActorLocation().X - GetActorLocation().X) > 0.0f ? 1.0f : -1.0f;
		UpdateDirection(Direction);
		if (ShouldMoveToTartget()) {
			if (CanMove) {
				FVector WorldLocation = FVector(1.0f, 0.0f, 0.0f);
				AddMovementInput(WorldLocation, Direction);
			}
		}
		else {
			//Attack
			if (Target->IsAlive)
				Attack();
		}
	}
}

void AEnemy::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if(Player)
		Target = Player;
}

void AEnemy::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
		Target = nullptr;
}

void AEnemy::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor)) {
		PlayerCharacter->TakenDamage(AttackDmg, AttackStunDuration);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Hit"));
	}
}

void AEnemy::EnableAttackBox(bool Enabled)
{
	if (Enabled) {
		AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttackBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	else {
		AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttackBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

bool AEnemy::ShouldMoveToTartget()
{
	bool Result = false;
	if (Target) {
		float DistToTarget = abs(Target->GetActorLocation().X - GetActorLocation().X);
		Result = DistToTarget > StopDistance;
	}
	return Result;
}

void AEnemy::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = GetActorRotation();
	if (MoveDirection > 0.0f) {
		if (CurrentRotation.Yaw != 0.0) {
			SetActorRotation(FRotator(CurrentRotation.Pitch, 0.0, CurrentRotation.Roll));
		}
	}
	else if (MoveDirection < 0.0f) {
		if (CurrentRotation.Yaw != 180.0) {
			SetActorRotation(FRotator(CurrentRotation.Pitch, 180.0, CurrentRotation.Roll));
		}
	}
}

void AEnemy::UpdateHP(int NewHP)
{
	HitPoints = NewHP;
	FString Str = FString::Printf(TEXT("HP: %d"), HitPoints);
	HPText->SetText(FText::FromString(Str));
}

void AEnemy::TakenDamage(int Dmg, float StunDuration)
{
	if (!IsAlive) return;
	UpdateHP(HitPoints - Dmg);
	if (HitPoints <= 0.0f) {
		//Disable Enemy
		IsAlive = false;
		CanMove = false;
		CanAttack = false;
		UpdateHP(0);
		HPText->SetHiddenInGame(true);
		//Play Die Animation
		GetAnimInstance()->JumpToNode(FName("JumpDied"), FName("CrabStateMachine"));
		EnableAttackBox(false);
	}
	else {
		//Stun
		Stun(StunDuration);
		//Play Hit Animation
		GetAnimInstance()->JumpToNode(FName("JumpTakenHit"), FName("CrabStateMachine"));
	}
}

void AEnemy::OnStunTimerTimeout()
{
	IsStunned = false;
}

void AEnemy::Stun(float Duration)
{
	IsStunned = true;

	bool IsTimerActive = GetWorldTimerManager().IsTimerActive(StunTimer);
	if (IsTimerActive) {
		GetWorldTimerManager().ClearTimer(StunTimer);
	}

	GetWorldTimerManager().SetTimer(StunTimer, this, &AEnemy::OnStunTimerTimeout, 1.0f, false, Duration);
	GetAnimInstance()->StopAllAnimationOverrides();
	EnableAttackBox(false);

}

void AEnemy::Attack()
{
	if (IsAlive && CanAttack && !IsStunned) {
		CanAttack = false;
		CanMove = false;
		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, 
			FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideAnimEndDelegate);
		GetWorldTimerManager().SetTimer(AttackCoolDownTimer, this, &AEnemy::OnAttackTimerCoolDownTimeout, 1.0f, false, AttackCoolDownInSecs);
	}
}

void AEnemy::OnAttackTimerCoolDownTimeout()
{
	if (IsAlive) {
		CanAttack = true;
	}
}

void AEnemy::OnAttackOverrideAnimEnd(bool Completed)
{
	if (IsAlive) {
		CanMove = true;
	}
}

#include "PlayerCharacter.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(RootComponent);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController
		= Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	OnAttackOverrideAnimEndDelegate.BindUObject(this, &APlayerCharacter::OnAttackOverrideAnimEnd);
	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::AttackBoxOverlapBegin);
	EnableAttackBox(false);

	GameInstance = Cast<UCrustyPirateGameInstance>(GetGameInstance());
	if (GameInstance)
		HitPoints = GameInstance->PlayerHP;

	if (GameInstance->IsDoubleJumpUnlocked) {
		UnlockDoubleJump();
	}

	if (PlayerHUDClass) {
		PlayerHUD = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHUDClass);
		if (PlayerHUD) {
			PlayerHUD->AddToPlayerScreen();
			PlayerHUD->SetHP(HitPoints);
			PlayerHUD->SetDiamonds(GameInstance->CollectedDiamondsCount);
			PlayerHUD->SetLevel(GameInstance->CurrentLevelIndex);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APlayerCharacter::JumpEnded);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpEnded);
		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &APlayerCharacter::QuitGame);
	}
}

void APlayerCharacter::AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy && Enemy->IsAlive) {
		Enemy->TakenDamage(AttackDmg, AttackStunDuration);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Enemy Hit"));
	}
}

void APlayerCharacter::EnableAttackBox(bool Enabled)
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

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	float MoveActionValue = Value.Get<float>();
	if (IsAlive && CanMove && !IsStunned) {
		FVector Direction = FVector(1.f, 0.f, 0.f);
		AddMovementInput(Direction, MoveActionValue);
		UpdateDirection(MoveActionValue);
	}
}

void APlayerCharacter::JumpStarted(const FInputActionValue& Value)
{
	if (IsAlive && CanMove && !IsStunned) {
		Jump();
	}
}

void APlayerCharacter::JumpEnded(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (IsAlive && CanAttack && !IsStunned) {
		CanAttack = false;
		CanMove = false;
		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f, 0.0f, OnAttackOverrideAnimEndDelegate);
	}
}

void APlayerCharacter::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = Controller->GetControlRotation();
	if (MoveDirection > 0.0f) {
		if (CurrentRotation.Yaw != 0) {
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));
		}
	}
	else if (MoveDirection < 0.0f) {
		if (CurrentRotation.Yaw != 180) {
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));
		}
	}
}

void APlayerCharacter::OnAttackOverrideAnimEnd(bool Completed)
{
	if (IsAlive && IsActive) {
		CanAttack = true;
		CanMove = true;
	}
}

void APlayerCharacter::TakenDamage(int Dmg, float StunDuration)
{
	if (!IsAlive) return;
	if (!IsActive) return;
	UpdateHP(HitPoints - Dmg);
	if (HitPoints <= 0) {
		IsAlive = false;
		CanMove = false;
		CanAttack = false;
		GetAnimInstance()->JumpToNode(FName("JumpDied"), FName("CaptainStateMachine"));
		EnableAttackBox(false);

		float RestartDelay = 3.0f;
		GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerCharacter::OnRestartTimerTimeout, 1.0f, false, RestartDelay);
	}
	else {
		Stun(StunDuration);

		GetAnimInstance()->JumpToNode(FName("JumpTakenHit"), FName("CaptainStateMachine"));
	}
}

void APlayerCharacter::UpdateHP(int NewHP)
{
	if (NewHP < 0)
		HitPoints = 0;
	else {
		HitPoints = NewHP;
	}
	GameInstance->SetPlayerHP(HitPoints);
	PlayerHUD->SetHP(HitPoints);
}

void APlayerCharacter::Stun(float Duration)
{
	IsStunned = true;

	bool IsTimerActive = GetWorldTimerManager().IsTimerActive(StunTimer);
	if (IsTimerActive) {
		GetWorldTimerManager().ClearTimer(StunTimer);
	}
	GetWorldTimerManager().SetTimer(StunTimer, this, &APlayerCharacter::OnStunTimerTimeout, 1.0f, false, Duration);
	GetAnimInstance()->StopAllAnimationOverrides();
	EnableAttackBox(false);
}

void APlayerCharacter::OnStunTimerTimeout()
{
	IsStunned = false;
}

void APlayerCharacter::CollectItem(ECollectableType Type)
{
	UGameplayStatics::PlaySound2D(GetWorld(), ItemCollectedSound);
	switch (Type) {
		case ECollectableType::Diamond:
			GameInstance->AddDiamond(1);
			PlayerHUD->SetDiamonds(GameInstance->CollectedDiamondsCount);
			break;
		case ECollectableType::HealthPotion:
			UpdateHP(HitPoints + 25);
			break;
		case ECollectableType::DoubleJumpUpgrade:
			if (!GameInstance->IsDoubleJumpUnlocked) {
				GameInstance->IsDoubleJumpUnlocked = true;
				UnlockDoubleJump();
			}
			break;
	}
}

void APlayerCharacter::UnlockDoubleJump()
{
	JumpMaxCount = 2;
}

void APlayerCharacter::OnRestartTimerTimeout()
{
	GameInstance->RestartGame();
}

void APlayerCharacter::DeActivate()
{
	if (IsActive) {
		IsActive = false;
		CanAttack = false;
		CanMove = false;
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void APlayerCharacter::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}

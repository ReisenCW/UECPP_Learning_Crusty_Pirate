#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "Engine/TimerHandle.h"

#include "PaperZDAnimInstance.h"

#include "PlayerHUD.h"

#include "PlayerCharacter.generated.h"


UCLASS()
class CRUSTY_PIRATE_API APlayerCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* AttackBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanAttack = true;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackDmg = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackStunDuration = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HitPoints = 100;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPlayerHUD* PlayerHUD;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideAnimEndDelegate;

	FTimerHandle StunTimer;

public:
	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void EnableAttackBox(bool Enabled);
public:
	void Move(const FInputActionValue& Value);
	void JumpStarted(const FInputActionValue& Value);
	void JumpEnded(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void UpdateDirection(float MoveDirection);
	void OnAttackOverrideAnimEnd(bool Completed);
	void TakenDamage(int Dmg, float StunDuration);
	void UpdateHP(int NewHP);
	void Stun(float StunDuration);
	void OnStunTimerTimeout();
};

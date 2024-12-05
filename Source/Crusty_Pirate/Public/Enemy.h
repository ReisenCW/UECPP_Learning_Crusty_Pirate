#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "PlayerCharacter.h"
#include "PaperZDAnimInstance.h"
#include "Engine/Timerhandle.h"

#include "Enemy.generated.h"

UCLASS()
class CRUSTY_PIRATE_API AEnemy : public APaperZDCharacter
{
	GENERATED_BODY()
public:
	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void AttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackBox(bool Enabled);

public:
	bool ShouldMoveToTartget();
	void UpdateDirection(float MoveDirection);
	void UpdateHP(int NewHP);
	void TakenDamage(int Dmg, float StunDuration);
	void OnStunTimerTimeout();
	void Stun(float Duration);
	void Attack();
	void OnAttackTimerCoolDownTimeout();
	void OnAttackOverrideAnimEnd(bool Completed);
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USphereComponent* SphereDetector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* AttackBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APlayerCharacter* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTextRenderComponent* HPText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopDistance = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCoolDownInSecs = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HitPoints = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackDmg = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackStunDuration = 0.2f;

	FTimerHandle StunTimer;
	FTimerHandle AttackCoolDownTimer;
	FZDOnAnimationOverrideEndSignature OnAttackOverrideAnimEndDelegate;
};

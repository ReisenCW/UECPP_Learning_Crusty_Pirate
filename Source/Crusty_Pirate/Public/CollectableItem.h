#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

#include "CollectableItem.generated.h"

UENUM(BlueprintType)
enum class ECollectableType : uint8
{
	Diamond,
	HealthPotion,
	DoubleJumpUpgrade
};

UCLASS()
class CRUSTY_PIRATE_API ACollectableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ACollectableItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPaperFlipbookComponent* ItemFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECollectableType Type;
public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

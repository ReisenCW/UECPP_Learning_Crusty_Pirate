#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"


#include "CrustyPirateGameInstance.generated.h"


UCLASS()
class CRUSTY_PIRATE_API UCrustyPirateGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int PlayerHP = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CollectedDiamondsCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDoubleJumpUnlocked = false;

public:
	void SetPlayerHP(int NewHP);
	void AddDiamond(int Amount);
};

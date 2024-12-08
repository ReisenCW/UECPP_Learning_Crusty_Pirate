#include "CrustyPirateGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UCrustyPirateGameInstance::SetPlayerHP(int NewHP)
{
	PlayerHP = NewHP;
}

void UCrustyPirateGameInstance::AddDiamond(int Amount)
{
	CollectedDiamondsCount += Amount;
}

void UCrustyPirateGameInstance::ChangeLevel(int LevelIndex)
{
	if (LevelIndex <= 0) return;
	CurrentLevelIndex = LevelIndex;
	FString LevelName = FString::Printf(TEXT("Level_%d"), LevelIndex);
	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName));
}

void UCrustyPirateGameInstance::RestartGame()
{
	PlayerHP = 100;
	CollectedDiamondsCount = 0;
	IsDoubleJumpUnlocked = false;

	CurrentLevelIndex = 1;
	ChangeLevel(CurrentLevelIndex);
}

#include "CrustyPirateGameInstance.h"

void UCrustyPirateGameInstance::SetPlayerHP(int NewHP)
{
	PlayerHP = NewHP;
}

void UCrustyPirateGameInstance::AddDiamond(int Amount)
{
	CollectedDiamondsCount += Amount;
}

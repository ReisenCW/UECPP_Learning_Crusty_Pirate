#include "PlayerHUD.h"

void UPlayerHUD::SetHP(int NewHP)
{
	FString HPString = FString::Printf(TEXT("HP: %d"), NewHP);
	HPText->SetText(FText::FromString(HPString));
}

void UPlayerHUD::SetDiamonds(int Amount)
{
	FString DiamondsString = FString::Printf(TEXT("Diamonds: %d"), Amount);
	DiamondsText->SetText(FText::FromString(DiamondsString));
}

void UPlayerHUD::SetLevel(int Index)
{
	FString LevelString = FString::Printf(TEXT("Level: %d"), Index);
	LevelText->SetText(FText::FromString(LevelString));
}

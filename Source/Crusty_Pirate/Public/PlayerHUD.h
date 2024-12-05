#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

#include "PlayerHUD.generated.h"

UCLASS()
class CRUSTY_PIRATE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* HPText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock*DiamondsText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* LevelText;

public:
	void SetHP(int NewHP);
	void SetDiamonds(int Amount);
	void SetLevel(int Index);
};

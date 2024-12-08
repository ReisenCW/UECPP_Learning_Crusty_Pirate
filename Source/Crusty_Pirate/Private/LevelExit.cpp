#include "LevelExit.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CrustyPirateGameInstance.h"

ALevelExit::ALevelExit()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	ExitFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ExitFlipbook"));
	ExitFlipbook->SetupAttachment(RootComponent);
	ExitFlipbook->SetPlayRate(0.0f);
	ExitFlipbook->SetLooping(false);
}

void ALevelExit::BeginPlay()
{
	Super::BeginPlay();
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ALevelExit::OnBoxOverlapBegin);
	ExitFlipbook->SetPlaybackPosition(0.0f, false);
}

void ALevelExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelExit::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsAlive) {
		if (IsActive) {
			IsActive = false;

			PlayerCharacter->DeActivate();

			ExitFlipbook->SetPlayRate(1.0f);
			ExitFlipbook->PlayFromStart();
			UGameplayStatics::PlaySound2D(GetWorld(), PlayerEnterSoound);

			GetWorldTimerManager().SetTimer(WaitTimer, this, &ALevelExit::OnWaitTimerTimeout, 1.0f, false, WaitTimeInSeconds);
		}
		
	}
}

void ALevelExit::OnWaitTimerTimeout()
{
	UCrustyPirateGameInstance* GameInstance = Cast<UCrustyPirateGameInstance>(GetGameInstance());
	if (GameInstance) {
		GameInstance->ChangeLevel(LevelIndex);
	}
}


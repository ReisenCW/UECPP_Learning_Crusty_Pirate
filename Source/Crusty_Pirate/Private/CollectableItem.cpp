#include "CollectableItem.h"
#include "PlayerCharacter.h"

ACollectableItem::ACollectableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	ItemFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ItemFlipbook"));
	ItemFlipbook->SetupAttachment(CapsuleComp);
}

void ACollectableItem::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ACollectableItem::OnOverlapBegin);
}

void ACollectableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollectableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsAlive) {
		PlayerCharacter->CollectItem(Type);
		Destroy();
	}
}


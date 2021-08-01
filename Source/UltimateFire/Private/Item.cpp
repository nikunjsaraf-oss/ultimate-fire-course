// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem()
	: ItemName(FString("Default")),
	  ItemCount(30),
	  ItemRarity(EItemRarity::EIR_Common),
	  ItemState(EItemState::EIS_Pickup),
	  ItemIterpStartLocation(FVector(0)),
	  CameraTargetLocation(FVector(0)),
	  bIsInterping(false),
	  ZCurveTime(0.7f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		// Hide pickup widget.
		PickupWidget->SetVisibility(false);
	}

	SetActiveStars();

	// Setup overlap for area sphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	SetItemState(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	// Not to use 0th element
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_UnCommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	case EItemRarity::EIR_MAX:
		ActiveStars[1] = false;
		ActiveStars[2] = false;
		ActiveStars[3] = false;
		ActiveStars[4] = false;
		ActiveStars[5] = false;
		break;
	default: break;
	}
}

void AItem::SetItemProperties(const EItemState State) const
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;

	case EItemState::EIS_Equipped:

		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	}
}

void AItem::FinishInterping()
{
	if (Character)
	{
		Character->GetPickupItem(this);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::SetItemState(const EItemState State)
{
	ItemState = State;
	SetItemProperties(ItemState);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	Character = Char;

	ItemIterpStartLocation = GetActorLocation(); // Store initial location of item

	bIsInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(TimerHandle_ItemInterp, this, &AItem::FinishInterping, ZCurveTime);
}

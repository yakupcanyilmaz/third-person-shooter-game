// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());	

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	InitializeItem();
}

void AItem::InitializeItem()
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);	

	SetItemProperties(ItemState);
	DisableCustomDepth();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			if (ItemState != EItemState::EIS_Equipped)
			{
				SetOwner(ShooterCharacter);
				ShooterCharacter->IncrementOverlappedItemCount(1);
				ShooterCharacter->SetOverlappedItem(this);
				EnableCustomDepth();
				SetPickupWidgetVisibility(true);
			}
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			if (ItemState != EItemState::EIS_Equipped)
			{
				ShooterCharacter->IncrementOverlappedItemCount(-1);
				ShooterCharacter->SetOverlappedItem(nullptr);
				DisableCustomDepth();
				SetPickupWidgetVisibility(false);
			}
		}
	}
}

void AItem::OnConstruction(const FTransform& Transform)
{
	SetGlowBlendAlphaValue(0);
}

void AItem::EnableCustomDepth()
{
	ServerEnableItemMeshCustomDepth();
}

void AItem::ServerEnableItemMeshCustomDepth_Implementation()
{
	ClientEnableItemMeshCustomDepth();
}

void AItem::ClientEnableItemMeshCustomDepth_Implementation()
{
	if (bCanChangeCustomDepth) ItemMesh->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth) ItemMesh->SetRenderCustomDepth(false);
}

void AItem::EnableGlowMaterial()
{
	//ServerEnableGlowMaterial();
	SetGlowBlendAlphaValue(0);
}

void AItem::DisableGlowMaterial()
{
	SetGlowBlendAlphaValue(1);
}

void AItem::SetGlowBlendAlphaValue(int8 Value)
{
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
	}
	if (DynamicMaterialInstance) DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), Value);
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EnableGlowMaterial();
		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DisableCustomDepth();
		DisableGlowMaterial();
		break;
	case EItemState::EIS_Falling:
		// Set mesh properties
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetItemState(EItemState State)
{
	ServerSetItemState(State);
}

void AItem::ServerSetItemState_Implementation(EItemState State)
{
	MulticastSetItemState(State);
}

bool AItem::ServerSetItemState_Validate(EItemState State)
{
	return true;
}

void AItem::MulticastSetItemState_Implementation(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::SetPickupWidgetVisibility(bool Bool)
{
	ServerSetPickupWidgetVisibility(Bool);
}

void AItem::ServerSetPickupWidgetVisibility_Implementation(bool Bool)
{
	ClientSetPickupWidgetVisibility(Bool);
}

void AItem::ClientSetPickupWidgetVisibility_Implementation(bool Bool)
{
	PickupWidget->SetVisibility(Bool);
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, ItemState);
	DOREPLIFETIME(AItem, ItemMesh);
	DOREPLIFETIME(AItem, DynamicMaterialInstance);
	DOREPLIFETIME(AItem, MaterialInstance);
	DOREPLIFETIME(AItem, PickupWidget);
}

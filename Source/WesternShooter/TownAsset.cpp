// Fill out your copyright notice in the Description page of Project Settings.


#include "TownAsset.h"
#include "Target.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATownAsset::ATownAsset() :
	TargetCount(8)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnLocation1 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation1"));
	SpawnLocation1->SetupAttachment(RootComponent);
	SpawnLocation2 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation2"));
	SpawnLocation2->SetupAttachment(RootComponent);
	SpawnLocation3 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation3"));
	SpawnLocation3->SetupAttachment(RootComponent);
	SpawnLocation4 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation4"));
	SpawnLocation4->SetupAttachment(RootComponent);
	SpawnLocation5 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation5"));
	SpawnLocation5->SetupAttachment(RootComponent);
	SpawnLocation6 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation6"));
	SpawnLocation6->SetupAttachment(RootComponent);
	SpawnLocation7 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation7"));
	SpawnLocation7->SetupAttachment(RootComponent);
	SpawnLocation8 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation8"));
	SpawnLocation8->SetupAttachment(RootComponent);
	SpawnLocation9 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation9"));
	SpawnLocation9->SetupAttachment(RootComponent);
	SpawnLocation10 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation10"));
	SpawnLocation10->SetupAttachment(RootComponent);

	SpawnLocationArray.Add(SpawnLocation1);
	SpawnLocationArray.Add(SpawnLocation2);
	SpawnLocationArray.Add(SpawnLocation3);
	SpawnLocationArray.Add(SpawnLocation4);
	SpawnLocationArray.Add(SpawnLocation5);
	SpawnLocationArray.Add(SpawnLocation6);
	SpawnLocationArray.Add(SpawnLocation7);
	SpawnLocationArray.Add(SpawnLocation8);
	SpawnLocationArray.Add(SpawnLocation9);
	SpawnLocationArray.Add(SpawnLocation10);

	ShuffleArray();
}

// Called when the game starts or when spawned
void ATownAsset::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SpawnTargets();
	}
}

//void ATownAsset::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(ATownAsset, TargetClass);
//}

// Called every frame
void ATownAsset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATownAsset::ShuffleArray()
{
	int32 m = SpawnLocationArray.Num();
	USceneComponent* t;
	int32 i;

	while (m > 0)
	{
		i = FGenericPlatformMath::FloorToInt(FMath::RandRange(0.f, 0.9f) * m);
		m--;
		t = SpawnLocationArray[m];
		SpawnLocationArray[m] = SpawnLocationArray[i];
		SpawnLocationArray[i] = t;
	}
}

void ATownAsset::SpawnTargets()
{
	for (int32 i = 0; i < TargetCount; i++)
	{
		GetWorld()->SpawnActor<ATarget>(TargetClass, SpawnLocationArray[i]->GetComponentLocation(), SpawnLocationArray[i]->GetComponentRotation());
	}
}


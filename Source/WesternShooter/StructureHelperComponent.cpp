// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureHelperComponent.h"

// Sets default values for this component's properties
UStructureHelperComponent::UStructureHelperComponent() :
	RandomNaturePlacement{ true },
	RandomNaturePlacementThreshold{ 0.3f }
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStructureHelperComponent::BeginPlay()
{
	Super::BeginPlay();


		if (BuildingTypes[0].Prefabs.Num() > 0)
		{
			BuildingTypes[0].Shuffle();
		}

	// ...
	
}


// Called every frame
void UStructureHelperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStructureHelperComponent::PlaceStructuresAroundRoad(TArray<FIntVector> RoadPositions, int32 UnitLength)
{
	TMap<FIntVector, EDirection> FreeEstateSpots = FindFreeSpacesAroundRoad(RoadPositions, UnitLength);

	TArray<FIntVector> BlockedPositions = TArray<FIntVector>();
	for(auto FreeSpot : FreeEstateSpots)
	{
		if (BlockedPositions.Contains(FreeSpot.Key))
		{
			continue;
		}

		FRotator Rotation = PlacementHelper::AlignRotation(FreeSpot.Value);

		for (int32 i = 0; i < BuildingTypes.Num(); i++)
		{
			if (BuildingTypes[i].Quantity == -1)
			{
				if (RandomNaturePlacement)
				{
					auto Random = FMath::RandRange(0.f, 1.f);
					if (Random < RandomNaturePlacementThreshold)
					{

						int32 RandomIndex = FMath::RandRange(0, (NaturePrefabs.Num() - 1));
						auto NaturePrefab = NaturePrefabs[RandomIndex].GetDefaultObject();
						if (NaturePrefab != NaturePrefabs[(NaturePrefabs.Num() - 1)].GetDefaultObject())
						{
							auto RandomRotationYaw = FMath::RandRange(-180.f, 180.f);
							FRotator RandomRotation = FRotator(0.f, RandomRotationYaw, 0.f);
							auto Nature = SpawnPrefab(NaturePrefab, FreeSpot.Key, RandomRotation);
							NatureDictionary.Add(FreeSpot.Key, Nature);
						}
						else
						{
							auto Nature = SpawnPrefab(NaturePrefab, FreeSpot.Key, Rotation);
							NatureDictionary.Add(FreeSpot.Key, Nature);
						}
						break;
					}
				}

				auto Building = SpawnPrefab(BuildingTypes[i].GetRandomPrefab(), FreeSpot.Key, Rotation);
				StructuresDictionary.Add(FreeSpot.Key, Building);
				break;
			}
			if (BuildingTypes[i].IsBuildingAvailable())
			{
				if (BuildingTypes[i].SizeRequired > 1)
				{
					auto HalfSize = FGenericPlatformMath::FloorToInt(BuildingTypes[i].SizeRequired / 2.0f);
					TArray<FIntVector> TempPositionsBlocked = TArray<FIntVector>();
					if (VerifyIfBuildingFits(HalfSize, FreeEstateSpots, FreeSpot, BlockedPositions, TempPositionsBlocked, UnitLength))
					{
						for (int32 j = 0; j < TempPositionsBlocked.Num(); j++)
						{
							BlockedPositions.Add(TempPositionsBlocked[j]);
						}
						auto Building = SpawnPrefab(BuildingTypes[i].GetPrefab(), FreeSpot.Key, Rotation);
						StructuresDictionary.Add(FreeSpot.Key, Building);
						for (auto Pos : TempPositionsBlocked)
						{
							StructuresDictionary.Add(Pos, Building);
						}
						break;
					}
				}
				else
				{
					auto Building = SpawnPrefab(BuildingTypes[i].GetPrefab(), FreeSpot.Key, Rotation);
					StructuresDictionary.Add(FreeSpot.Key, Building);
				}
				break;
			}
		}
	}
}

bool UStructureHelperComponent::VerifyIfBuildingFits(int32 HalfSize, TMap<FIntVector, EDirection> FreeEstateSpots, TPair<FIntVector, EDirection> FreeSpot, TArray<FIntVector> BlockedPositions, TArray<FIntVector>& TempPositionsBlocked, int32 UnitLength)
{
	FIntVector Direction = FIntVector(0);
	if (FreeSpot.Value == EDirection::ED_Down || FreeSpot.Value == EDirection::ED_Up)
	{
		Direction = FIntVector(0, 1, 0);
	}
	else
	{
		Direction = FIntVector(1, 0, 0);
	}
	for (int i = 1; i <= HalfSize; i++)
	{
		FIntVector Pos1 = FreeSpot.Key + Direction * i * UnitLength;
		FIntVector Pos2 = FreeSpot.Key - Direction * i * UnitLength;

		if (!FreeEstateSpots.Contains(Pos1) || !FreeEstateSpots.Contains(Pos2) || BlockedPositions.Contains(Pos1) || BlockedPositions.Contains(Pos2))
		{
			return false;
		}
		TempPositionsBlocked.Add(Pos1);
		TempPositionsBlocked.Add(Pos2);
	}
	return true;
}

AActor* UStructureHelperComponent::SpawnPrefab(AActor* Prefab, FIntVector Position, FRotator& Rotation)
{
	return GetWorld()->SpawnActor<AActor>(Prefab->GetClass(), FVector(Position), Rotation);
}

TMap<FIntVector, EDirection> UStructureHelperComponent::FindFreeSpacesAroundRoad(TArray<FIntVector> RoadPositions, int32 UnitLength)
{
	TMap<FIntVector, EDirection> FreeSpaces = TMap<FIntVector, EDirection>();

	for(auto Position : RoadPositions)
	{
		auto NeighbourDirections = PlacementHelper::FindNeighbour(Position, RoadPositions, UnitLength);

		for(EDirection Direction : TEnumRange<EDirection>())
		{
			if (NeighbourDirections.Contains(Direction) == false)
			{
				auto NewPosition = Position + PlacementHelper::GetOffsetFromDirection(Direction, UnitLength);
				if (FreeSpaces.Contains(NewPosition))
				{
					continue;
				}
				FreeSpaces.Add(NewPosition, PlacementHelper::GetReverseDirection(Direction));
			}
		}
	}
	return FreeSpaces;
}

void UStructureHelperComponent::DestroyObjects()
{
	TArray<FIntVector> StructuresKeyArray;
	StructuresDictionary.GenerateKeyArray(StructuresKeyArray);
	for (auto Item : StructuresKeyArray)
	{
		AActor* Prefab = StructuresDictionary.FindChecked(Item);
		Prefab->GetOwner();
		if (Prefab)
		{
			Prefab->Destroy();
		}
	}

	TArray<FIntVector> NatureKeyArray;
	NatureDictionary.GenerateKeyArray(NatureKeyArray);
	for (auto Item : NatureKeyArray)
	{
		AActor* Prefab = NatureDictionary.FindChecked(Item);
		Prefab->GetOwner();
		if (Prefab)
		{
			Prefab->Destroy();
		}
	}
}


void UStructureHelperComponent::Reset()
{
	StructuresDictionary.Empty();
	NatureDictionary.Empty();
	for (int32 i = 0; i < BuildingTypes.Num(); i++)
	{
		BuildingTypes[i].Reset();
	}
}
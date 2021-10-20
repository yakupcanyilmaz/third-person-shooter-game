// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadHelperComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "PlacementHelper.h"

// Sets default values for this component's properties
URoadHelperComponent::URoadHelperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URoadHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URoadHelperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URoadHelperComponent::PlaceStreetPositions(FVector StartPosition, FIntVector Direction, int32 Length, int32 UnitLength)
{
	FRotator Rotation = FRotator(0.f);
	if (Direction.X == 0)
	{
		Rotation = FRotator(0.f, 90.f, 0.f);
	}

	for (int32 i = 0; i < Length; i += UnitLength)
	{
		FVector Position = StartPosition + FVector(Direction) * i;
		Position.X = FGenericPlatformMath::RoundToInt(Position.X);
		Position.Y = FGenericPlatformMath::RoundToInt(Position.Y);
		Position.Z = FGenericPlatformMath::RoundToInt(Position.Z);
		if (RoadDictionary.Contains(FIntVector(Position)))
		{
			continue;
		}
		AActor* Road = GetWorld()->SpawnActor<AActor>(RoadStraight, Position, Rotation);
		RoadDictionary.Add(FIntVector(Position), Road);
		if (i == 0 || i == Length - UnitLength)
		{
			FixRoadCandidates.Add(FIntVector(Position));
		}
	}
}

void URoadHelperComponent::FixRoad(int32 UnitLength)
{
	for (FIntVector Position : FixRoadCandidates)
	{
		TArray<FIntVector> KeyArray;
		RoadDictionary.GenerateKeyArray(KeyArray);

		TArray<EDirection> NeighbourDirections = PlacementHelper::FindNeighbour(Position, KeyArray, UnitLength);

		FRotator Rotation = FRotator(0.f);

		if (NeighbourDirections.Num() == 1)
		{
			RoadDictionary[Position]->Destroy();
			if (NeighbourDirections.Contains(EDirection::ED_Down))
			{
				Rotation = FRotator(0.f, 0.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Left))
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Up))
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Right))
			{
				Rotation = FRotator(0.f, -90.f, 0.f);
			}
			RoadDictionary[Position] = GetWorld()->SpawnActor<AActor>(RoadEnd, FVector(Position), Rotation);
		}
		else if (NeighbourDirections.Num() == 2)
		{
			if (
				NeighbourDirections.Contains(EDirection::ED_Up) && NeighbourDirections.Contains(EDirection::ED_Down)
				|| NeighbourDirections.Contains(EDirection::ED_Right) && NeighbourDirections.Contains(EDirection::ED_Left)
				)
			{
				continue;
			}
			RoadDictionary[Position]->Destroy();
			if (NeighbourDirections.Contains(EDirection::ED_Up) && NeighbourDirections.Contains(EDirection::ED_Right))
			{
				Rotation = FRotator(0.f, 0.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Right) && NeighbourDirections.Contains(EDirection::ED_Down))
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Down) && NeighbourDirections.Contains(EDirection::ED_Left))
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Up) && NeighbourDirections.Contains(EDirection::ED_Left))
			{
				Rotation = FRotator(0.f, -90.f, 0.f);
			}
			RoadDictionary[Position] = GetWorld()->SpawnActor<AActor>(RoadCorner, FVector(Position), Rotation);
		}
		else if (NeighbourDirections.Num() == 3)
		{
			RoadDictionary[Position]->Destroy();
			if (NeighbourDirections.Contains(EDirection::ED_Right)
				&& NeighbourDirections.Contains(EDirection::ED_Down)
				&& NeighbourDirections.Contains(EDirection::ED_Left)
				)
			{
				Rotation = FRotator(0.f, 0.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Down)
				&& NeighbourDirections.Contains(EDirection::ED_Left)
				&& NeighbourDirections.Contains(EDirection::ED_Up))
			{
				Rotation = FRotator(0.f, 90.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Left)
				&& NeighbourDirections.Contains(EDirection::ED_Up)
				&& NeighbourDirections.Contains(EDirection::ED_Right))
			{
				Rotation = FRotator(0.f, 180.f, 0.f);
			}
			else if (NeighbourDirections.Contains(EDirection::ED_Right)
				&& NeighbourDirections.Contains(EDirection::ED_Up)
				&& NeighbourDirections.Contains(EDirection::ED_Down))
			{
				Rotation = FRotator(0.f, -90.f, 0.f);
			}
			RoadDictionary[Position] = GetWorld()->SpawnActor<AActor>(Road3Way, FVector(Position), Rotation);
		}
		else
		{
			RoadDictionary[Position]->Destroy();
			RoadDictionary[Position] = GetWorld()->SpawnActor<AActor>(Road4Way, FVector(Position), Rotation);
		}
	}
}

TArray<FIntVector> URoadHelperComponent::GetRoadPositions()
{
	TArray<FIntVector> KeyArray;
	RoadDictionary.GenerateKeyArray(KeyArray);
	return KeyArray;
}

void URoadHelperComponent::DestroyObjects()
{
	TArray<FIntVector> RoadKeyArray;
	RoadDictionary.GenerateKeyArray(RoadKeyArray);
	for (auto Item : RoadKeyArray)
	{
		AActor* Prefab = RoadDictionary.FindChecked(Item);
		Prefab->GetOwner();
		if (Prefab)
		{
			Prefab->Destroy();
		}
	}
}

void URoadHelperComponent::Reset()
{
	RoadDictionary.Empty();
	FixRoadCandidates.Empty();
}
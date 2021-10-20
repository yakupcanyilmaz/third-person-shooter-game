// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlacementHelper.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructureHelperComponent.generated.h"

USTRUCT(BlueprintType)
struct FBuildingType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class AActor>> Prefabs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SizeRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 QuantityAlreadyPlaced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;

	AActor* GetRandomPrefab()
	{
		QuantityAlreadyPlaced++;
		if (Prefabs.Num() > 1)
		{
			int32 Random = FMath::RandRange(0, (Prefabs.Num() - 1));
			auto Prefab = Prefabs[Random].GetDefaultObject();
			return Prefab;
		}
		auto Prefab = Prefabs[0].GetDefaultObject();
		return Prefab;
	}

	AActor* GetPrefab()
	{
		QuantityAlreadyPlaced++;
		if (Prefabs.Num() > 1)
		{
			auto Prefab = Prefabs[Index].GetDefaultObject();
			Index++;
			return Prefab;
		}
		auto Prefab = Prefabs[0].GetDefaultObject();
		return Prefab;
	}

	bool IsBuildingAvailable()
	{
		return QuantityAlreadyPlaced < Quantity;
	}

	void Reset()
	{
		QuantityAlreadyPlaced = 0;
	}

	void Shuffle()
	{
		int32 m = Prefabs.Num();
		TSubclassOf<class AActor> t;
		int32 i;

		while (m > 0)
		{
			i = FGenericPlatformMath::FloorToInt(FMath::RandRange(0.f, 0.9f) * m);
			m--;
			t = Prefabs[m];
			Prefabs[m] = Prefabs[i];
			Prefabs[i] = t;
		}
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTERNSHOOTER_API UStructureHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStructureHelperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Test, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AActor> Object;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buildings, meta = (AllowPrivateAccess = "true"))
	TArray<FBuildingType> BuildingTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Nature, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AActor>> NaturePrefabs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Nature, meta = (AllowPrivateAccess = "true"))
	bool RandomNaturePlacement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Nature, meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float RandomNaturePlacementThreshold;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Dimensions, meta = (AllowPrivateAccess = "true"))
	//	int32 UnitLength;

	AActor* SpawnPrefab(AActor* Prefab, FIntVector Position, FRotator& Rotation);

	TMap<FIntVector, EDirection> FindFreeSpacesAroundRoad(TArray<FIntVector> RoadPositions, int32 UnitLength);

	TMap<FIntVector, AActor*> StructuresDictionary;

	TMap<FIntVector, AActor*> NatureDictionary;

	bool VerifyIfBuildingFits(
		int32 HalfSize,
		TMap<FIntVector, EDirection> FreeEstateSpots,
		TPair<FIntVector, EDirection> FreeSpot,
		TArray<FIntVector> BlockedPositions,
		TArray<FIntVector>& TempPositionsBlocked, 
		int32 UnitLength);

public:


	void PlaceStructuresAroundRoad(TArray<FIntVector> RoadPositions, int32 UnitLength);

	void DestroyObjects();

	void Reset();
};

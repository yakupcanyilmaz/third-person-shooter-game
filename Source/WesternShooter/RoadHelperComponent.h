// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoadHelperComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTERNSHOOTER_API URoadHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URoadHelperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AActor> RoadStraight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> RoadCorner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> Road3Way;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> Road4Way;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> RoadEnd;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Roads, meta = (AllowPrivateAccess = "true"))
	//	int32 UnitLength;

public:

	void PlaceStreetPositions(FVector StartPosition, FIntVector Direction, int32 Length, int32 UnitLength);
	void FixRoad(int32 UnitLength);
	TArray<FIntVector> GetRoadPositions();
	void Reset();
	void DestroyObjects();

	TMap<FIntVector, AActor*> RoadDictionary;

	TSet<FIntVector> FixRoadCandidates;
};

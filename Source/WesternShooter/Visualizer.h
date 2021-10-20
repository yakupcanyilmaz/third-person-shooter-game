// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Visualizer.generated.h"

struct FAgentParameters
{
	FAgentParameters(FVector NewPosition, FVector NewDirection, int32 NewLength) {
		Position = NewPosition;
		Direction = NewDirection;
		Length = NewLength;
	}
	FVector Position;
	FVector Direction;
	int32 Length;
};

enum class EEncodingLetters
{
	EEL_Unknown = '1',
	EEL_Save = '[',
	EEL_Load = ']',
	EEL_Draw = 'F',
	EEL_TurnRight = '+',
	EEL_TurnLeft = '-'
};

UCLASS()
class WESTERNSHOOTER_API AVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LSystemGenerator, meta = (AllowPrivateAccess = "true"))
	class ULSystemGeneratorComponent* LSystemGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LSystemGenerator, meta = (AllowPrivateAccess = "true"))
	class URoadHelperComponent* RoadHelper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LSystemGenerator, meta = (AllowPrivateAccess = "true"))
	class UStructureHelperComponent* StructureHelper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = LSystemGenerator, meta = (AllowPrivateAccess = "true"))
	int32 Length;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = LSystemGenerator, meta = (AllowPrivateAccess = "true"))
		int32 UnitLength;

	float Angle;
	TArray<FVector> Positions;
	FString SequenceToGenerate;

	void VisualizeSequence(FString& Sequence);

	void Reset();

	UFUNCTION(BlueprintCallable)
	void CreateTown();

	UFUNCTION(BlueprintCallable)
	void ClearTown();


public:
	FORCEINLINE int32 GetLength()
	{
		if (Length > UnitLength * 2)
		{
			return Length;
		}
		else
		{
			return UnitLength * 2;
		}
	}

	FORCEINLINE void SetLength(int32 Value) { Length = Value; }

};

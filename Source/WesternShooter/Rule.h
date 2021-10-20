// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rule.generated.h"

UCLASS()
class WESTERNSHOOTER_API ARule : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARule();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rule, meta = (AllowPrivateAccess = "true"))
	FString Letter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rule, meta = (AllowPrivateAccess = "true"))
	TArray<FString> Results;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rule, meta = (AllowPrivateAccess = "true"))
	bool RandomResult = false;

public:
	FORCEINLINE FString GetLetter() const { return Letter; }
	FORCEINLINE FString GetResult() 
	{ 
		if (RandomResult)
		{
			int RandomIndex = FMath::RandRange(0, Results.Num()-1);
			return Results[RandomIndex];
		}
	return Results[0]; 
	}

};

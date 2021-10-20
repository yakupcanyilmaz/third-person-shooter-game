// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LSystemGenerator.generated.h"

UCLASS()
class WESTERNSHOOTER_API ALSystemGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSystemGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class ARule>> Rules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
	FString RootSentence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (ClampMin = "0.0", ClampMax = "10.0", AllowPrivateAccess = "true"))
	int32 IterationLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
	bool RandomIgnoreRuleModifier = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float ChanceToIgnoreRule = 0.3f;

public:

	FString GenerateSentence(FString Word = FString());
	FString GrowRecursive(FString Word, int32 IterationIndex = 0);
	void ProcessRulesRecursivelly(FString& NewWord, TCHAR& c, int32& IterationIndex);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSystemGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WESTERNSHOOTER_API ULSystemGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULSystemGeneratorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<class ARule>> Rules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
		FString RootSentence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (ClampMin = "0.0", ClampMax = "10.0", AllowPrivateAccess = "true"))
		int32 IterationLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (AllowPrivateAccess = "true"))
		bool RandomIgnoreRuleModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rules, meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
		float ChanceToIgnoreRule;

public:

	FString GenerateSentence(FString Word = FString());
	FString GrowRecursive(FString Word, int32 IterationIndex = 0);
	void ProcessRulesRecursivelly(FString& NewWord, TCHAR& c, int32& IterationIndex);
};

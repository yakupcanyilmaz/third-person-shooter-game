// Fill out your copyright notice in the Description page of Project Settings.


#include "LSystemGeneratorComponent.h"
#include "Rule.h"

// Sets default values for this component's properties
ULSystemGeneratorComponent::ULSystemGeneratorComponent() :
	RandomIgnoreRuleModifier{ true },
	ChanceToIgnoreRule{ 0.3f }
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULSystemGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULSystemGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString ULSystemGeneratorComponent::GenerateSentence(FString Word)
{
	if (Word.IsEmpty())
	{
		Word = RootSentence;
	}
	return GrowRecursive(Word);
}

FString ULSystemGeneratorComponent::GrowRecursive(FString Word, int32 IterationIndex)
{
	//if (IterationIndex == 0)
	//{
	//	return Word;
	//}
	//FString Result;
	//for (int i = 0; i < Word.Len(); i++)
	//{
	//	if (Word[i] == 'A')
	//	{
	//		Result.Append("AB");
	//	}
	//	else if (Word[i] == 'B')
	//	{
	//		Result.Append("A");
	//	}
	//}
	//return Word + " " + GrowRecursive(Result, IterationIndex - 1);

	if (IterationIndex >= IterationLimit)
	{
		return Word;
	}
	FString NewWord;
	for (int i = 0; i < Word.Len(); i++)
	{
		NewWord.AppendChar(Word[i]);
		ProcessRulesRecursivelly(NewWord, Word[i], IterationIndex);
	}

	return NewWord;
}

void ULSystemGeneratorComponent::ProcessRulesRecursivelly(FString& NewWord, TCHAR& c, int32& IterationIndex)
{
	FString Result;
	for (int32 i = 0; i < Rules.Num(); i++)
	{
		auto Rule = Rules[i].GetDefaultObject();
		if (Rule)
		{
			if (Rule->GetLetter() == FString::Chr(c))
			{
				if (RandomIgnoreRuleModifier && IterationIndex > 1)
				{
					if (FMath::Rand() < ChanceToIgnoreRule)
					{
						return;
					}
				}
				Result = Rule->GetResult();
				NewWord.Append(GrowRecursive(Result, IterationIndex + 1));
			}
		}
	}

	//FString Result;
	//if (c == 'A')
	//{
	//	Result = "AB";
	//}
	//NewWord.Append(GrowRecursive(Result, IterationIndex + 1));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LSystemGenerator.h"
#include "Rule.h"

// Sets default values
ALSystemGenerator::ALSystemGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALSystemGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	//FString Sentence = GenerateSentence(RootSentence);
	//UE_LOG(LogTemp, Warning, TEXT("Sentence: %s"), *Sentence);
	//UE_LOG(LogTemp, Warning, TEXT("Iteration: %d"), IterationLimit);
}

// Called every frame
void ALSystemGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString ALSystemGenerator::GenerateSentence(FString Word)
{
	if (Word.IsEmpty())
	{
		Word = RootSentence;
	}
	return GrowRecursive(Word);
}

FString ALSystemGenerator::GrowRecursive(FString Word, int32 IterationIndex)
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

void ALSystemGenerator::ProcessRulesRecursivelly(FString& NewWord, TCHAR& c, int32& IterationIndex)
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



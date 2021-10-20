// Fill out your copyright notice in the Description page of Project Settings.


#include "Visualizer.h"
#include "DrawDebugHelpers.h"
//#include "LSystemGenerator.h"
#include "LSystemGeneratorComponent.h"
#include "RoadHelperComponent.h"
#include "StructureHelperComponent.h"

// Sets default values
AVisualizer::AVisualizer() :
    Length{ 1000 },
    UnitLength{ 200 },
    Angle{90.f}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    LSystemGenerator = CreateDefaultSubobject<ULSystemGeneratorComponent>(TEXT("LSystemGeneratorComponent"));
    RoadHelper = CreateDefaultSubobject<URoadHelperComponent>(TEXT("RoadHelperComponent"));
    StructureHelper = CreateDefaultSubobject<UStructureHelperComponent>(TEXT("StructureHelperComponent"));
}

// Called when the game starts or when spawned
void AVisualizer::BeginPlay()
{
	Super::BeginPlay(); 
    if (HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
        CreateTown();
    }
}

// Called every frame
void AVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVisualizer::VisualizeSequence(FString& Sequence)
{
    TArray<FAgentParameters> SavePoints = TArray<FAgentParameters>();
    FVector CurrentPosition = FVector(0);

    FVector Direction = AActor::GetActorForwardVector();
    FVector TempPosition = FVector(0);

    Positions.Add(CurrentPosition);

    for (auto Letter : Sequence)
    {
        EEncodingLetters Encoding = (EEncodingLetters)Letter;
        switch (Encoding)
        {
        case EEncodingLetters::EEL_Save:
            SavePoints.Push(FAgentParameters(CurrentPosition, Direction, GetLength()));
            break;
        case EEncodingLetters::EEL_Load:
            if (SavePoints.Num() > 0)
            {
                auto AgentParameter = SavePoints.Pop();
                CurrentPosition = AgentParameter.Position;
                Direction = AgentParameter.Direction;
                SetLength(AgentParameter.Length);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Dont have saved point in our stack"));
            }
            break;
        case EEncodingLetters::EEL_Draw:
            TempPosition = CurrentPosition;
            CurrentPosition += Direction * GetLength();

            Direction.X = FGenericPlatformMath::RoundToInt(Direction.X);
            Direction.Y = FGenericPlatformMath::RoundToInt(Direction.Y);
            Direction.Z = FGenericPlatformMath::RoundToInt(Direction.Z);
    
            RoadHelper->PlaceStreetPositions(TempPosition, FIntVector(Direction), GetLength(), UnitLength);
            
            DrawDebugLine(GetWorld(), TempPosition, CurrentPosition, FColor::Red, true);
            SetLength(GetLength() - UnitLength);
            Positions.Add(CurrentPosition);
            break;
        case EEncodingLetters::EEL_TurnRight:
            Direction = Direction.RotateAngleAxis(Angle, AActor::GetActorUpVector());
            break;
        case EEncodingLetters::EEL_TurnLeft:
            Direction = Direction.RotateAngleAxis(-Angle, AActor::GetActorUpVector());
            break;
        default:
            break;
        }
    }
    RoadHelper->FixRoad(UnitLength);
    StructureHelper->PlaceStructuresAroundRoad(RoadHelper->GetRoadPositions(), UnitLength);

    //for (auto Position : Positions)
    //{
    //    GetWorld()->SpawnActor<AActor>(PrefabClass, Position, FRotator(0.f, 0.f, 0.f));
    //}
}

void AVisualizer::Reset()
{
    RoadHelper->Reset();
    StructureHelper->Reset();
    Positions.Empty();
    //FTimerHandle TimerHandle;
    //GetWorldTimerManager().SetTimer(TimerHandle, this, &AVisualizer::CreateTown, 0.2f, false);
}

void AVisualizer::CreateTown()
{
    SequenceToGenerate = LSystemGenerator->GenerateSentence();
    //UE_LOG(LogTemp, Warning, TEXT("Sentence: %s"), *SequenceToGenerate);
    VisualizeSequence(SequenceToGenerate);
}

void AVisualizer::ClearTown()
{
    RoadHelper->DestroyObjects();
    StructureHelper->DestroyObjects();

    //FTimerHandle TimerHandle;
    //GetWorldTimerManager().SetTimer(TimerHandle, this, &AVisualizer::Reset, 0.2f, false); 
}

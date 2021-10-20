// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TownAsset.generated.h"

UCLASS()
class WESTERNSHOOTER_API ATownAsset : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATownAsset();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnTargets();

private:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ATarget> TargetClass;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		int32 TargetCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation7;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* SpawnLocation10;

	TArray<USceneComponent*> SpawnLocationArray;

	void ShuffleArray();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetHit, class AShooterCharacter*, ShooterCharacter);

/**
 * 
 */
UCLASS()
class WESTERNSHOOTER_API AShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnTargetHit OnTargetHit;

};

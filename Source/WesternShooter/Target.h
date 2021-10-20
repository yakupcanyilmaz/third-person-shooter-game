// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHitInterface.h"
#include "Target.generated.h"

enum class ETargetState : uint8
{
	ETS_Still UMETA(DisplayName = "Still"),
	ETS_Shot UMETA(DisplayName = "Shot"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class WESTERNSHOOTER_API ATarget : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TargetMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class AActor> DestroyedVersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
		class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		int32 MaterialIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* MaterialInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE USoundCue* GetHitSound() const { return HitSound; }
	FORCEINLINE TSubclassOf <AActor> GetDestroyedVersion() const { return DestroyedVersion; }

	void SpawnDestroyedVersion();

	void EnableGlowMaterial();
	void DisableGlowMaterial();

	virtual void BulletHit_Implementation(FHitResult HitResult) override;
};

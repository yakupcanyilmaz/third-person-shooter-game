// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "ShooterGameModeBase.h"
#include "ShooterGameStateBase.h"


// Sets default values
ATarget::ATarget() : 
	MaterialIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	SetRootComponent(TargetMesh);
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATarget::OnConstruction(const FTransform& Transform)
{
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		TargetMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
	}
	EnableGlowMaterial();
}

void ATarget::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void ATarget::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void ATarget::SpawnDestroyedVersion()
{
	GetWorld()->SpawnActor<AActor>(DestroyedVersion, GetActorLocation(), GetActorRotation());
}

void ATarget::BulletHit_Implementation()
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (ShooterCharacter)
	{
		ShooterCharacter->DestroyTarget(this);
	}
}
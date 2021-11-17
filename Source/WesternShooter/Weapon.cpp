// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

#include "BulletHitInterface.h"
#include "Target.h"
#include "ShooterCharacter.h"
#include "ShooterGameModeBase.h"

#define COLLISION_WEAPON			ECC_GameTraceChannel1

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(8),
	MagazineCapacity(8),
	WeaponType(EWeaponType::EWT_Rifle),
	AmmoType(EAmmoType::EAT_RifleAmmo),
	ReloadMontageSection(FName(TEXT("Reload Rifle"))),
	AutoFireRate(.5f),
	bAutomatic(true),
	ScoreNumberDestroyTime(1.5f)
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(true);
	SetReplicates(true);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateScoreNumbers();

	// Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::Fire()
{
	ServerFire();
	if (TargetHit)
	{
		ServerShowTargetScore(HitLocation);
	}
}

void AWeapon::ServerShowTargetScore_Implementation(FVector Location)
{
	ClientShowTargetScore(Location);
}

void AWeapon::ClientShowTargetScore_Implementation(FVector Location)
{
	OnShowScoreNumbers(Location);
}

void AWeapon::StoreScoreNumbers(UUserWidget* ScoreNumber, FVector Location)
{
	ScoreNumbers.Add(ScoreNumber, Location);

	FTimerHandle ScoreNumberTimer;
	FTimerDelegate ScoreNumberDelegate;

	ScoreNumberDelegate.BindUFunction(this, FName("DestroyScoreNumber"), ScoreNumber);
	GetWorld()->GetTimerManager().SetTimer(ScoreNumberTimer, ScoreNumberDelegate, ScoreNumberDestroyTime, false);
	UE_LOG(LogTemp, Warning, TEXT("Store"));
}

void AWeapon::DestroyScoreNumber(UUserWidget* ScoreNumber)
{
	UE_LOG(LogTemp, Warning, TEXT("Remove"));
	ScoreNumbers.Remove(ScoreNumber);
	ScoreNumber->RemoveFromParent();
}

void AWeapon::UpdateScoreNumbers()
{
	for (auto& HitPair : ScoreNumbers)
	{
		UUserWidget* ScoreNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		ScoreNumber->SetPositionInViewport(ScreenPosition);
	}
}

void AWeapon::ServerFire_Implementation()
{
	MulticastFire();
}

bool AWeapon::ServerFire_Validate()
{
	return true;
}

void AWeapon::MulticastFire_Implementation()
{
	if (LineTrace(OutHitResult))
	{
		HitLocation = OutHitResult.Location;
		PlayFireEffects(HitLocation);
		PlayFireSound();

		if (OutHitResult.Actor.IsValid())
		{
			IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(OutHitResult.Actor.Get());
			if (BulletHitInterface)
			{
				BulletHitInterface->BulletHit_Implementation();
				OnHit();
				TargetHit = true;
			}
			else 
			{
				TargetHit = false;
			}
		}
	}
}

bool AWeapon::LineTrace(FHitResult& HitResult)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector FireDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (FireDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bReturnPhysicalMaterial = true;

		if(GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			return true;
		}
	}
	return false;
}

void AWeapon::PlayFireEffects(FVector TraceEnd)
{
	const FVector BarrelSocketLocation = ItemMesh->GetSocketLocation("BarrelSocket");

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketLocation);
	}

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, TraceEnd);
	}

	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, BarrelSocketLocation);
	if (Beam)
	{
		Beam->SetVectorParameter(FName("Target"), TraceEnd);
	}
}

void AWeapon::PlayFireSound()
{
	// Play fire sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AWeapon::OnHitLocation()
{
	PlayFireEffects(HitLocation);
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	// Direction in which we throw the Weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

	EnableGlowMaterial();
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to reload with more than magazine capacity!"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(AWeapon, HitLocation, COND_SkipOwner);
	DOREPLIFETIME(AWeapon, HitLocation);
}

void AWeapon::OnHit()
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
	AShooterGameModeBase* GM = Cast<AShooterGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM && ShooterCharacter)
	{
		GM->OnTargetHit.Broadcast(ShooterCharacter);
	}
	//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, FString::Printf(TEXT("Hit!")));
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 *
 */
UCLASS()
class WESTERNSHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

protected:
	void StopFalling();

	void PlayFireEffects(FVector TraceEnd);

	void PlayFireSound();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();


	UFUNCTION(Server, Reliable)
	void ServerShowTargetScore(FVector Location);
	UFUNCTION(Client, Reliable)
	void ClientShowTargetScore(FVector Location);

	void OnHit();

	UFUNCTION(BlueprintCallable)
	void StoreScoreNumbers(class UUserWidget* ScoreNumber, FVector Location);

	UFUNCTION()
	void DestroyScoreNumber(UUserWidget* ScoreNumber);

	void UpdateScoreNumbers();

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category ="Weapon Properties", meta= (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bAutomatic;

	/*UPROPERTY(ReplicatedUsing=OnHitLocation)
	FVector_NetQuantize HitLocation;*/

	FHitResult OutHitResult;

	UPROPERTY(Replicated)
	FVector_NetQuantize HitLocation;

	UFUNCTION()
	void OnHitLocation();

	bool LineTrace(FHitResult& HitResult);

	UPROPERTY(Replicated)
	bool TargetHit;

	UPROPERTY(VisibleAnywhere, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> ScoreNumbers;

	UPROPERTY(EditAnywhere, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	float ScoreNumberDestroyTime;

public:

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; }
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE bool GetAutomatic() const { return bAutomatic; }

	void Fire();

	void ThrowWeapon();
	void DecrementAmmo();
	void ReloadAmmo(int32 Amount);

	bool ClipIsFull();

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowScoreNumbers(FVector Location);
};

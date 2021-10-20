#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_RifleAmmo UMETA(DisplayName = "RifleAmmo"),
	EAT_ShotgunAmmo UMETA(DisplayName = "ShotgunAmmo"),

	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};
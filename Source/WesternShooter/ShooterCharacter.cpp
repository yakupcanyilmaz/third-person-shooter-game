// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

#include "Item.h"
#include "Weapon.h"
#include "Ammo.h"
#include "Target.h"
#include "ShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	// Base rates for turning/looking up
	BaseTurnRate(35.f),
	BaseLookUpRate(35.f),
	SprintTurnRate(10.f),
	// Turn rates for aiming/not aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	// Mouse look sensitivity scale factors 
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookUpRate(0.6f),
	// true when aiming the weapon
	bAiming(false),
	// true when sprinting
	bSprinting(false),
	// Camera field of view values
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(45.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// Crosshair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// Automatic fire variables
	bFireButtonPressed(false),
	// Starting ammo amounts
	StartingRifleAmmo(32),
	StartingShotgunAmmo(8),
	// Combat variables
	CombatState(ECombatState::ECS_Unoccupied),
	BaseMovementSpeed(600.f),
	CrouchMovementSpeed(400.f),
	SprintMovementSpeed(800.f),
	bAimingButtonPressed(false),
	bSprintButtonPressed(false),
	// Bullet fire timer variables
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	// Item trace variables
	OverlappedItemCount(0)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 70.f, 50.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	SetReplicateMovement(true);
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	ShooterPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	HUDOverlay = CreateWidget<UUserWidget>(ShooterPlayerController, HUDOverlayClass);
	PauseMenu = CreateWidget<UUserWidget>(ShooterPlayerController, PauseMenuClass);

	if (HUDOverlay)
	{
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	if (HasAuthority())
	{
		DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	EquipWeapon(DefaultWeapon);

	InitializeAmmoMap();
}


void AShooterCharacter::MoveForward(float Value)
{
		if ((Controller != nullptr) && (Value != 0.f))
		{
			if (bSprinting && Value > 0.f)
			{
				Sprint();
			}
			else if (bSprinting && Value <= 0.f)
			{
				StopSprinting();
			}
			const FRotator Rotation{ Controller->GetControlRotation() };
			const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
			const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

			AddMovementInput(Direction, Value);
			//const FVector Direction{ GetActorForwardVector() };
			//AddMovementInput(Direction * Value);
		}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{	
		float MoveForwardInputValue = GetInputAxisValue("MoveForward");
		if (bSprinting && MoveForwardInputValue > 0.f)
		{
			AddControllerYawInput(Value * SprintTurnRate * GetWorld()->GetDeltaSeconds());
		}
		else
		{
			StopSprinting();
		}

		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);

		//const FVector Direction{ GetActorRightVector() };
			//AddMovementInput(Direction * Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo() && !bSprinting)
	{
		SendBullet();

		if (bAiming && AimFireMontage && HipFireMontage)
		{
			PlayAnimationMontage(AimFireMontage);
		}
		else
		{
			PlayAnimationMontage(HipFireMontage);
		}

		EquippedWeapon->DecrementAmmo();

		StartFireTimer();

		// Start bullet fire timer for crosshairs
		StartCrosshairBulletFire();
	}
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::SprintButtonPressed()
{
	bSprintButtonPressed = true;
	if (!bAiming &&  CombatState != ECombatState::ECS_Reloading)
	{
		Sprint();
	}
}

void AShooterCharacter::SprintButtonReleased()
{
	bSprintButtonPressed = false;
	StopSprinting();
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}
	CrosshairSpreadMultiplier =
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, EquippedWeapon->GetAutoFireRate());
}

void AShooterCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon == nullptr) return;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::EquipDefaultWeapon()
{
	if (DefaultWeapon)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(DefaultWeapon, GetMesh());
		}
		EquippedWeapon = DefaultWeapon;
		EquippedWeapon->SetOwner(this);
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetOwner(this);
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (OverlappedItem)
	{
		auto Weapon = Cast<AWeapon>(OverlappedItem);
		if (Weapon)
		{
			SwapWeapon(Weapon);
			UGameplayStatics::PlaySound2D(this, Weapon->GetPickupSound());
		}
		auto Ammo = Cast<AAmmo>(OverlappedItem);
		if (Ammo)
		{
			PickupAmmo(Ammo);
			UGameplayStatics::PlaySound2D(this, Ammo->GetPickupSound());
		}
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (!HasAuthority()) ServerSwapWeapon(WeaponToSwap);

	DropWeapon();
	EquipWeapon(WeaponToSwap);
}

void AShooterCharacter::ServerSwapWeapon_Implementation(AWeapon* WeaponToSwap)
{
	SwapWeapon(WeaponToSwap);
}

bool AShooterCharacter::ServerSwapWeapon_Validate(AWeapon* WeaponToSwap)
{
	return true;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_RifleAmmo, StartingRifleAmmo);
	AmmoMap.Add(EAmmoType::EAT_ShotgunAmmo, StartingShotgunAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::SendBullet()
{
	EquippedWeapon->Fire();
}

void AShooterCharacter::DestroyTarget(ATarget* Target)
{
	GetWorld()->DestroyActor(Target);
	GetWorld()->SpawnActor<AActor>(Target->GetDestroyedVersion(), Target->GetActorLocation(), Target->GetActorRotation());
	UGameplayStatics::PlaySound2D(Target, Target->GetHitSound());
}

void AShooterCharacter::PlayAnimationMontage(UAnimMontage* AnimMontage)
{
	ServerPlayAnimationMontage(AnimMontage);
}

void AShooterCharacter::ServerPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage)
{
	MulticastPlayAnimationMontage(AnimMontage);
}

bool AShooterCharacter::ServerPlayAnimationMontage_Validate(UAnimMontage* AnimMontage)
{
	return true;
}

void AShooterCharacter::MulticastPlayAnimationMontage_Implementation(UAnimMontage* AnimMontage)
{
	if (AnimMontage == nullptr) return;
	
	PlayAnimMontage(AnimMontage);
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (!HasAuthority()) ServerReloadWeapon();

	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;

	// Do we have ammo of the correct type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}
		if (bSprinting)
		{
			StopSprinting();
		}
		CombatState = ECombatState::ECS_Reloading;

		PlayAnimationMontage(ReloadMontage);
	}
}

void AShooterCharacter::ServerReloadWeapon_Implementation()
{
	MulticastReloadWeapon();
}

bool AShooterCharacter::ServerReloadWeapon_Validate()
{
	return true;
}

void AShooterCharacter::MulticastReloadWeapon_Implementation()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;

	// Do we have ammo of the correct type?
	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bAiming)
		{
			StopAiming();
		}
		if (bSprinting)
		{
			StopSprinting();
		}
		CombatState = ECombatState::ECS_Reloading;

		PlayAnimationMontage(ReloadMontage);
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::Aim()
{
	if (!HasAuthority()) ServerAim();

	bAiming = true;
	bSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::ServerAim_Implementation()
{
	MulticastAim();
}

bool AShooterCharacter::ServerAim_Validate()
{
	return CrouchMovementSpeed == 400.f ;
}

void AShooterCharacter::MulticastAim_Implementation()
{
	bAiming = true;
	bSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	if (!HasAuthority()) ServerStopAiming();

	bAiming = false;
	if (bSprintButtonPressed)
	{
		bSprinting = true;
	}
	if (bSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::ServerStopAiming_Implementation()
{
	MulticastStopAiming();
}

bool AShooterCharacter::ServerStopAiming_Validate()
{
	return BaseMovementSpeed == 600.f && SprintMovementSpeed == 800.f;
}

void AShooterCharacter::MulticastStopAiming_Implementation()
{
	bAiming = false;
	if (bSprintButtonPressed)
	{
		bSprinting = true;
	}
	if (bSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::Sprint()
{
	if (!HasAuthority()) ServerSprint();

	bSprinting = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
}

void AShooterCharacter::ServerSprint_Implementation()
{
	MulticastSprint();
}

bool AShooterCharacter::ServerSprint_Validate()
{
	return SprintMovementSpeed == 800.f;
}

void AShooterCharacter::MulticastSprint_Implementation()
{
	bSprinting = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
}

void AShooterCharacter::StopSprinting()
{
	if (!HasAuthority()) ServerStopSprinting();

	bSprinting = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	if (bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::ServerStopSprinting_Implementation()
{
	MulticastStopSprinting();
}

bool AShooterCharacter::ServerStopSprinting_Validate()
{
	return BaseMovementSpeed == 600.f && CrouchMovementSpeed == 400.f;
}

void AShooterCharacter::MulticastStopSprinting_Implementation()
{
	bSprinting = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	if (bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	ServerPickupAmmo(Ammo);
}

void AShooterCharacter::ServerPickupAmmo_Implementation(AAmmo* Ammo)
{
	MulticastPickupAmmo(Ammo);
}

bool AShooterCharacter::ServerPickupAmmo_Validate(AAmmo* Ammo)
{
	return true;
}

void AShooterCharacter::MulticastPickupAmmo_Implementation(AAmmo* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->OnPickup_Implementation();
}

void AShooterCharacter::DestroyItem(AItem* Item)
{
	GetWorld()->DestroyActor(Item);
}

void AShooterCharacter::PauseButtonPressed()
{
	ShooterPlayerController->SetInputMode(FInputModeUIOnly());
	ShooterPlayerController->SetPause(true);

	if (HUDOverlay)
	{
		HUDOverlay->RemoveFromParent();
	}

	if (PauseMenu)
	{
		PauseMenu->AddToViewport();
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		ShooterPlayerController->bShowMouseCursor = true;
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("SprintButton", IE_Pressed, this, &AShooterCharacter::SprintButtonPressed);
	PlayerInputComponent->BindAction("SprintButton", IE_Released, this, &AShooterCharacter::SprintButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this,&AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this,&AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AShooterCharacter::PauseButtonPressed);
}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (bSprintButtonPressed && !bAimingButtonPressed)
	{
		Sprint();
	}

	if (EquippedWeapon == nullptr) return;
	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];

		const int32 MagEmptySpace =
			EquippedWeapon->GetMagazineCapacity() -
			EquippedWeapon->GetAmmo();
		if (MagEmptySpace > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::SetOverlappedItem(AItem* Item)
{
	OverlappedItem = Item;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
	}
	else
	{
		OverlappedItemCount += Amount;
	}
}

FVector AShooterCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, bAiming);
	DOREPLIFETIME(AShooterCharacter, EquippedWeapon);
	DOREPLIFETIME(AShooterCharacter, OverlappedItem);
}
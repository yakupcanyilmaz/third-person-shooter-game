// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	TIPMaxDegree(90.f),
	YawDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	OffsetState(EOffsetState::EOS_Hip)
{

}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		//MovementOffsetYaw = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::InverseTransformDirection(ShooterCharacter->GetActorTransform(), ShooterCharacter->GetVelocity())).Yaw;

		bAiming = ShooterCharacter->GetAiming();
		bSprinting = ShooterCharacter->GetSprinting();

		if (bAiming)
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}

		TurnInPLace(DeltaTime);
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPLace(float DeltaTime)
{
	//Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;
	Pitch = (ShooterCharacter->GetControlRotation() - ShooterCharacter->GetActorRotation()).GetNormalized().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		// Don't want to turn in place; Character is moving
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		// Root Yaw Offset, updated and clamped to [-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// 1.0 if turning, 0.0 if not
		const float Turning{ GetCurveValue(TEXT("Turning")) };
	
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(3, -1, FColor::Cyan, FString::Printf(TEXT("RotationCurve: %f"), RotationCurve));
		//	GEngine->AddOnScreenDebugMessage(4, -1, FColor::Cyan, FString::Printf(TEXT("RotationCurveLastFrame: %f"), RotationCurveLastFrame));
		//	GEngine->AddOnScreenDebugMessage(5, -1, FColor::Green, FString::Printf(TEXT("Turning: %f"), Turning));
		//}
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ (RotationCurve - RotationCurveLastFrame) };

			//if (GEngine)
			//{
			//	GEngine->AddOnScreenDebugMessage(6, -1, FColor::Cyan, FString::Printf(TEXT("DeltaRotation: %f"), DeltaRotation));
			//}

			// RootYawOffset > 0, -> Turning Left. RootYawOffset < 0, -> Turning Right.
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > TIPMaxDegree)
			{
				const float YawExcess{ ABSRootYawOffset - TIPMaxDegree };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			bTurningInPlace = false;
		}
	}
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, -1, FColor::Red, FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
	//	GEngine->AddOnScreenDebugMessage(2, -1, FColor::Blue, FString::Printf(TEXT("CharacterYaw: %f"), TIPCharacterYaw));
	//}
}

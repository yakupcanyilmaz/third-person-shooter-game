// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::SetMenuInterface(IMenuInterface* MInterface)
{
	this->MenuInterface = MInterface;
}

void UMenuWidget::Setup()
{
	this->AddToViewport();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(this->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);

			PlayerController->bShowMouseCursor = true;
		}
	}
}

//void UMenuWidget::Teardown()
//{
//	this->RemoveFromViewport();
//	UWorld* World = GetWorld();
//	if (World)
//	{
//		APlayerController* PlayerController = World->GetFirstPlayerController();
//		if (PlayerController)
//		{
//			FInputModeGameOnly InputModeData;
//			PlayerController->SetInputMode(InputModeData);
//			PlayerController->bShowMouseCursor = false;
//		}
//	}
//}

void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	this->RemoveFromViewport();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = false;
		}
	}
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
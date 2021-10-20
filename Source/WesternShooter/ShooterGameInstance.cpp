// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "MainMenu.h"


UShooterGameInstance::UShooterGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/_Game/Menus/MultiMainMenuBP"));
	if (MenuBPClass.Class)
	{
		MenuClass = MenuBPClass.Class;
	}
}

void UShooterGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MenuClass->GetName());
}


void UShooterGameInstance::Host()
{
	//if (Menu)
	//{
	//	Menu->Teardown();
	//}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/_Game/Maps/ArenaMap?listen");
	}
}

void UShooterGameInstance::Join(const FString& Address)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}

}

void UShooterGameInstance::LoadMenu() 
{
	if (MenuClass)
	{
		Menu = CreateWidget<UMainMenu>(this, MenuClass);
		if (Menu)
		{
			Menu->Setup();
			Menu->SetMenuInterface(this);
		}
	}
}
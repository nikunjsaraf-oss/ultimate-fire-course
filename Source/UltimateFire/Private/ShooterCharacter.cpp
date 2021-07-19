// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called"));

	const int myInt{18};
	UE_LOG(LogTemp, Warning, TEXT("int myInt: %d"), myInt);

	const float myFloat{18.5f};
	UE_LOG(LogTemp, Warning, TEXT("float myFloat: %f"), myFloat);

	const double myDouble{19.000005};
	UE_LOG(LogTemp, Warning, TEXT("double myDouble: %lf"), myDouble);

	const char myChar{'N'};
	UE_LOG(LogTemp, Warning, TEXT("char myChar: %c"), myChar);

	const wchar_t myWideChar{L'J'};
	UE_LOG(LogTemp, Warning, TEXT("wchar_t myWideChar: %lc"), myWideChar);

	const bool myBool{true};
	UE_LOG(LogTemp, Warning, TEXT("bool myBool: %d"), myBool);

	const FString myString{"My String!!!!!!!!!"};
	UE_LOG(LogTemp, Warning, TEXT("FString myString: %s"), *myString);
	
	UE_LOG(LogTemp, Warning, TEXT("Object name: %s"), *GetName());
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

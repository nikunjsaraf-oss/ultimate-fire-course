// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
	: BaseTurnRate(45.0f),
	  BaseLookupRate(45.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when controller rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;	// Move in Input's direction
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0); // Move at this rate
	GetCharacterMovement()->JumpZVelocity = 600;
	GetCharacterMovement()->AirControl = 0.2f;

	// Setup Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent()); // Attach the Boom to the root component of the object
	CameraBoom->TargetArmLength = 300.0f; // Camera will have this fixed distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the camera based on Controller's Input

	// Setup Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	// Attach Camera at the end of the Springarm component
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Doesn't rotate the camera relative to the arm.
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
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
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AShooterCharacter::MoveForward(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Get and calculate forward vector.
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Get and calculate right vector.
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0, Rotation.Yaw, 0};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(const float Rate)
{
	// Calculate turning rate by converting from deg/sec to deg/frame.
	// Base turn rate = deg/sec
	// Delta seconds = sec/frame
	// Deg/sec * sec/frame = deg/frame.
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(const float Rate)
{
	// Calculate turning rate by converting from deg/sec to deg/frame.
	// Base turn rate = deg/sec
	// Delta seconds = sec/frame
	// Deg/sec * sec/frame = deg/frame.
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
	: BaseTurnRate(45.0f),
	  BaseLookupRate(45.0f),
	  HipTurnRate(90),
	  HipLookUpRate(90),
	  AimTurnRate(20),
	  AimLookUpRate(20),
	  MouseHipTurnRate(1),
	  MouseHipLookUpRate(1),
	  MouseAimingTurnRate(0.5f),
	  MouseAimLookUpRate(0.5f),
	  bIsAimimg(false),
	  CameraDefaultFOV(0.0f), // Set in BeginPlay()
	  CameraZoomedFOV(35.0f),
	  CameraCurrentFOV(0),
	  ZoomInterpSpeed(20.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when controller rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; // Move in Input's direction
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0); // Move at this rate
	GetCharacterMovement()->JumpZVelocity = 600;
	GetCharacterMovement()->AirControl = 0.2f;

	// Setup Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent()); // Attach the Boom to the root component of the object
	CameraBoom->TargetArmLength = 180.0f; // Camera will have this fixed distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the camera based on Controller's Input
	CameraBoom->SocketOffset = FVector(0, 50, 70);

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

	if (FollowCamera)
	{
		CameraDefaultFOV = FollowCamera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAndSetFOV(DeltaTime);

	SetLookRates();

	CalculateCrosshairSpread(DeltaTime);
}

void AShooterCharacter::CalculateAndSetFOV(float DeltaTime)
{
	if (bIsAimimg)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	// Set FOV
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRates()
{
	if (bIsAimimg)
	{
		BaseTurnRate = AimTurnRate;
		BaseLookupRate = AimLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookupRate = HipLookUpRate;
	}
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
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
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

void AShooterCharacter::Turn(const float Value)
{
	float TurnScaleFactor{};
	if (bIsAimimg)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(const float Value)
{
	float LookScaleFactor{};
	if (bIsAimimg)
	{
		LookScaleFactor = MouseAimLookUpRate;
	}
	else
	{
		LookScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	// Check if FireSound is assigned. 
	if (FireSound)
	{
		// If assigned play sound.
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	// Get the socket's reference.
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket) // Check if valid.
	{
		// if valid get it's transform and cache it.
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		// Check if MuzzleFlash is assigned.
		if (MuzzleFlash)
		{
			// If assigned, spawn it at the given transform.
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		const bool bBeamEnd = CheckBeamEnd(SocketTransform.GetLocation(), BeamEnd);;
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

bool AShooterCharacter::CheckBeamEnd(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) const
{
	FVector2D OutViewPortSize; // Get Current ViewPort size
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(OutViewPortSize);
	}

	// Get and calculate crosshair location
	FVector2D CrossHairLocation(OutViewPortSize.X / 2.f, OutViewPortSize.Y / 2.f);
	CrossHairLocation.Y -= 50.0f;

	FVector OutCrossHairWorldPosition;
	FVector OutCrossHairWorldDirection;

	// Get CrossHair position and location
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrossHairLocation,
		OutCrossHairWorldPosition,
		OutCrossHairWorldDirection
	);

	// Was deprojecion successful?
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start{OutCrossHairWorldPosition};
		const FVector End{OutCrossHairWorldPosition + OutCrossHairWorldDirection * 50'000.f};

		// Set beam end point to line trace end point
		OutBeamLocation = End;

		// Trace outward from crosshairs world location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECC_Visibility);
		if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
		{
			// Beam endpoint is now trace end point
			OutBeamLocation = ScreenTraceHit.Location;
		}

		// Perform another line trace from gun barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{MuzzleSocketLocation};
		const FVector WeaponTraceEnd{OutBeamLocation};
		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

		return true;
	}

	return false;
}

void AShooterCharacter::AimingButtonPressed()
{
	bIsAimimg = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bIsAimimg = false;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	const FVector2D WalkSpeedRange{0, 600};
	const FVector2D VelocityMultiplierRange{0, 1};
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

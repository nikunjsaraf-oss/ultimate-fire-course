// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Item.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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
	  CameraDefaultFOV(0.0f),
	  CameraZoomedFOV(35.0f),
	  CameraCurrentFOV(0),
	  ZoomInterpSpeed(20.0f),
	  CrosshairSpreadMultiplier(0),
	  CrosshairVelocityFactor(0),
	  CrosshairAirFactor(0),
	  CrosshairAimFactor(0),
	  CrosshairShootingFactor(0),
	  ShootTimeDuration(0.05f),
	  bIsFiringBullet(false),
	  bIsFireButtonPressed(false),
	  bShouldFire(true),
	  AutomaticFireRate(0.1f),
	  bShoulTraceForItem(false),
	  CameraInterpDistance(250),
	  CameraInterpElevation(65)

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

	// Spawn default weapon and equip it
	EquipWeapon(SpawnDefaultWeapon());
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAndSetFOV(DeltaTime);

	SetLookRates();

	CalculateCrosshairSpread(DeltaTime);

	TraceForItems();
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

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AShooterCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AShooterCharacter::InteractButtonReleased);
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
	float TurnScaleFactor;
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
	float LookScaleFactor;
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

void AShooterCharacter::InteractButtonPressed()
{
	if (TraceHitItemLastFrame)
	{
		AWeapon* TraceHitWeapon = Cast<AWeapon>(TraceHitItemLastFrame);
		SwapWeapon(TraceHitWeapon);
	}
}

void AShooterCharacter::InteractButtonReleased()
{
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TracheHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}


void AShooterCharacter::FireButtonPressed()
{
	bIsFireButtonPressed = true;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bIsFireButtonPressed = false;
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
		const bool bBeamEnd = GetBeamEnd(SocketTransform.GetLocation(), BeamEnd);;
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

	StartCrosshairBulletFire(); // for crosshair
}

bool AShooterCharacter::GetBeamEnd(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) const
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrossHairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative hit location (Still need to trace from gun)
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
		// Outbeamlocation is the end location for the lone trace. 
	}

	// Perform another line trace from gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{MuzzleSocketLocation};
	const FVector StartToEnd{OutBeamLocation - MuzzleSocketLocation};
	const FVector WeaponTraceEnd{MuzzleSocketLocation + StartToEnd * 1.25f};
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
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

	// Calculate Velocity factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());

	// Calculate Air fator
	if (GetCharacterMovement()->IsFalling())
	{
		// Spread crosshair slow
		CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 1.5f, DeltaTime, 2.25f);
	}
	else
	{
		// Character is on ground
		CrosshairAirFactor = FMath::FInterpTo(CrosshairAirFactor, 0, DeltaTime, 30);
	}

	// Calculate Aim Factor
	if (bIsAimimg)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 00.0f, DeltaTime, 30.f);
	}

	if (bIsFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.0f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.0f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairAirFactor - CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bIsFiringBullet = true;

	GetWorldTimerManager().SetTimer(TimerHandle_CrosshairShoot, this, &AShooterCharacter::FinishCrossHairBulletFire,
	                                ShootTimeDuration);
}

void AShooterCharacter::FinishCrossHairBulletFire()
{
	bIsFiringBullet = false;
}


void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(TimerHandle_AutoFire, this, &AShooterCharacter::AutoFireReset,
		                                AutomaticFireRate);
	}
}

void AShooterCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bIsFireButtonPressed)
	{
		StartFireTimer();
	}
}

bool AShooterCharacter::TraceUnderCrossHairs(FHitResult& OutHitResult, FVector& OutHitLocation) const
{
	// Get viewport size.

	FVector2D OutViewPortSize; // Get Current ViewPort size
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(OutViewPortSize);
	}

	// Get and calculate crosshair location
	const FVector2D CrossHairLocation(OutViewPortSize.X / 2, OutViewPortSize.Y / 2);

	FVector OutCrossHairWorldPosition;
	FVector OutCrossHairWorldDirection;


	// Get CrossHair position and location
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrossHairLocation,
		OutCrossHairWorldPosition,
		OutCrossHairWorldDirection
	);

	if (bScreenToWorld)
	{
		// Trace from crosshair world location

		const FVector Start{OutCrossHairWorldPosition};
		const FVector End{Start + OutCrossHairWorldDirection * 50'000};
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShoulTraceForItem)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrossHairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TracheHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TracheHitItem && TracheHitItem->GetPickupWidget())
			{
				// Show Items pickup widget.
				TracheHitItem->GetPickupWidget()->SetVisibility(true);
			}

			// We hit an Item last frame
			if (TraceHitItemLastFrame)
			{
				if (TracheHitItem != TraceHitItemLastFrame)
				{
					// We are hitting Item this fram from last frame || Item is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// Store a reference
			TraceHitItemLastFrame = TracheHitItem;
		}
	}

	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any item
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon() const
{
	if (DefaultWeaponClass)
	{
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}
	return nullptr;
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
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}


void AShooterCharacter::DropWeapon() const
{
	if (EquippedWeapon)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(const int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShoulTraceForItem = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShoulTraceForItem = true;
	}
}

FVector AShooterCharacter::GetCameraInterpLocation() const
{
	const FVector CameraWorldLocation{FollowCamera->GetComponentLocation()};
	const FVector CameraForward{FollowCamera->GetForwardVector()};

	// Desire location = CameraWorldLocation + CameraForward * A + Up * B

	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0, 0, CameraInterpElevation);
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	AWeapon* Weapon = Cast<AWeapon>(Item);
	if(Weapon)
	{
		SwapWeapon(Weapon); 
	}
}

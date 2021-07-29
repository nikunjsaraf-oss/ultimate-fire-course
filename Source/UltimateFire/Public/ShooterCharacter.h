// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AWeapon;

UCLASS()
class ULTIMATEFIRE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * @brief Called for moving forward and backwards
	 * @param Value Input Axis' value passed through PlayerInputComponent
	 */
	void MoveForward(float Value);

	/**
	 * @brief Called for moving sideways
	 * @param Value Input Axis' value passed through PlayerInputComponent
	 */
	void MoveRight(float Value);

	/**
	 * @brief To turn at given rate.
	 * @param Rate Normalized rate (1 = 100% of desired rate).
	 */
	void TurnAtRate(float Rate);

	/**
	 * @brief To look up/down at given rate.
	 * @param Rate Normalized rate (1 = 100% of desired rate).
	 */
	void LookUpAtRate(float Rate);


	/**
	* @brief Turn the controller from mouse X input.
	* @param Value Input value from mouse X movement
	*/
	void Turn(float Value);

	/**
	* @brief LookUp  from mouse Y input.
	* @param Value Input value from mouse Y movement
	*/
	void LookUp(float Value);

	/**
	 * @brief Call when fire button is pressed
	 */
	void FireWeapon();

	/**
	 * @brief Check if the ray trace is successful or not.
	 * @param MuzzleSocketLocation Location from where he beam is supposed to start.
	 * @param OutBeamLocation Location of the end point of the beam.
	 * @return bool
	 */
	bool GetBeamEnd(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) const;

	/**
	 * @brief Set bIsAimimg to true
	 */
	void AimingButtonPressed();

	/**
	 * @brief Set bIsAimimg to false
	 */
	void AimingButtonReleased();

	/**
	 * @brief  To calculate and set Current Field Of View
	 * @param DeltaTime World Delta Time in s
	 */
	void CalculateAndSetFOV(float DeltaTime);
	/**
	 * @brief Change sensitivity for aiming.
	 */
	void SetLookRates();

	/**
	 * @brief Function to calculate how much our crosshair should spread.
	 * @param DeltaTime World's Delta time in seconds
	 */
	void CalculateCrosshairSpread(float DeltaTime);

	/**
	 * @brief When firing, call this to start shrinking crosshair
	 */
	void StartCrosshairBulletFire();

	/**
	 * @brief After finished firing, call this to rest crosshair position.
	 * I marked this UFunction because this is called from SetTimer(). 
	 */
	UFUNCTION()
	void FinishCrossHairBulletFire();

	/**
	 * @brief Sets bIsFireButtonPressed to true and calls start StartFireTimer.
	 */
	void FireButtonPressed();

	/**
	 * @brief Sets bIsFireButtonPressed to false.
	 */
	void FireButtonReleased();

	/**
	 * @brief Sets TimerHandle_AutoFire for automatic firing
	 */
	void StartFireTimer();

	/**
	 * @brief Resets the firing condition.
	 */
	UFUNCTION()
	void AutoFireReset();

	/**
	 * @brief Line trace for items under crosshairs
	 * @param OutHitResult Stores the hit result information.
	 * @param OutHitLocation Stores the location of hit result.
	 * @return If line trace is successful, it will return true.
	 */
	bool TraceUnderCrossHairs(FHitResult& OutHitResult, FVector& OutHitLocation) const;

	/**
	 * @brief Trace for items is overlappedItemCount > 0
	 */
	void TraceForItems();

	/**
	 * @brief Spawns a default a weapon and equips it.
	 */
	AWeapon* SpawnDefaultWeapon() const;

	/**
	 * @brief Takes a weapon and attaches it to the mesh.
	 */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/**
	* @brief Detach weapon and let it fall to the ground.
	*/
	void DropWeapon() const;

	void InteractButtonPressed();
	void InteractButtonReleased();

	/**
	 * @brief Drops currently equipped weapon and equips tracehit weapon
	 */
	void SwapWeapon(AWeapon* WeaponToSwap);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/**
	 * @brief Camera boom is used for positioning behind the character
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**
	 * @brief It is attached to the camera boom and follows the character throughout the game.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**
	 * @brief Turn rate in degrees per second. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**
	 * @brief Look up/down Rate in degrees per second 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	/**
	 * @brief  Turn rate while not aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	/**
	 * @brief Look up/down rate while not aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	/**
	 * @brief Turn rate while aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float AimTurnRate;

	/**
	 * @brief Look up/down rate while aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float AimLookUpRate;

	/**
	 * @brief Scale factor for mouse look sensitivty.
	 * Turn rate when not aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClamMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	/**
	* @brief Scale factor for mouse look sensitivty.
	* LookUp rate when not aiming.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClamMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	/**
	* @brief Scale factor for mouse look sensitivty.
	* Turn rate when aiming.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClamMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	/**
	* @brief Scale factor for mouse look sensitivty.
	* LookUp rate when not aiming.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClamMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimLookUpRate;

	/**
	 * @brief Randomized gun shot sound cue
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/**
	 * @brief Spawn 'MuzzleFlash' at the barrel socket
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/**
	 * @brief Montage for firing weapon
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/**
	 * @brief Particles to spawn after bullet get hit.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	/**
	 * @brief Smoke trail for bullets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	/**
	 * @brief To check if aim is on or not.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsAimimg;

	/**
	 * @brief Default FOV for our follow camera
	 */
	float CameraDefaultFOV;

	/**
	 * @brief Zoomed FOV for Follow Camera
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	/**
	 * @brief Camera's Current FOV for interpolation
	 */
	float CameraCurrentFOV;

	/**
	 * @brief  Speed for interpolation of FOV
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/**
	 * @brief Determines the spread of crosshairs.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/**
	 * @brief Velocity component for crosshair spread.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/**
	 * @brief Air component for crosshair spread.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairAirFactor;

	/**
	 * @brief Aim component for crosshair spread.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/**
	 * @brief Shooting component for crosshair spread.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/**
	 * @brief Rate at which we have to set timer for crosshair-shooting factor
	 */
	float ShootTimeDuration;

	/**
	 * @brief bool to store whether is firing or not
	 */
	bool bIsFiringBullet;

	/**
	 * @brief Timerhandle to reset crosshair position after firing
	 */
	FTimerHandle TimerHandle_CrosshairShoot;

	/**
	 * @brief whether or not fire button is pressed or not.
	 */
	bool bIsFireButtonPressed;

	/**
	 * @brief if false, cannot fire bullet.
	 */
	bool bShouldFire;

	/**
	 * @brief Rate of Gunfir
	 */
	float AutomaticFireRate;

	/**
	 * @brief Sets a timer for gunshots.
	 */
	FTimerHandle TimerHandle_AutoFire;

	/**
	 * @brief true if we should trace every frame for items.
	 */
	bool bShoulTraceForItem;

	/**
	 * @brief Number of overlapped AItems 
	 */
	int8 OverlappedItemCount;

	/**
	 * @brief The AItem we hit last frame.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	/**
	 * @brief Currently equipped weapon
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/**
	 * @brief Set default weapon class in blueprints
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/**
	 * @brief Item currently hit by our trace hit for items (could be null)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess = "true"))
	AItem* TracheHitItem;

public:
	/**
	 * @brief Returns the CameraBoom Subobject
	 * @return USpringArmComponent*
	 */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/**
	 * @brief Returns the FollowCamera SubObject
	 * @return UCameraComponent*
	 */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/**
	 * @brief Returns where IsAiming is true or not.
	 * @return IsAiming
	 */
	FORCEINLINE bool GetIsAiming() const { return bIsAimimg; }

	/**
	 * @brief Returns the multiplying factor for CrossHair.
	 * @return CrosshairSpread Multiplier
	 */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/**
	 * @brief Adds/Subtracts to/from OverlappedItemCount and updates bShoulTraceForItem. 
	 */
	void IncrementOverlappedItemCount(int8 Amount);
};

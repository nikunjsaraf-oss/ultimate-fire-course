// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

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
	 * @brief Call when fire button is pressed
	 */
	void FireWeapon();

	/**
	 * @brief Check if the ray trace is successful or not.
	 * @param MuzzleSocketLocation Location from where he beam is supposed to start.
	 * @param OutBeamLocation Location of the end point of the beam.
	 * @return bool
	 */
	bool CheckBeamEnd(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) const;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/**
	 * @brief Camera boom is used for positioning behind the character
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/**
	 * @brief It is attached to the camera boom and follows the character throughout the game.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**
	 * @brief Turn rate in degrees per second. 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/**
	 * @brief Look up/down Rate in degrees per second 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

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
};

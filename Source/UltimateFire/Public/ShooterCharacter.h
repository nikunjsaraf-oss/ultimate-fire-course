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
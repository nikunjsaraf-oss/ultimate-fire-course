// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFIRE_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/**
	 * @brief Used for initializing properties.
	 * Just like BeginPlay.
	 */
	virtual void NativeInitializeAnimation() override;

	/**
	 * @brief used for calling per frame to update the animation properties.
	 * Like Tick() from actors.
	 * @param DeltaTime For caching one frame's time.
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	/**
	 * @brief Character's Speed.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	/**
	 * @brief if character is in air it will return true.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	/**
	 * @brief If character is moving, it will return true.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;
};

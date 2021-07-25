// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class ULTIMATEFIRE_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/**
	* @brief Skeletal mesh for the item
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/**
	 * @brief Line trace collides with box to show HUD widgets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	/**
	 * @brief Widget blueprint for pickup guns
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};

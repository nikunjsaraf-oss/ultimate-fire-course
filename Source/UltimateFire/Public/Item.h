// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),


	EIS_MAX UMETA(DisplayName = "Default")
};


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

	/**
	 * @brief Function to call when there is a overlap
	 */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
	                     OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	* @brief Function to call when there overlap finishes
	*/
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
	                        OtherComp, int32 OtherBodyIndex);


	/**
	 * @brief Set active stars boolean based on rarity
	 */
	void SetActiveStars();

	/**
	 * @brief Function to set the item state
	 * @param State State of item to set
	 */
	void SetItemProperties(EItemState State) const;

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

	/**
	 * @brief Enable item tracing when overlapped.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/**
	 * @brief Name to show in the widget.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	FString ItemName;

	/**
	* @brief Number of ammo to show in the widget.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	int32 ItemCount;

	/**
	 * @brief Item Rarity - determines the number of stars in WBP
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/**
	 * @brief To hold the number of stars
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/**
	 * @brief State of the item
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ItemProperties", meta=(AllowPrivateAccess = "true"))
	EItemState ItemState;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	
	void SetItemState(const EItemState State);
};

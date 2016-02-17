// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "MyPlayerController.h"
#include "MyBomb.generated.h"

UCLASS()
class LEETAPIDEMO3_API AMyBomb : public AActor
{
	GENERATED_BODY()

	UFUNCTION(Category = "LEET", Server, Reliable, WithValidation)
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	
private:
	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UProjectileMovementComponent* MovementComp;
	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;
	//UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	//	UParticleSystemComponent* ParticleComp;

	
public:	

	/** point light component */
	UPROPERTY(VisibleAnywhere, Category = "LEET")
	class UPointLightComponent* PointLight1;

	/** sphere component */
	UPROPERTY(VisibleAnywhere, Category = "LEET")
	class USphereComponent* Sphere1;

	// We want to keep track of who created the bomb so they get credit for any kills
	int32 ownerPlayerID;
	//class AMyPlayerController ownerPlayerController;

	// Sets default values for this actor's properties
	AMyBomb();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void setPlayerID(int32 playerID);
	//void setPlayerController(class AMyPlayerController ownerPlayerController);

	/** the desired intensity for the light */
	UPROPERTY(VisibleAnywhere, Category = "LEET")
		float DesiredIntensity;

};

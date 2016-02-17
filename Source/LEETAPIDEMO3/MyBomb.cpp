// Fill out your copyright notice in the Description page of Project Settings.

#include "LEETAPIDEMO3.h"
#include "LEETAPIDEMO3Character.h"
#include "MyBomb.h"
#include "Engine.h"
//All particle definitions for using functions of UParticleSystemComponent
//#include "ParticleDefinitions.h"

// Sets default values
AMyBomb::AMyBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DesiredIntensity = 30000.0f;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyBomb::OnHit);
	RootComponent = CollisionComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>( TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2500.0f;
	MovementComp->MaxSpeed = 2500.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bShouldBounce = true;
	MovementComp->Bounciness = 100.f;
	MovementComp->Friction = 0.f;

	// Create and position a mesh component so we can see where our sphere is
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		SphereVisual->SetWorldScale3D(FVector(0.5f));
	}

	//ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MyParticleSystem"));
	//ParticleComp->bAutoActivate = true;
	//ParticleComp->bAutoDestroy = true;
	//ParticleComp->AttachParent = RootComponent;

	PointLight1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight1"));
	PointLight1->Intensity = DesiredIntensity;
	PointLight1->bVisible = true;
	PointLight1->AttachParent = RootComponent;

	Sphere1 = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere1"));
	Sphere1->InitSphereRadius(20.0f);
	Sphere1->AttachParent = RootComponent;

	bReplicates = true;
	bReplicateMovement = true;

}

// Called when the game starts or when spawned
void AMyBomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyBomb::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMyBomb::setPlayerID(int32 playerID)
{
	UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyBomb] [setPlayerID] %i "), playerID);
	ownerPlayerID = playerID;
}

//void AMyBomb::setPlayerController(AMyPlayerController playerController)
//{
//	UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyBomb] [setPlayerController] "));
//	ownerPlayerController = playerController*;
//}

void AMyBomb::OnHit_Implementation(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyBomb] [OnHit] "));
	// cast to our chatacter.


	ALEETAPIDEMO3Character* TheCharacter = Cast<ALEETAPIDEMO3Character>(OtherActor);
	if (TheCharacter) {
		UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyBomb] [OnHit] Cast to ALEETAPIDEMO3Character"));

		// Create a damage event  
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);

		
		TheCharacter->TakeDamageCustom(.3f, DamageEvent, this);


		
	}
	else {
		// It wasn't a character hit.  Just destroy it.
		bool destroyed = Destroy();
	}

	//UE_LOG(LogTemp, Log, TEXT("[LEET] [AMyBomb] [OnHit] OtherActor: %s"), OtherActor.);
}

bool AMyBomb::OnHit_Validate(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	return true;
}
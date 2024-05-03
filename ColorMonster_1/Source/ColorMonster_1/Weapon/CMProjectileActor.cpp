// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/CMProjectileActor.h"

#include "Character/CMMonster.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Color/CMGameplayTag.h"

// Sets default values
ACMProjectileActor::ACMProjectileActor()
{
	// Sphere Collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(16.0f);
	RootComponent = CollisionComponent;

	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACMProjectileActor::OnHit);
	//CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACMProjectileActor::OnTriggerBegin);

	// Players can't walk on it
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	// Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/FPWeapon/Mesh/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
	if (StaticMeshRef.Object)
	{
		StaticMesh->SetStaticMesh(StaticMeshRef.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CMPROJECTILEACTOR: Failed to Load Static Mesh"));
	}
	// static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT(""));
	// if (MaterialRef.Object)
	// {
	// 	StaticMesh->SetMaterial(0, MaterialRef.Object);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("CMPROJECTILEACTOR: Failed to Load Material"));
	// }

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.6f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	CurrentColor = CM_COLOR_BLUE;
}

// Called when the game starts or when spawned
void ACMProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	//CurrentColor = FGameplayTag::EmptyTag;
}

// Called every frame
void ACMProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACMProjectileActor::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	//CurrentColor = InColor;
}

void ACMProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != this) && (OtherComponent != nullptr))
	{
		// Knock Back
		//OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);

		ensure(OtherActor != nullptr);
		ACMMonster* HitMonster = Cast<ACMMonster>(OtherActor);
		if(HitMonster != nullptr)
		{
			HitMonster->ChangeColor(CurrentColor);
			// refer interface from ideugu class
		}

		Destroy();
	}
}

//GetWorld()->OverlapMultiByChannel()
//UKismetSystemLibrary::SphereOverlapActors()
//UKismetSystemLibrary::SphereTraceSingle()
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/CMCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Interface/CMAIInterface.h"
#include "CMMonster.generated.h"

/**
 * 
 */
UCLASS()
class COLORMONSTER_1_API ACMMonster : public ACMCharacter, public ICMAIInterface
{
	GENERATED_BODY()

public:
	ACMMonster();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CategoryOfMonster, meta = (AllowPrivateAccess = "true"))
	FGameplayTag CurrentCategory;

// Basic Action
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void RotateHPBarToCamera();
	virtual void Dead() override;
	virtual void Attack() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
// For Hp Bar Rotate To Player Camera
private:
	void LoadPlayer();
	TObjectPtr<APlayerController> PlayerController;
public:
	FORCEINLINE float GetAttackRange() const {return AttackRange;}
	FORCEINLINE float GetAttackRadius() const {return AttackRadius;}
	FORCEINLINE float GetAttackDamage() const {return AttackDamage;}
// Attack Data
private:
	float AttackRange = 100.0f;
	float AttackRadius = 100.0f;
	float AttackDamage = 0.1f;

// Decal
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TObjectPtr<class UDecalComponent> DecalComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	FVector DecalSize;

	void InitializeDecalComponent() const;
	void TurnReceiveDecal(bool IsTurnOn) const;
	void UpdateDecal(const FLinearColor& InDecalColor) const;
	void UpdateDecal(const FLinearColor& InDecalColor, const FVector& InDecalSize) const;

private:
	// Decal Material For Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterialInstance> DecalMaterial;

	// Monster - Monster Collision
	UPROPERTY(VisibleDefaultsOnly, Category = "Collision")
	TObjectPtr<class USphereComponent> MonsterCollisionComponent;
	
// Color Data -> Tag
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Color, meta = (AllowPrivateAccess = "true"))
	FGameplayTag CurrentColor;

	UFUNCTION()
	void ChangeColor(const FGameplayTag& InColor);

private:
	UPROPERTY();
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MaterialArray;

// AnimInstance
	UPROPERTY()
	TObjectPtr<class UCMMonsterAnimInstance> AnimInstance;
	
public:
	UFUNCTION()
	void AfterAnimEndedAttack();

	void TraceHit();
private:
	TObjectPtr<class ACMPlayer> HitPlayer;

// AI Interface
protected:
	FAICharacterAttackFinished OnAttackFinished;
	
	virtual float GetAIPatrolRadius();
	virtual float GetAIDetectRange();
	virtual float GetAIAttackRange();
	virtual float GetAITurnSpeed();

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished);
	virtual void AttackByAI();
};

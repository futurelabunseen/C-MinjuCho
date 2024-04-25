﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CMPlayer.h"

#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Animation/CMPlayerAnimInstance.h"
#include "Weapon/CMWeapon.h"
#include "Weapon/CMLineGun.h"
#include "Weapon/CMColorGun.h"

ACMPlayer::ACMPlayer()
{
	// Create a Camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("FX_Head"));
	//FirstPersonCameraComponent->SetupAttachment(RootComponent);
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(12.0f, -5.0f, 72.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Meshes/TwinBlast.TwinBlast'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
	
	GetMesh()->HideBoneByName(TEXT("head"), PBO_None);
	
	// Animation
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Twinblast_AnimBlueprint.Twinblast_AnimBlueprint_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to call anim instance class From Player"));
	}
	//GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));

	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/FirstPerson/Input/IMC_Default.IMC_Default'"));
	if (nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FirstPerson/Input/Actions/IA_Move.IA_Move'"));
	if (nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FirstPerson/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FirstPerson/Input/Actions/IA_Look.IA_Look'"));
	if (nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFireRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FirstPerson/Input/Actions/IA_Shoot.IA_Shoot'"));
	if (nullptr != InputActionFireRef.Object)
	{
		FireAction = InputActionFireRef.Object;
	}

	isLeft = EGunHandler::Right;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionConvertRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FirstPerson/Input/Actions/IA_Convert.IA_Convert'"));
	if (nullptr != InputActionConvertRef.Object)
	{
		ConvertAction = InputActionConvertRef.Object;
	}

	// Weapon Class
	// Right Weapon
	static ConstructorHelpers::FClassFinder<ACMLineGun> RightGunRef (TEXT("/Game/Blueprint/BP_LineGun.BP_LineGun_C"));
	if(RightGunRef.Class)
	{
		RightGunClass = RightGunRef.Class;
	}
	RightGun = CreateDefaultSubobject<ACMLineGun>(TEXT("RightGun"));
	
	// Left Weapon
	static ConstructorHelpers::FClassFinder<ACMColorGun> LeftGunRef (TEXT(""));
	if(LeftGunRef.Class)
	{
		LeftGunClass = LeftGunRef.Class;
	}
	LeftGun = CreateDefaultSubobject<ACMColorGun>(TEXT("LeftGun"));
	ArrayGun.Add(RightGun);
	ArrayGun.Add(LeftGun);
	
	// Custom Anim Instance
	PlayerAnimInstance = Cast<UCMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
}

void ACMPlayer::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	
	GetMesh()->HideBoneByName(TEXT("head"), PBO_None);
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Equip Weapon Mesh
	RightGun->SetPlayer(this);
	if (RightGun)
	{
		// Material Visibility = None
		// If you want change weapon Mesh, here
		// FVector SocketLocation = GetMesh()->GetSocketLocation(FName("Muzzle_R"));
		// WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		// WeaponMesh->SetRelativeLocation(WeaponRelativeLocation);
		// WeaponMesh->SetRelativeRotation(WeaponRotator);
		// WeaponMesh->CastShadow = true;
		// WeaponMesh->bCastHiddenShadow = true;
	}
	LeftGun->SetPlayer(this);
	if (LeftGun)
	{
		// Material Visibility = None
		// If you want change weapon Mesh, here
		// FVector SocketLocation = GetMesh()->GetSocketLocation(FName("Muzzle_R"));
		// WeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		// WeaponMesh->SetRelativeLocation(WeaponRelativeLocation);
		// WeaponMesh->SetRelativeRotation(WeaponRotator);
		// WeaponMesh->CastShadow = true;
		// WeaponMesh->bCastHiddenShadow = true;
	}
	Gun = ArrayGun[0];
}

void ACMPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACMPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACMPlayer::Look);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACMPlayer::Fire);

		// Converting Gun
		EnhancedInputComponent->BindAction(ConvertAction, ETriggerEvent::Triggered, this, &ACMPlayer::ConvertingGun);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACMPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.X);
		AddMovementInput(GetActorRightVector(), MovementVector.Y);
	}
}

void ACMPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACMPlayer::Fire()
{
	Gun->Fire();
	
}

void ACMPlayer::ConvertingGun()
{
	if(!PlayerAnimInstance)
	{
		PlayerAnimInstance = Cast<UCMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}
	if (PlayerAnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast anim instance"));
		return;
	}
	if(PlayerAnimInstance->PlayConverting((uint8)isLeft) == 1)
	{
		switch(isLeft)
		{
		case EGunHandler::Right:
			isLeft = EGunHandler::Left;
			break;
		case EGunHandler::Left:
			isLeft = EGunHandler::Right;
			break;
		}
		Gun = ArrayGun[(uint8)isLeft];
	}
	UE_LOG(LogTemp, Warning, TEXT("Converting Gun!: %d"), isLeft);
}
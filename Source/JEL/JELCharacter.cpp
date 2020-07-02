// Copyright Epic Games, Inc. All Rights Reserved.

#include <time.h>
#include <string>
#include "JELCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AJELCharacter::AJELCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 780.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,125.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f,180.f,0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->AspectRatio = (1920.f / 1080.f);
	SideViewCameraComponent->bConstrainAspectRatio = true;
	//SideViewCameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 2720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 1.7f;
	GetCharacterMovement()->AirControl = 1.00f;
	GetCharacterMovement()->JumpZVelocity = 880.f;
	GetCharacterMovement()->GroundFriction = 300.f;
	fDefaultMaxWalkSpeed = 250.f;
	GetCharacterMovement()->MaxWalkSpeed = fDefaultMaxWalkSpeed;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Configure dash movement
	fDashInputLeeway = 2.f;
	fDefaultMaxDashSpeed = 1000.f;
	lastLeftPress = time(NULL);
	lastRightPress = time(NULL);
	fLastMovementValue = 0;
	bMovementHeld = 0;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AJELCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AJELCharacter::MoveRight);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AJELCharacter::Drop);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AJELCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AJELCharacter::TouchStopped);
}

void AJELCharacter::MoveRight(float Value)
{
	bool bNewPress = 0;
	if (Value != 0 && !bMovementHeld) {
		bNewPress = 1;
	}

	if (Value > 0 && bNewPress) {
		GetCharacterMovement()->MaxWalkSpeed = fDefaultMaxWalkSpeed;
		if (difftime(time(NULL), lastRightPress) < fDashInputLeeway) {
			GetCharacterMovement()->MaxWalkSpeed = fDefaultMaxDashSpeed;
		}
		if (fLastMovementValue <= 0) {
			time(&lastRightPress);
		}
	}
	else if (Value < 0 && bNewPress) {
		GetCharacterMovement()->MaxWalkSpeed = fDefaultMaxWalkSpeed;
		if(difftime(time(NULL), lastLeftPress) < fDashInputLeeway) {
			GetCharacterMovement()->MaxWalkSpeed = fDefaultMaxDashSpeed;
		}
		if (fLastMovementValue >= 0) {
			time(&lastLeftPress);
		}
	}

	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
	fLastMovementValue = Value;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(fLastMovementValue));
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(difftime(time(NULL), lastRightPress)));
	
	bMovementHeld = 0;
	if (Value != 0) {
		bMovementHeld = 1;
	}
}

void AJELCharacter::Drop() {
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "trying to drop");
}

void AJELCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void AJELCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}


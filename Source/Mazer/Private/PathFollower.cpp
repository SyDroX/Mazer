// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFollower.h"
#include "GenericPlatform/GenericPlatformMath.h"

// Sets default values
APathFollower::APathFollower(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	Initialize();
	zPosition = 100;
	MoveSpeed = 50;
}

void APathFollower::Initialize()
{
	if (FollowerMeshComponent != NULL)
	{
		FollowerMeshComponent->DestroyComponent();
	}

	FollowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Follower"));
	FollowerMeshComponent->SetRelativeLocation(FVector().ZeroVector);
	FollowerMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

#if WITH_EDITOR
void APathFollower::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName propertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (propertyName == GET_MEMBER_NAME_CHECKED(APathFollower, FollowerMeshComponent)) 
	{
		//Initialize();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

// Called when the game starts or when spawned
void APathFollower::BeginPlay()
{
	Super::BeginPlay();
	Path = GridCreator->CalculateShortestPath(Source, Target);
	
	if (Path.Num() > 0)
	{
		CurrentPosition = FVector(Path[0].Y * GridCreator->NodeVerticalDistance, Path[0].X * GridCreator->NodeHorizontalDistance, zPosition);
		Path.RemoveAt(0);

		CurrentTargetNode = Path[0];
		CurrentTargetPosition = FVector(CurrentTargetNode.Y * GridCreator->NodeVerticalDistance, CurrentTargetNode.X * GridCreator->NodeHorizontalDistance, zPosition);

		PreviousDistance = 999999.0f;
		ReachedTarget = false;
		//UpdateTarget();
	}
}

void APathFollower::UpdateTarget() 
{
	if (Path.Num() > 0)
	{
		CurrentTargetNode = Path[0];
		CurrentTargetPosition = FVector(CurrentTargetNode.Y * GridCreator->NodeVerticalDistance, CurrentTargetNode.X * GridCreator->NodeHorizontalDistance, zPosition);	
		Path.RemoveAt(0);
		
	}
	else 
	{
		ReachedTarget = true;
	}
}

// Called every frame
void APathFollower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ReachedTarget)
	{
		SetActorTickEnabled(false);

		return;
	}
	
	DeltaTime = 0.033;
	
	float currentDistance = FVector::Distance(CurrentPosition, CurrentTargetPosition);

	if (currentDistance <= PreviousDistance)
	{
		CurrentPosition += FVector(CurrentTargetPosition - CurrentPosition).GetSafeNormal() * DeltaTime * MoveSpeed;
		CurrentPosition.Z = zPosition;
		RootComponent->SetWorldLocation(CurrentPosition);
		PreviousDistance = currentDistance;
	}
	else 
	{
		UpdateTarget();
		PreviousDistance = 999999.0f;
	}
}


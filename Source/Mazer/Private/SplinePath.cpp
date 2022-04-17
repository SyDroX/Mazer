#include "SplinePath.h"
#include "Components/SplineMeshComponent.h"

ASplinePath::ASplinePath(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>("Spline");

	if (IsValid(SplinePath))
	{
		SetRootComponent(SplinePath);
		MoveSpeed = 1;
	}

	FollowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("SplineFollower");
	FollowerMeshComponent->AttachToComponent(SplinePath, FAttachmentTransformRules::KeepRelativeTransform);
	FollowerMeshComponent->SetRelativeLocation(FVector().ZeroVector);
}

void ASplinePath::BeginPlay()
{
	Super::BeginPlay();
	SplinePoints = GridCreator->CalculateShortestPath(Source, Target);
	timeTraveled = 0;

	for (float i = 0; i < SplinePoints.Num(); ++i)
	{
		FSplinePoint point = FSplinePoint(i + 1, FVector(SplinePoints[i].Y * GridCreator->NodeVerticalDistance, 
														 SplinePoints[i].X * GridCreator->NodeHorizontalDistance, 0));
		SplinePath->AddPoint(point, true);
	}
}

void ASplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (timeTraveled <= SplinePath->GetSplineLength())
	{
		FVector currentPosition = SplinePath->GetLocationAtTime(timeTraveled, ESplineCoordinateSpace::Local, true);
		timeTraveled += DeltaTime * MoveSpeed;
		FollowerMeshComponent->SetRelativeLocation(currentPosition);
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

#include "SplinePath.h"
#include "Components/SplineMeshComponent.h"

ASplinePath::ASplinePath(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
	TimeTraveled = 0;

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
	
	if (TimeTraveled <= SplinePath->GetSplineLength())
	{
		const FVector currentPosition = SplinePath->GetLocationAtTime(TimeTraveled, ESplineCoordinateSpace::Local, true);
		TimeTraveled += DeltaTime * MoveSpeed;
		FollowerMeshComponent->SetRelativeLocation(currentPosition);
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

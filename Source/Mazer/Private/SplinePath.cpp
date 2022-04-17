#include "SplinePath.h"
#include "Components/SplineMeshComponent.h"

ASplinePath::ASplinePath()
{
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>("Spline");

	if (SplinePath)
	{
		SetRootComponent(SplinePath);
		USplineMeshComponent* splineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		splineMeshComponent->SetStaticMesh(FollowerMeshComponent);
		splineMeshComponent->SetMobility(EComponentMobility::Movable);
		splineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		splineMeshComponent->RegisterComponentWithWorld(GetWorld());
		splineMeshComponent->AttachToComponent(SplinePath, FAttachmentTransformRules::KeepRelativeTransform);
		
	}
}

void ASplinePath::BeginPlay()
{
	Super::BeginPlay();
	SplinePoints = GridCreator->CalculateShortestPath(Source, Target);

	for (float i = 0; i < SplinePoints.Num(); i++)
	{
		FSplinePoint point = FSplinePoint(i + 1, FVector(SplinePoints[i], 0));
		SplinePath->AddPoint(point);
	}
}

void ASplinePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#include "PathFollower.h"
#include "GenericPlatform/GenericPlatformMath.h"

APathFollower::APathFollower(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	Initialize();
}

void APathFollower::Initialize()
{
	FollowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Follower"));
	FollowerMeshComponent->SetRelativeLocation(FVector().ZeroVector);
	FollowerMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	zPosition = 100;
	MoveSpeed = 50;
}

#if WITH_EDITOR
void APathFollower::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName propertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (propertyName == GET_MEMBER_NAME_CHECKED(APathFollower, FollowerMeshComponent)) 
	{
		//Initialize();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void APathFollower::BeginPlay()
{
	Super::BeginPlay();
	Path = GridCreator->CalculateShortestPath(Source, Target);
	
	if (Path.Num() > 0)
	{
		CurrentPosition = FVector(Path[0].Y * GridCreator->NodeVerticalDistance,
								  Path[0].X * GridCreator->NodeHorizontalDistance,
								  zPosition);
		Path.RemoveAt(0);

		CurrentTargetNode = Path[0];
		CurrentTargetPosition = FVector(CurrentTargetNode.Y * GridCreator->NodeVerticalDistance,
										CurrentTargetNode.X * GridCreator->NodeHorizontalDistance,
										zPosition);

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
		CurrentTargetPosition = FVector(CurrentTargetNode.Y * GridCreator->NodeVerticalDistance,
										CurrentTargetNode.X * GridCreator->NodeHorizontalDistance,
										zPosition);	
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
	
	//DeltaTime = 0.033;

	const float currentDistance = FVector::Distance(CurrentPosition, CurrentTargetPosition);

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


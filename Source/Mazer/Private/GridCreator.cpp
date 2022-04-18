#include "GridCreator.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/UnrealType.h"
#include "GenericPlatform/GenericPlatformMath.h"

// Sets default values
AGridCreator::AGridCreator(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	GridWidth = 10;
	GridHeight = 10;
	NodeVerticalDistance = 125;
	NodeHorizontalDistance = 125;
	InitializeGrid();
	UpdateNodes();
	SetBlockedRegions();
	//CreateBlockedCenter();
}

// Called when the game starts or when spawned
void AGridCreator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGridCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void AGridCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName propertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TArray<FName> changedProps;
	changedProps.Reserve(7);
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, WalkableNodeMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, NonWalkableNodeMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, NodeMesh));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, BlockedRegions));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FBlockedRegion, Radius));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FVector2D, X));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FVector2D, Y));

	if (changedProps.Contains(propertyName))
	{
		UpdateNodes();
		//CreateBlockedCenter();
		SetBlockedRegions();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AGridCreator::InitializeGrid()
{
	for (int i = 0; i < PathGrid.Num(); ++i)
	{
		if (IsValid(PathGrid[i]))
		{
			PathGrid[i]->DestroyComponent();
		}
	}

	PathGrid.Empty();
	PathGrid.Reserve(GridHeight * GridWidth);

	for (int i = 0; i < GridHeight; ++i)
	{
		for (int j = 0; j < GridWidth; ++j)
		{
			const FName NodeName = FName("GridNode", i * GridHeight + j);
			UStaticMeshComponent* currentNodeMesh = CreateDefaultSubobject<UStaticMeshComponent>(NodeName);

			if (IsValid(currentNodeMesh))
			{
				const FVector nodePosition = FVector(j * NodeHorizontalDistance, i * NodeVerticalDistance, 0);
				currentNodeMesh->SetRelativeLocation(nodePosition);
				PathGrid.Emplace(currentNodeMesh);
				currentNodeMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
			}
		}
	}
}

void AGridCreator::UpdateNodes()
{
	if (IsValid(WalkableNodeMaterial) && IsValid(NodeMesh))
	{
		for (int i = 0; i < PathGrid.Num(); ++i)
		{
			if (IsValid(PathGrid[i]))
			{
				PathGrid[i]->SetMaterial(0, WalkableNodeMaterial);
				PathGrid[i]->SetStaticMesh(NodeMesh);
			}
		}
	}
}

void AGridCreator::CreateBlockedCenter()
{
	const int centerX = GridWidth % 2 == 0 ? GridWidth / 2 - 1 : GridWidth / 2;
	const int centerY = GridHeight % 2 == 0 ? GridHeight / 2 - 1 : GridHeight / 2;

	for (int i = centerY; i <= GridHeight / 2; ++i)
	{
		for (int j = centerX; j <= GridWidth / 2; ++j)
		{
			if (i * GridHeight + j < PathGrid.Num())
			{
				PathGrid[i * GridHeight + j]->SetMaterial(0, NonWalkableNodeMaterial);
			}
		}
	}
}

void AGridCreator::SetBlockedRegions()
{
	if (IsValid(NonWalkableNodeMaterial) && PathGrid.Num() > 0)
	{
		for (int i = 0; i < BlockedRegions.Num(); ++i)
		{
			for (int j = FGenericPlatformMath::Max(static_cast<int>(BlockedRegions[i].Center.Y) - BlockedRegions[i].Radius, 0);
					j <= FGenericPlatformMath::Min(static_cast<int>(BlockedRegions[i].Center.Y) + BlockedRegions[i].Radius, GridHeight - 1); ++j)
			{
				for (int k = FGenericPlatformMath::Max(static_cast<int>(BlockedRegions[i].Center.X) - BlockedRegions[i].Radius, 0);
						k <= FGenericPlatformMath::Min(static_cast<int>(BlockedRegions[i].Center.X) + BlockedRegions[i].Radius, GridWidth - 1); ++k)
				{
					if (FGenericPlatformMath::Pow(BlockedRegions[i].Center.Y - j, 2) +
						FGenericPlatformMath::Pow(BlockedRegions[i].Center.X - k, 2) <=
						FGenericPlatformMath::Pow(BlockedRegions[i].Radius + 0.5, 2))
					{
						if (IsValid(PathGrid[j * GridHeight + k]))
						{
							PathGrid[j * GridHeight + k]->SetMaterial(0, NonWalkableNodeMaterial);
						}
					}
				}
			}
		}
	}
}

TArray<FVector2D> AGridCreator::CalculateShortestPath(const FVector2D source, const FVector2D target) const
{
	TArray<int> previousNodes;
	TArray<float> distances;
	TArray<bool> visitedNodes;

	distances.Reserve(GridHeight * GridWidth);
	previousNodes.Reserve(GridHeight * GridWidth);
	visitedNodes.Reserve(GridHeight * GridWidth);

	for (int i = 0; i < PathGrid.Num(); ++i)
	{
		distances.Add(GridHeight * GridWidth);
		previousNodes.Add(NULL);

		if (PathGrid[i]->GetMaterial(0) == WalkableNodeMaterial)
		{
			visitedNodes.Add(false);
		}
		else
		{
			visitedNodes.Add(true);
		}
	}

	const int sourceIndex = source.Y * GridHeight + source.X;
	distances[sourceIndex] = 0;
	FVector2D currentNode = source;

	while (AnyUnvisited(visitedNodes) == true && currentNode != target)
	{
		const int currentNodeIndex = currentNode.Y * GridHeight + currentNode.X;
		TArray<FVector2D> neighbors = GetNeighbors(currentNode, visitedNodes);

		for (int i = 0; i < neighbors.Num(); ++i)
		{
			const int currentNeighborIndex = neighbors[i].Y * GridHeight + neighbors[i].X;
			const float currentDistance = GetDistance(currentNode, neighbors[i]) + distances[currentNodeIndex];

			if (distances[currentNeighborIndex] > currentDistance)
			{
				distances[currentNeighborIndex] = currentDistance;
				previousNodes[currentNeighborIndex] = currentNodeIndex;
			}
		}

		visitedNodes[currentNodeIndex] = true;
		currentNode = GetMinDistanceUnvisitedNode(distances, visitedNodes);
	}

	TArray<FVector2D> path;

	if (currentNode == target)
	{
		int nodeIndex = currentNode.Y * GridHeight + currentNode.X;
		//PathGrid[nodeIndex]->SetMaterial(0, PathMaterial);
		path.Emplace(target);

		while (nodeIndex != sourceIndex)
		{
			nodeIndex = previousNodes[nodeIndex];
			//PathGrid[nodeIndex]->SetMaterial(0, PathMaterial);
			path.EmplaceAt(0, FVector2D(nodeIndex / GridHeight, nodeIndex % GridWidth));
		}
	}

	return path;
}

bool AGridCreator::AnyUnvisited(const TArray<bool> graph) const
{
	for (int i = 0; i < graph.Num(); ++i)
	{
		if (graph[i] == false)
		{
			return true;
		}
	}

	return false;
}

float AGridCreator::GetDistance(const FVector2D u, const FVector2D v) const
{
	if (FGenericPlatformMath::Abs(u.X - v.X) == 1 && FGenericPlatformMath::Abs(u.Y - v.Y) == 1)
	{
		return FGenericPlatformMath::Sqrt(2);
	}
	
	return 1;
}

TArray<FVector2D> AGridCreator::GetNeighbors(const FVector2D source, const TArray<bool> graph) const
{
	TArray<FVector2D> neighbors;

	for (int y = FGenericPlatformMath::Max(static_cast<int>(source.Y) - 1, 0); y <=
				 FGenericPlatformMath::Min(static_cast<int>(source.Y) + 1, GridHeight - 1); ++y)
	{
		for (int x = FGenericPlatformMath::Max(static_cast<int>(source.X) - 1, 0); x <=
					 FGenericPlatformMath::Min(static_cast<int>(source.X) + 1, GridWidth - 1); ++x)
		{
			if (!graph[y * GridHeight + x] && (x != source.X || y != source.Y))
			{
				neighbors.Add(FVector2D(x, y));
			}
		}
	}

	return neighbors;
}

FVector2D AGridCreator::GetMinDistanceUnvisitedNode(const TArray<float> distances, const TArray<bool> visitedNodes) const
{
	float minDistance = GridHeight * GridWidth;
	FVector2D minDistanceNode;

	for (int i = 0; i < distances.Num(); ++i)
	{
		if (!visitedNodes[i] && distances[i] < minDistance)
		{
			minDistanceNode.X = i % GridWidth;
			minDistanceNode.Y = i / GridHeight;
			minDistance = distances[i];
		}
	}

	return minDistanceNode;
}

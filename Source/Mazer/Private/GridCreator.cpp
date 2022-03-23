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
	//CreateBlockedCenter();
}
	
// Called when the game starts or when spawned
void AGridCreator::BeginPlay()
{
	Super::BeginPlay();

	CalculateShortestPath();
}


// Called every frame
void AGridCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) 
{
	FName propertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TArray<FName> changedProps;
	changedProps.Reserve(7);
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, WalkableNodeMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, NonWalkableNodeMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, NodeMesh));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, BlockedRegions));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FBlockedRegion, radius));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FVector2D, X));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FVector2D, Y));
	
	if (changedProps.Contains(propertyName))
	{
		InitializeGrid();
		//CreateBlockedCenter();
		SetBlockedRegions();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

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

	if (IsValid(NodeMesh) && IsValid(WalkableNodeMaterial))
	{
		for (int i = 0; i < GridHeight; ++i)
		{
			for (int j = 0; j < GridWidth; ++j)
			{
				FName NodeName = FName(TEXT("GridNode"), i * GridHeight + j);
				UStaticMeshComponent* currentNodeMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), NodeName);

				if (IsValid(currentNodeMesh))
				{
					currentNodeMesh->SetStaticMesh(NodeMesh);
					currentNodeMesh->SetMaterial(0, WalkableNodeMaterial);
					FVector nodePosition = FVector(j * 125, i * 125, 0);
					currentNodeMesh->SetRelativeLocation(nodePosition);
					PathGrid.Emplace(currentNodeMesh);
					currentNodeMesh->RegisterComponent();
					currentNodeMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
				}
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
	int centerX = GridWidth % 2 == 0 ? GridWidth / 2 - 1 : GridWidth / 2;
	int centerY = GridHeight % 2 == 0 ? GridHeight / 2 - 1 : GridHeight / 2;

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
	if (IsValid(NonWalkableNodeMaterial) && IsValid(NodeMesh))
	{
		for (int i = 0; i < BlockedRegions.Num(); ++i)
		{
			for (int j = FGenericPlatformMath::Max((int)BlockedRegions[i].center.Y - BlockedRegions[i].radius, 0);
				j <= FGenericPlatformMath::Min((int)BlockedRegions[i].center.Y + BlockedRegions[i].radius, GridHeight - 1); ++j)
			{
				for (int k = FGenericPlatformMath::Max((int)BlockedRegions[i].center.X - BlockedRegions[i].radius, 0);
					k <= FGenericPlatformMath::Min((int)BlockedRegions[i].center.X + BlockedRegions[i].radius, GridWidth - 1); ++k)
				{
					if (FGenericPlatformMath::Pow(BlockedRegions[i].center.Y - j, 2) + FGenericPlatformMath::Pow(BlockedRegions[i].center.X - k, 2) <= FGenericPlatformMath::Pow(BlockedRegions[i].radius, 2))
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

void AGridCreator::CalculateShortestPath() 
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

	distances[Source.Y * GridHeight + Source.X] = 0;
	FVector2D currentNode = Source;

	while (AnyUnvisited(visitedNodes) == true && currentNode != Target)
	{
		int currentNodeIndex = currentNode.Y * GridHeight + currentNode.X;
		FVector2D minDistanceNeighbor;
		TArray<FVector2D> neighbors = GetNeighbors(currentNode, visitedNodes);

		for (int i = 0; i < neighbors.Num(); ++i)
		{
			int currentNeighborIndex = neighbors[i].Y * GridHeight + neighbors[i].X;
			
			float currentDistance = GetDistance(currentNode, neighbors[i]) + distances[currentNodeIndex];
			
			if (distances[currentNeighborIndex] > currentDistance)
			{
				distances[currentNeighborIndex] = currentDistance;
				previousNodes[currentNeighborIndex] = currentNodeIndex;
			}
		}

		visitedNodes[currentNodeIndex] = true;
		currentNode = GetMinDistanceUnvisitedNode(distances,visitedNodes);
	}
	
	if (currentNode == Target)
	{
		int sourceIndex = Source.Y * GridHeight + Source.X;
		int nodeIndex = currentNode.Y * GridHeight + currentNode.X;
		PathGrid[nodeIndex]->SetMaterial(0, PathMaterial);

		while (nodeIndex != sourceIndex)
		{
			nodeIndex = previousNodes[nodeIndex];
			PathGrid[nodeIndex]->SetMaterial(0, PathMaterial);
		}
	}
}

bool AGridCreator::AnyUnvisited(TArray<bool> graph) 
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

float AGridCreator::GetDistance(FVector2D u, FVector2D v)
{
	if (FGenericPlatformMath::Abs(u.X - v.X) == 1 && FGenericPlatformMath::Abs(u.Y - v.Y) == 1)
	{
		return FGenericPlatformMath::Sqrt(2);
	}
	else 
	{
		return 1;
	}
}

TArray<FVector2D> AGridCreator::GetNeighbors(FVector2D source, TArray<bool> graph)
{
	TArray<FVector2D> neighbors;

	for (int y = FGenericPlatformMath::Max((int)source.Y - 1, 0); y <= FGenericPlatformMath::Min((int)source.Y + 1, GridHeight -1); ++y)
	{
		for (int x = FGenericPlatformMath::Max((int)source.X - 1, 0); x <= FGenericPlatformMath::Min((int)source.X + 1, GridWidth -1); ++x)
		{
			if (!graph[y * GridHeight + x] && (x != source.X || y != source.Y))
			{
				neighbors.Add(FVector2D(x, y));
			}
		}
	}

	return neighbors;
}

FVector2D AGridCreator::GetMinDistanceUnvisitedNode(TArray<float> distances, TArray<bool> visitedNodes) 
{
	float minDistance = GridHeight * GridWidth;
	FVector2D minDistanceNode;

	for (int i = 0; i < distances.Num(); ++i)
	{
		if (!visitedNodes[i] && distances[i] < minDistance)
		{
			minDistanceNode.X = i % GridHeight;
			minDistanceNode.Y = i / GridHeight;
			minDistance = distances[i];
		}
	}

	return minDistanceNode;
}
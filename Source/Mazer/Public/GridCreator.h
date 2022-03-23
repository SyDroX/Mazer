// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCreator.generated.h"

USTRUCT()
struct FGraphNode
{
	GENERATED_BODY()
public:
	FVector2D position;
	bool visited;
	float distance;
	FVector2D previousNode;
};

USTRUCT(BlueprintType)
struct FBlockedRegion
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FVector2D center;

	UPROPERTY(EditAnywhere)
	int radius;
};

UCLASS()
class MAZER_API AGridCreator : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AGridCreator(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridWidth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMesh* NodeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UMaterial* WalkableNodeMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UMaterial* NonWalkableNodeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UMaterial* PathMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TArray<UStaticMeshComponent*> PathGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FBlockedRegion> BlockedRegions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TArray<FVector2D> Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector2D Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector2D Target;

protected:
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void InitializeGrid();
	void CreateBlockedCenter();
	void UpdateNodes();
	void SetBlockedRegions();
	void CalculateShortestPath();
	bool AnyUnvisited(TArray<bool> graph);
	float GetDistance(FVector2D u, FVector2D v);
	FVector2D GetMinDistanceUnvisitedNode(TArray<float> distances, TArray<bool> visitedNodes);
	TArray<FVector2D> GetNeighbors(FVector2D source, TArray<bool> graph);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

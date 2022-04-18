// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCreator.generated.h"

USTRUCT(BlueprintType)
struct FBlockedRegion
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FVector2D Center;

	UPROPERTY(EditAnywhere)
	int Radius;
};

UCLASS()
class MAZER_API AGridCreator : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AGridCreator(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridWidth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 NodeHorizontalDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 NodeVerticalDistance;

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

protected:
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void InitializeGrid();
	void CreateBlockedCenter();
	void UpdateNodes();
	void SetBlockedRegions();
	bool AnyUnvisited(TArray<bool> graph) const;
	float GetDistance(FVector2D u, FVector2D v) const;
	FVector2D GetMinDistanceUnvisitedNode(TArray<float> distances, TArray<bool> visitedNodes) const;
	TArray<FVector2D> GetNeighbors(FVector2D source, TArray<bool> graph) const;

public:	
	virtual void Tick(float DeltaTime) override;
	TArray<FVector2D> CalculateShortestPath(FVector2D source, FVector2D target) const;
};

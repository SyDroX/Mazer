// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCreator.generated.h"

USTRUCT(BlueprintType)
struct FBlockedRegion
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int column;

	UPROPERTY(EditAnywhere)
	int row;

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
	UStaticMesh* CellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UMaterial* CellWalkableMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UMaterial* CellNonWalkableMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TArray<UStaticMeshComponent*> PathGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TArray<FBlockedRegion> BlockedRegions;

protected:
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void InitializeGrid();
	void CreateBlockedCenter();
	void UpdateCells();
	void SetBlockedRegions();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

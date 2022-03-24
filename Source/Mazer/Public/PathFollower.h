// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCreator.h"
#include "PathFollower.generated.h"

UCLASS()
class MAZER_API APathFollower : public AActor
{
	GENERATED_BODY()
private:
	FVector CurrentPosition;
	FVector CurrentTargetPosition;
	FVector2D CurrentTargetNode;
	float zPosition;
	float PreviousDistance;
	bool ReachedTarget;

public:	
	// Sets default values for this actor's properties
	APathFollower(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridCreator* GridCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	TArray<FVector2D> Path;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent* FollowerMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
	float MoveSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void Initialize();
	void UpdateTarget();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

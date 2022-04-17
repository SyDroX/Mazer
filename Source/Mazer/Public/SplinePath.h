// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "GridCreator.h"
#include "SplinePath.generated.h"

UCLASS()
class MAZER_API ASplinePath : public AActor
{
	GENERATED_BODY()
private:
	TArray<FVector2D> SplinePoints;
public:	
	// Sets default values for this actor's properties
	ASplinePath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline")
	USplineComponent* SplinePath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Path")
	AGridCreator* GridCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMesh* FollowerMeshComponent;
};

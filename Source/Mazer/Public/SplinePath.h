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
	
	float TimeTraveled;
	TArray<FVector2D> SplinePoints;
public:	
	ASplinePath(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline")
	USplineComponent* SplinePath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
	AGridCreator* GridCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent* FollowerMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
	float MoveSpeed;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};

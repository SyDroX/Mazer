#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCreator.h"
#include "PathFollower.generated.h"

UCLASS()
class MAZER_API APathFollower : public AActor
{
	GENERATED_BODY()
	FVector PreviousPosition;
	FVector CurrentPosition;
	FVector CurrentTargetPosition;
	FVector2D CurrentTargetNode;
	float zPosition;
	float PreviousDistance;
	bool ReachedTarget;
	TArray<FVector2D> Path;

public:	
	APathFollower(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	AGridCreator* GridCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FVector2D Target;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent* FollowerMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
	float MoveSpeed;

protected:
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void Initialize();
	void UpdateTarget();
	
public:	
	virtual void Tick(float DeltaTime) override;
};

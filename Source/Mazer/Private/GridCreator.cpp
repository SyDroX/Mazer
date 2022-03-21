// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCreator.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/UnrealType.h"

// Sets default values
AGridCreator::AGridCreator(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	GridWidth = 10;
	GridHeight = 10;
}
	
// Called when the game starts or when spawned
void AGridCreator::BeginPlay()
{
	Super::BeginPlay();
}

void AGridCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) 
{
	FName propertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TArray<FName> changedProps;
	changedProps.Reserve(3);
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, GridHeight));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, GridWidth));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, PathGrid));

	if (changedProps.Contains(propertyName))
	{
		InitializeGrid();
		CreateBlockedCenter();
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

	for (int i = 0; i < GridHeight; ++i)
	{
		for (int j = 0; j < GridWidth; ++j)
		{
			FName cellName = FName(TEXT("GridCell"), i * GridHeight + j);
			UStaticMeshComponent* currentCellMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), cellName);

			if (IsValid(CellMesh) && IsValid(currentCellMesh))
			{
				currentCellMesh->SetStaticMesh(CellMesh);
				currentCellMesh->SetMaterial(0, CellWalkableMaterial);
				FVector cellPos = FVector(j * 125, i * 125, 0);
				currentCellMesh->SetRelativeLocation(cellPos);
				PathGrid.Emplace(currentCellMesh);
				currentCellMesh->RegisterComponent();
				currentCellMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
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
				PathGrid[i * GridHeight + j]->SetMaterial(0, NonCellWalkableMaterial);
			}
		}
	}
}

// Called every frame
void AGridCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


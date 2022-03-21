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

	InitializeGrid();
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

void AGridCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) 
{
	FName propertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	TArray<FName> changedProps;
	changedProps.Reserve(7);
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, CellWalkableMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, CellNonWalkableMaterial));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, CellMesh));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(AGridCreator, BlockedRegions));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FBlockedRegion, radius));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FBlockedRegion, row));
	changedProps.Emplace(GET_MEMBER_NAME_CHECKED(FBlockedRegion, column));

	if (changedProps.Contains(propertyName))
	{
		UpdateCells();
		CreateBlockedCenter();
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

	for (int i = 0; i < GridHeight; ++i)
	{
		for (int j = 0; j < GridWidth; ++j)
		{
			FName cellName = FName(TEXT("GridCell"), i * GridHeight + j);
			UStaticMeshComponent* currentCellMesh = CreateDefaultSubobject<UStaticMeshComponent>(cellName);

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

void AGridCreator::UpdateCells()
{
	for (int i = 0; i < PathGrid.Num(); ++i)
	{
		PathGrid[i]->SetMaterial(0, CellWalkableMaterial);
		PathGrid[i]->SetStaticMesh(CellMesh);
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
				PathGrid[i * GridHeight + j]->SetMaterial(0, CellNonWalkableMaterial);
			}
		}
	}
}

void AGridCreator::SetBlockedRegions()
{
	for (int i = 0; i < BlockedRegions.Num(); ++i)
	{
		for (int j = FGenericPlatformMath::Max(BlockedRegions[i].row - BlockedRegions[i].radius, 0);
			j <= FGenericPlatformMath::Min(BlockedRegions[i].row + BlockedRegions[i].radius, GridHeight - 1); ++j)
		{
			for (int k = FGenericPlatformMath::Max(BlockedRegions[i].column - BlockedRegions[i].radius, 0);
				k <= FGenericPlatformMath::Min(BlockedRegions[i].column + BlockedRegions[i].radius, GridWidth - 1); ++k)
			{
				if (FGenericPlatformMath::Pow(BlockedRegions[i].row - j, 2) + FGenericPlatformMath::Pow(BlockedRegions[i].column - k, 2) <= FGenericPlatformMath::Pow(BlockedRegions[i].radius, 2))
				{
					PathGrid[j * GridHeight + k]->SetMaterial(0, CellNonWalkableMaterial);
				}
			}
		}
	}
}

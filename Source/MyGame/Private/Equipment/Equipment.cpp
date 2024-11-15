// Fill out your copyright notice in the Description page of Project Settings.

#include "./Equipment/Equipment.h"
#include "./Equipment/EquipmentStruct.h"

// Sets default values
AEquipment::AEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh Component"));
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTableObj(TEXT("DataTable'/Game/EquipmentSystem/DB_Equipment.DB_Equipment'"));
    if (DataTableObj.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Data table loaded"));
        DataTable = DataTableObj.Object;
    }
}

void AEquipment::ChangeData(int Index)
{
    this->DataIndex = Index;
    const FString ContextString(TEXT("GENERAL"));
    TArray<FEquipmentStruct*> Rows;
    DataTable->GetAllRows(ContextString, Rows);
    this->StaticMeshComponent->SetStaticMesh(Rows[Index]->StaticMesh);
}



// Called when the game starts or when spawned
void AEquipment::BeginPlay()
{
	Super::BeginPlay();
    if (IsValid(DataTable))
    {
        const FString ContextString(TEXT("GENERAL"));
        TArray<FEquipmentStruct*> Rows;
        DataTable->GetAllRows(ContextString,Rows);
        StaticMeshComponent->SetStaticMesh(Rows[0]->StaticMesh);
        DataIndex = 0;
    }
}

// Called every frame
void AEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


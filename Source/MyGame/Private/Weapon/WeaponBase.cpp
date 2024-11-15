
#include "./Weapon/WeaponBase.h"

UWeaponBase::UWeaponBase()
{
	//PrimaryComponentTick.bCanEverTick = true;

}

void UWeaponBase::OnEquipped()
{
	Super::OnEquipped();
}

void UWeaponBase::OnUnequipped()
{
	Super::OnUnequipped();
}

//void UWeaponBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	
//}

// Called when the game starts or when spawned
//void UWeaponBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}






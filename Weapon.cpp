

#include "Weapon.h"

AWeapon::AWeapon() :
	ThrowWeaponTime(0.7f),
	bFalling(false),
	Ammo(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	// 무기버릴때속도정의
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(
		ThrowWeaponTimer,
		this,
		&AWeapon::StopFalling,
		ThrowWeaponTime);

}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_Pickup);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"


AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),

	//�����ۺ����ʱ�ȭ
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f)
{
	//ƽ���پ��ͼ���
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	//�������������̽����ܵα� sphere�� ���� ���̰�
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	//�����۵�޿뺰
	SetActiveStars();

	//sphere�� ���� ���̰�
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//�����ۼӼ�����
	SetItemProperties(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	//�����۵�޼���.1���ͽ����ϰ�
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{//��޺� �� Ȱ��ȭ
		case EItemRarity::EIR_Damaged:
			ActiveStars[1] = true;
			break;
		case EItemRarity::EIR_Common:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			break;
		case EItemRarity::EIR_Uncommon:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			break;
		case EItemRarity::EIR_Rare:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			break;
		case EItemRarity::EIR_Legendary:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			ActiveStars[5] = true;
			break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{//�����ۻ��º����Ǽ���
		case EItemState::EIS_Pickup:
			//�����۸Žü���
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������̽��뱸ü��������
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//�������浹����
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(
				ECollisionChannel::ECC_Visibility,
				ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		case EItemState::EIS_Equipped:
			PickupWidget->SetVisibility(false);
			//�����۸Žü���
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������̽��뱸ü��������
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������浹����
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_Falling:
			//�����۸Žü���
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemMesh->SetSimulatePhysics(true);
			ItemMesh->SetEnableGravity(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionResponseToChannel(
				ECollisionChannel::ECC_WorldStatic,
				ECollisionResponse::ECR_Block);
			//�������̽��뱸ü��������
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������浹����
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		case EItemState::EIS_EquipInterping:
			PickupWidget->SetVisibility(false);
			//�����۸Žü���
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetEnableGravity(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������̽��뱸ü��������
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�������浹����
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		Character->GetPickupItem(this);
	}
	
	SetActorScale3D(FVector(1.f));
}

void AItem::ItemInterp(float DeltaTime)//�����۵��ø���Ȯ��
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		UE_LOG(LogTemp, Warning, TEXT("CurveValue: %f"), CurveValue);
		FVector ItemLocation = ItemInterpStartLocation;
		const FVector CameraInterpLocation{ Character->GetCameraInterpLocation() };

		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X,
			CameraInterpLocation.X,
			DeltaTime,
			30.0f);
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.f);

		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ItemInterp(DeltaTime);
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AShooterCharacter* Char)
{
	
	Character = Char;
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&AItem::FinishInterping,
		ZCurveTime);

	//ī�޶�RotationYaw���ϱ�
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	//������yaw���ϱ�
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	//ī�޶�������ۻ���offsetyaw���ϱ�
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

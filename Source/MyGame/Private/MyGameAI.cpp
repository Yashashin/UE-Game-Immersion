#include "MyGameAI.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyGameCharacter.h"
#include "AIAttackSystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#ifndef MYGAMEAI_MACRO
#define RAG_DOLL_DEALY_TIME 1.9f
#define ASSASSINATE_RADIUS 35.0f
#define HIT_LAUNCH_VELOCITY 700.0f
#define ASSASSINATE_WIDGET_SCALE 0.375f
#define INVESTIGATE_WIDGET_SCALE 0.05f
#define Default_MAX_WALK_SPEED 450.0f
#endif // !MY_GAME_AI_MACRO

// Sets default values
AMyGameAI::AMyGameAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(TEXT("Enemy"));
	GetCharacterMovement()->MaxWalkSpeed = Default_MAX_WALK_SPEED;

	AssassinateRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Assassinate Radius"));
	AssassinateRadius->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	AssassinateRadius->InitSphereRadius(ASSASSINATE_RADIUS);
	AssassinateRadius->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	AssassinateRadius->SetRelativeLocation(FVector(-40.0f, 0.0f, 30.0f));
	AssassinateRadius->OnComponentBeginOverlap.AddDynamic(this, &AMyGameAI::OnOverlapBegin);
	AssassinateRadius->OnComponentEndOverlap.AddDynamic(this, &AMyGameAI::OnOverlapEnd);
	
	AssassinateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Assassinate Widget Component"));
	AssassinateWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	AssassinateWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	AssassinateWidgetComponent->SetRelativeScale3D(FVector(ASSASSINATE_WIDGET_SCALE));
	AssassinateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//AssassinateWidgetComponent->SetTickMode(ETickMode::Disabled);
	AssassinateWidgetComponent->SetVisibility(false);


	InvestigateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Investigate Widget Component"));
	InvestigateWidgetComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	InvestigateWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	InvestigateWidgetComponent->SetRelativeScale3D(FVector(INVESTIGATE_WIDGET_SCALE));
	InvestigateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//AssassinateWidgetComponent->SetTickMode(ETickMode::Disabled);
	InvestigateWidgetComponent->SetVisibility(false);

	AIAttackSystem = CreateDefaultSubobject<UAIAttackSystem>(TEXT("AI Attack System"));
	AIAttackSystem->Init();
}

void AMyGameAI::Die()
{
	AssassinateWidgetComponent->SetVisibility(false);
	AssassinateRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyGameAI::RagDoll()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			GetMesh()->SetSimulatePhysics(true);
		}, RAG_DOLL_DEALY_TIME, false);
}

void AMyGameAI::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyGameCharacter* MyGameCharacter = Cast<AMyGameCharacter>(OtherActor);
	if (IsValid(MyGameCharacter))
	{
		AssassinateWidgetComponent->SetVisibility(true);
	}
}

void AMyGameAI::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyGameCharacter* MyGameCharacter = Cast<AMyGameCharacter>(OtherActor);
	if (IsValid(MyGameCharacter))
	{
		AssassinateWidgetComponent->SetVisibility(false);
	}
}

void AMyGameAI::SetInvestigateWidgetVisible()
{
	InvestigateWidgetComponent->SetVisibility(true);
}

void AMyGameAI::SetInvestigateWidgetHidden()
{
	InvestigateWidgetComponent->SetVisibility(false);
}



// Called when the game starts or when spawned
void AMyGameAI::BeginPlay()
{
	Super::BeginPlay();

	AssassinateWidgetComponent->SetWidgetClass(AssassinateWidgetClass);
	InvestigateWidgetComponent->SetWidgetClass(InvestigateWidgetClass);
}

// Called every frame
void AMyGameAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyGameAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMyGameAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	int Rand=FMath::RandRange(0, HitAnims.Num() - 1);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(HitAnims[Rand]);
	AActor* Actor=UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector LaunchVelocity = Actor->GetActorForwardVector() * HIT_LAUNCH_VELOCITY;
	LaunchCharacter(LaunchVelocity,false,false);
	return DamageAmount;
}

void AMyGameAI::StealthBackAssassinate(FVector& RefLocation, FRotator& AssassinateRotation)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(RagDollAnim);
	Die();
	RagDoll();
	RefLocation = GetActorLocation() + GetActorForwardVector() * FVector(-78, 47, -88);// GetActorRotation().Quaternion()* FVector(-64, 25, -89);
	AssassinateRotation = GetActorRotation();
}

float AMyGameAI::Attack_Implementation()
{
	AIAttackSystem->Attack();
	float Duration = AttackAnim->GetPlayLength();
	return 0.4f;
}



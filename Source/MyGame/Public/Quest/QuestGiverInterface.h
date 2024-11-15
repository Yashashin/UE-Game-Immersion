#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuestGiverInterface.generated.h"

UINTERFACE(MinimalAPI)
class UQuestGiver : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MYGAME_API IQuestGiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GiveQuest();
};
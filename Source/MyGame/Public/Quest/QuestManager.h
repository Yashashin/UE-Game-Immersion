// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestManager.generated.h"

struct FQuestData;

/**
 * 
 */
UCLASS()
class MYGAME_API UQuestManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Quest")
	TArray<FQuestData> ActiveQuests;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FQuestData> CompletedQuests;

public:
	UQuestManager();

	UFUNCTION(Category = "Quest")
	void AddQuest(const FQuestData& NewQuest);

	UFUNCTION(Category = "Quest")
	void CompleteQuest(int32 QuestID);

	UFUNCTION(Category = "Quest")
	void CompleteObjective(int32 QuestID, int32 ObjectiveIndex);

	//UFUNCTION(Category= "Quest")
private:
	bool FindQuestByID(FQuestData &Quest,int32 QuestID);
};

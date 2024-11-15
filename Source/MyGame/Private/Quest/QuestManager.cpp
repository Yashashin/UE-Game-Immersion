#include "./Quest/QuestManager.h"
#include "./Quest/QuestStruct.h"


UQuestManager::UQuestManager()
{
}

void UQuestManager::AddQuest(const FQuestData& NewQuest)
{
	//ActiveQuests.Add(NewQuest);
}

void UQuestManager::CompleteQuest(int32 QuestID)
{
	/*FQuestData Quest;
	if (FindQuestByID(Quest, QuestID))
	{
		Quest.bIsCompleted = true;
		CompletedQuests.Add(Quest);
		ActiveQuests.RemoveSingle(Quest);
	}*/
}

void UQuestManager::CompleteObjective(int32 QuestID, int32 ObjectiveIndex)
{
	FQuestData Quest;
	if (FindQuestByID(Quest, QuestID) && Quest.Objectives.IsValidIndex(ObjectiveIndex))
	{
		Quest.Objectives[ObjectiveIndex].bIsCompleted = true;

		// Check if all objectives are completed
		bool bAllCompleted = true;
		for (const FQuestObjective& Objective : Quest.Objectives)
		{
			if (!Objective.bIsCompleted)
			{
				bAllCompleted = false;
				break;
			}
		}
		if (bAllCompleted)
		{
			CompleteQuest(QuestID);
		}
	}
}

bool UQuestManager::FindQuestByID(FQuestData& Quest, int32 QuestID)
{
	for (FQuestData& FindQuest : ActiveQuests)
	{
		if (FindQuest.ID == QuestID)
		{
			Quest = FindQuest;
			return true;
		}
	}
	return false;
}



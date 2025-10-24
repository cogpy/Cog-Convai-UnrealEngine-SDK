// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OpenCogExampleCharacter.generated.h"

/**
 * Example character demonstrating OpenCog integration with Convai
 * This serves as a reference implementation for creating autonomous agents
 */
UCLASS()
class CONVAI_API AOpenCogExampleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOpenCogExampleCharacter();

protected:
	virtual void BeginPlay() override;

public:
	/** OpenCog Agent Component - handles autonomous behavior */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenCog")
	class UOpenCogAgentComponent* OpenCogAgent;

	/** Convai Chatbot Component - handles conversation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Convai")
	class UConvaiChatbotComponent* ConvaiChatbot;

	/** Example: Configure as a patrol guard */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Examples")
	void ConfigureAsPatrolGuard();

	/** Example: Configure as a merchant */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Examples")
	void ConfigureAsMerchant();

	/** Example: Configure as a quest giver */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Examples")
	void ConfigureAsQuestGiver();

protected:
	/** Called when a behavior is executed by the agent */
	UFUNCTION()
	void OnBehaviorExecuted(class UOpenCogAgentComponent* Agent, const FString& BehaviorName);

	/** Called when a goal changes */
	UFUNCTION()
	void OnGoalChanged(class UOpenCogAgentComponent* Agent, const FOpenCogGoal& Goal);

	/** Called when a message is received from another agent */
	UFUNCTION()
	void OnMessageReceived(class UOpenCogAgentComponent* Agent, const FString& SenderID, const FOpenCogAgentMessage& Message);
};

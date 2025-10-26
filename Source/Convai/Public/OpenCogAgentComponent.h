// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OpenCogDefinitions.h"
#include "OpenCogAgentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoalChangedSignature, UOpenCogAgentComponent*, Agent, const FOpenCogGoal&, Goal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBehaviorExecutedSignature, UOpenCogAgentComponent*, Agent, const FString&, BehaviorName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMessageReceivedSignature, UOpenCogAgentComponent*, Agent, const FString&, SenderID, const FOpenCogAgentMessage&, Message);

/**
 * Component that makes an actor an autonomous OpenCog agent
 * Integrates with Convai for conversational capabilities
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent), DisplayName = "OpenCog Agent")
class CONVAI_API UOpenCogAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOpenCogAgentComponent();

	// Component interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Agent configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	FString AgentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	bool bEnableAutonomousBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	float BehaviorUpdateInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	float PerceptionRange;

	/** Current goals */
	UPROPERTY(BlueprintReadOnly, Category = "OpenCog|Goals")
	TArray<FOpenCogGoal> Goals;

	/** Behavior rules */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Behavior")
	TArray<FOpenCogBehaviorRule> BehaviorRules;

	/** Current active behavior */
	UPROPERTY(BlueprintReadOnly, Category = "OpenCog|Behavior")
	FString ActiveBehavior;

	/** Knowledge atoms this agent is aware of */
	UPROPERTY(BlueprintReadOnly, Category = "OpenCog|Knowledge")
	TArray<FString> KnowledgeAtomIDs;

	/** Current attention target */
	UPROPERTY(BlueprintReadOnly, Category = "OpenCog|Perception")
	AActor* AttentionTarget;

	/**
	 * Add a goal for this agent
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	void AddGoal(const FOpenCogGoal& Goal);

	/**
	 * Remove a goal by ID
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	bool RemoveGoal(const FString& GoalID);

	/**
	 * Get goal by ID
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	bool GetGoal(const FString& GoalID, FOpenCogGoal& OutGoal) const;

	/**
	 * Update goal satisfaction level
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	void UpdateGoalSatisfaction(const FString& GoalID, float Satisfaction);

	/**
	 * Get the highest priority active goal
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	bool GetTopPriorityGoal(FOpenCogGoal& OutGoal) const;

	/**
	 * Add a behavior rule
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Behavior")
	void AddBehaviorRule(const FOpenCogBehaviorRule& Rule);

	/**
	 * Remove a behavior rule
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Behavior")
	bool RemoveBehaviorRule(const FString& RuleID);

	/**
	 * Evaluate which behavior to execute based on goals and context
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Behavior")
	FString SelectBehavior();

	/**
	 * Execute a specific behavior
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Behavior")
	void ExecuteBehavior(const FString& BehaviorName);

	/**
	 * Perceive the environment and update knowledge
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Perception")
	void PerceiveEnvironment();

	/**
	 * Set attention target
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Perception")
	void SetAttentionTarget(AActor* Target);

	/**
	 * Send a message to another agent
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Communication")
	void SendMessage(const FString& ReceiverID, const FString& MessageType, const FString& Content, float Priority = 0.5f);

	/**
	 * Receive a message from another agent
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Communication")
	void ReceiveMessage(const FOpenCogAgentMessage& Message);

	/**
	 * Get agent ID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCog|Agent")
	FString GetAgentID() const { return AgentID; }

	/**
	 * Integrate with Convai chatbot component
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Integration")
	void IntegrateWithConvai(class UConvaiChatbotComponent* ChatbotComponent);

	/** Events */
	UPROPERTY(BlueprintAssignable, Category = "OpenCog|Events")
	FOnGoalChangedSignature OnGoalChanged;

	UPROPERTY(BlueprintAssignable, Category = "OpenCog|Events")
	FOnBehaviorExecutedSignature OnBehaviorExecuted;

	UPROPERTY(BlueprintAssignable, Category = "OpenCog|Events")
	FOnMessageReceivedSignature OnMessageReceived;

protected:
	/** Unique agent ID */
	FString AgentID;

	/** Reference to AtomSpace subsystem */
	UPROPERTY()
	class UOpenCogAtomSpace* AtomSpace;

	/** Reference to orchestration manager */
	UPROPERTY()
	class UOpenCogOrchestrationManager* OrchestrationManager;

	/** Reference to integrated Convai component */
	UPROPERTY()
	class UConvaiChatbotComponent* ConvaiChatbot;

	/** Timer for behavior updates */
	float BehaviorUpdateTimer;

	/** Message queue */
	TArray<FOpenCogAgentMessage> MessageQueue;

	/** Process goals and update priorities */
	void ProcessGoals(float DeltaTime);

	/** Evaluate context for behavior rules */
	bool EvaluateContext(const FString& Context) const;

	/** Update knowledge from perception */
	void UpdateKnowledge();

	/** Process incoming messages */
	void ProcessMessages();
};

// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OpenCogDefinitions.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(OpenCogLog, Log, All);

/**
 * Represents an Atom in OpenCog's AtomSpace
 * Atoms are the fundamental knowledge representation units
 */
USTRUCT(BlueprintType)
struct FOpenCogAtom
{
	GENERATED_BODY()

	/** Unique identifier for the atom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString AtomID;

	/** Type of the atom (e.g., ConceptNode, PredicateNode, EvaluationLink) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString AtomType;

	/** Name/label of the atom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString Name;

	/** Truth value strength [0.0, 1.0] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float TruthStrength;

	/** Truth value confidence [0.0, 1.0] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float TruthConfidence;

	/** Attention value for importance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float AttentionValue;

	/** Outgoing set - IDs of connected atoms */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	TArray<FString> OutgoingSet;

	FOpenCogAtom()
		: AtomID("")
		, AtomType("ConceptNode")
		, Name("")
		, TruthStrength(1.0f)
		, TruthConfidence(1.0f)
		, AttentionValue(0.0f)
	{
	}
};

/**
 * Represents a goal in the OpenPsi framework
 */
USTRUCT(BlueprintType)
struct FOpenCogGoal
{
	GENERATED_BODY()

	/** Goal identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString GoalID;

	/** Goal name/description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString GoalName;

	/** Priority of the goal [0.0, 1.0] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float Priority;

	/** Current satisfaction level [0.0, 1.0] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float Satisfaction;

	/** Is this goal currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	bool bIsActive;

	/** Target value to achieve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float TargetValue;

	FOpenCogGoal()
		: GoalID("")
		, GoalName("")
		, Priority(0.5f)
		, Satisfaction(0.0f)
		, bIsActive(false)
		, TargetValue(1.0f)
	{
	}
};

/**
 * Represents a behavior/action rule in OpenPsi
 */
USTRUCT(BlueprintType)
struct FOpenCogBehaviorRule
{
	GENERATED_BODY()

	/** Rule identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString RuleID;

	/** Rule name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString RuleName;

	/** Context condition (when to apply) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString Context;

	/** Action to execute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString Action;

	/** Goal this rule helps achieve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	FString GoalID;

	/** Weight/strength of the rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	float Weight;

	/** Has this rule been satisfied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog")
	bool bIsSatisfied;

	FOpenCogBehaviorRule()
		: RuleID("")
		, RuleName("")
		, Context("")
		, Action("")
		, GoalID("")
		, Weight(1.0f)
		, bIsSatisfied(false)
	{
	}
};

/**
 * Represents an autonomous agent managed by OpenCog
 */
USTRUCT(BlueprintType)
struct FOpenCogAgent
{
	GENERATED_BODY()

	/** Unique agent identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	FString AgentID;

	/** Agent name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	FString AgentName;

	/** Reference to the actor this agent controls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	TWeakObjectPtr<AActor> ControlledActor;

	/** Current goals for this agent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	TArray<FOpenCogGoal> Goals;

	/** Behavior rules for this agent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	TArray<FOpenCogBehaviorRule> BehaviorRules;

	/** Current active behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	FString ActiveBehavior;

	/** Agent's knowledge atoms */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	TArray<FString> KnowledgeAtomIDs;

	/** Is this agent currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	bool bIsActive;

	/** Agent's attention focus */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Agent")
	TWeakObjectPtr<AActor> AttentionTarget;

	FOpenCogAgent()
		: AgentID("")
		, AgentName("")
		, ControlledActor(nullptr)
		, ActiveBehavior("")
		, bIsActive(false)
		, AttentionTarget(nullptr)
	{
	}
};

/**
 * Parameters for multi-agent coordination
 */
USTRUCT(BlueprintType)
struct FOpenCogCoordinationParams
{
	GENERATED_BODY()

	/** Enable collaborative goal achievement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Coordination")
	bool bEnableCollaboration;

	/** Enable competition for resources */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Coordination")
	bool bEnableCompetition;

	/** Communication range between agents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Coordination")
	float CommunicationRange;

	/** Share knowledge between agents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Coordination")
	bool bShareKnowledge;

	/** Enable emergent behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Coordination")
	bool bEnableEmergentBehavior;

	FOpenCogCoordinationParams()
		: bEnableCollaboration(true)
		, bEnableCompetition(false)
		, CommunicationRange(2000.0f)
		, bShareKnowledge(true)
		, bEnableEmergentBehavior(true)
	{
	}
};

/**
 * Message passed between agents
 */
USTRUCT(BlueprintType)
struct FOpenCogAgentMessage
{
	GENERATED_BODY()

	/** Sender agent ID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	FString SenderID;

	/** Receiver agent ID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	FString ReceiverID;

	/** Message type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	FString MessageType;

	/** Message content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	FString Content;

	/** Timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	float Timestamp;

	/** Priority */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCog|Communication")
	float Priority;

	FOpenCogAgentMessage()
		: SenderID("")
		, ReceiverID("")
		, MessageType("Info")
		, Content("")
		, Timestamp(0.0f)
		, Priority(0.5f)
	{
	}
};

// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenCogDefinitions.h"
#include "OpenCogBlueprintLibrary.generated.h"

/**
 * Blueprint function library for OpenCog autonomous agent system
 */
UCLASS()
class CONVAI_API UOpenCogBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Create a new OpenCog goal
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCog|Goals")
	static FOpenCogGoal MakeGoal(const FString& GoalName, float Priority = 0.5f, float TargetValue = 1.0f);

	/**
	 * Create a new behavior rule
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCog|Behavior")
	static FOpenCogBehaviorRule MakeBehaviorRule(const FString& RuleName, const FString& Context, const FString& Action, const FString& GoalID, float Weight = 1.0f);

	/**
	 * Create a new atom
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCog|AtomSpace")
	static FOpenCogAtom MakeAtom(const FString& AtomType, const FString& Name, float TruthStrength = 1.0f, float TruthConfidence = 1.0f);

	/**
	 * Get the OpenCog AtomSpace subsystem
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace", meta = (WorldContext = "WorldContextObject"))
	static class UOpenCogAtomSpace* GetAtomSpace(const UObject* WorldContextObject);

	/**
	 * Get the OpenCog Orchestration Manager subsystem
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration", meta = (WorldContext = "WorldContextObject"))
	static class UOpenCogOrchestrationManager* GetOrchestrationManager(const UObject* WorldContextObject);

	/**
	 * Create default coordination parameters
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCog|Coordination")
	static FOpenCogCoordinationParams MakeDefaultCoordinationParams();

	/**
	 * Check if an agent has a specific goal
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	static bool AgentHasGoal(class UOpenCogAgentComponent* Agent, const FString& GoalID);

	/**
	 * Get all active goals from an agent
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Goals")
	static TArray<FOpenCogGoal> GetActiveGoals(class UOpenCogAgentComponent* Agent);

	/**
	 * Get agents near a location
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration", meta = (WorldContext = "WorldContextObject"))
	static TArray<class UOpenCogAgentComponent*> FindAgentsNearLocation(const UObject* WorldContextObject, FVector Location, float Range);

	/**
	 * Import Convai environment into OpenCog AtomSpace
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Integration", meta = (WorldContext = "WorldContextObject"))
	static void ImportConvaiEnvironment(const UObject* WorldContextObject, class UConvaiEnvironment* Environment);

	/**
	 * Create a common goal for multiple agents (collaborative task)
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Coordination")
	static void CreateCollaborativeGoal(const TArray<class UOpenCogAgentComponent*>& Agents, const FString& GoalName, float Priority);

	/**
	 * Get atom by name from AtomSpace
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace", meta = (WorldContext = "WorldContextObject"))
	static bool FindAtomByName(const UObject* WorldContextObject, const FString& AtomName, FOpenCogAtom& OutAtom);

	/**
	 * Link two concepts in AtomSpace
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace", meta = (WorldContext = "WorldContextObject"))
	static FString LinkConcepts(const UObject* WorldContextObject, const FString& Concept1, const FString& Concept2, const FString& LinkType = "SimilarityLink");
};

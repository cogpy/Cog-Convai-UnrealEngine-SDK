// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OpenCogDefinitions.h"
#include "Subsystems/WorldSubsystem.h"
#include "OpenCogOrchestrationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAgentRegisteredSignature, const FString&, AgentID, class UOpenCogAgentComponent*, Agent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentUnregisteredSignature, const FString&, AgentID);

/**
 * Central manager for multi-agent orchestration
 * Coordinates communication, collaboration, and emergent behavior between agents
 */
UCLASS()
class CONVAI_API UOpenCogOrchestrationManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UOpenCogOrchestrationManager();

	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/**
	 * Register an agent with the orchestration manager
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void RegisterAgent(class UOpenCogAgentComponent* Agent);

	/**
	 * Unregister an agent
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void UnregisterAgent(const FString& AgentID);

	/**
	 * Get agent by ID
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	class UOpenCogAgentComponent* GetAgent(const FString& AgentID) const;

	/**
	 * Get all registered agents
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	TArray<class UOpenCogAgentComponent*> GetAllAgents() const;

	/**
	 * Get agents within range of a location
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	TArray<class UOpenCogAgentComponent*> GetAgentsInRange(FVector Location, float Range) const;

	/**
	 * Send a message between agents
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void SendMessage(const FOpenCogAgentMessage& Message);

	/**
	 * Broadcast a message to all agents
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void BroadcastMessage(const FString& SenderID, const FString& MessageType, const FString& Content, float Priority = 0.5f);

	/**
	 * Set coordination parameters
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void SetCoordinationParams(const FOpenCogCoordinationParams& Params);

	/**
	 * Get coordination parameters
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	FOpenCogCoordinationParams GetCoordinationParams() const { return CoordinationParams; }

	/**
	 * Enable/disable emergent behavior
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void SetEmergentBehaviorEnabled(bool bEnabled);

	/**
	 * Find agents working on similar goals
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	TArray<class UOpenCogAgentComponent*> FindAgentsWithSimilarGoals(const FString& GoalID) const;

	/**
	 * Coordinate collaborative goal achievement
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void CoordinateCollaboration(const TArray<FString>& AgentIDs, const FString& GoalID);

	/**
	 * Share knowledge between agents
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void ShareKnowledge(const FString& SourceAgentID, const FString& TargetAgentID, const TArray<FString>& AtomIDs);

	/**
	 * Update emergent behavior patterns
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|Orchestration")
	void UpdateEmergentBehavior(float DeltaTime);

	/** Events */
	UPROPERTY(BlueprintAssignable, Category = "OpenCog|Events")
	FOnAgentRegisteredSignature OnAgentRegistered;

	UPROPERTY(BlueprintAssignable, Category = "OpenCog|Events")
	FOnAgentUnregisteredSignature OnAgentUnregistered;

protected:
	/** Registered agents */
	UPROPERTY()
	TMap<FString, class UOpenCogAgentComponent*> RegisteredAgents;

	/** Coordination parameters */
	UPROPERTY()
	FOpenCogCoordinationParams CoordinationParams;

	/** Reference to AtomSpace */
	UPROPERTY()
	class UOpenCogAtomSpace* AtomSpace;

	/** Message queue for inter-agent communication */
	TArray<FOpenCogAgentMessage> MessageQueue;

	/** Emergent behavior patterns */
	TArray<FString> EmergentPatterns;

	/** Process message queue */
	void ProcessMessageQueue();

	/** Detect and create emergent behavior patterns */
	void DetectEmergentPatterns();

	/** Facilitate knowledge sharing based on coordination params */
	void FacilitateKnowledgeSharing();

	/** Manage collaborative goals */
	void ManageCollaboration();
};

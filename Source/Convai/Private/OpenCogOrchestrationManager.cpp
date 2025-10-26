// Copyright 2022 Convai Inc. All Rights Reserved.

#include "OpenCogOrchestrationManager.h"
#include "OpenCogAgentComponent.h"
#include "OpenCogAtomSpace.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UOpenCogOrchestrationManager::UOpenCogOrchestrationManager()
{
	CoordinationParams.bEnableCollaboration = true;
	CoordinationParams.bEnableCompetition = false;
	CoordinationParams.CommunicationRange = 2000.0f;
	CoordinationParams.bShareKnowledge = true;
	CoordinationParams.bEnableEmergentBehavior = true;
}

void UOpenCogOrchestrationManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Get AtomSpace subsystem
	if (UWorld* World = GetWorld())
	{
		AtomSpace = World->GetSubsystem<UOpenCogAtomSpace>();
	}

	UE_LOG(OpenCogLog, Log, TEXT("OpenCog Orchestration Manager initialized"));
}

void UOpenCogOrchestrationManager::Deinitialize()
{
	RegisteredAgents.Empty();
	MessageQueue.Empty();
	EmergentPatterns.Empty();
	
	UE_LOG(OpenCogLog, Log, TEXT("OpenCog Orchestration Manager deinitialized"));
	Super::Deinitialize();
}

void UOpenCogOrchestrationManager::Tick(float DeltaTime)
{
	// Process message queue
	ProcessMessageQueue();

	// Update emergent behavior if enabled
	if (CoordinationParams.bEnableEmergentBehavior)
	{
		UpdateEmergentBehavior(DeltaTime);
	}

	// Facilitate knowledge sharing
	if (CoordinationParams.bShareKnowledge)
	{
		FacilitateKnowledgeSharing();
	}

	// Manage collaboration
	if (CoordinationParams.bEnableCollaboration)
	{
		ManageCollaboration();
	}
}

TStatId UOpenCogOrchestrationManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UOpenCogOrchestrationManager, STATGROUP_Tickables);
}

void UOpenCogOrchestrationManager::RegisterAgent(UOpenCogAgentComponent* Agent)
{
	if (!Agent)
		return;

	FString AgentID = Agent->GetAgentID();
	if (RegisteredAgents.Contains(AgentID))
	{
		UE_LOG(OpenCogLog, Warning, TEXT("Agent %s already registered"), *AgentID);
		return;
	}

	RegisteredAgents.Add(AgentID, Agent);
	OnAgentRegistered.Broadcast(AgentID, Agent);
	
	UE_LOG(OpenCogLog, Log, TEXT("Registered agent: %s (Total agents: %d)"), *AgentID, RegisteredAgents.Num());
}

void UOpenCogOrchestrationManager::UnregisterAgent(const FString& AgentID)
{
	if (RegisteredAgents.Remove(AgentID) > 0)
	{
		OnAgentUnregistered.Broadcast(AgentID);
		UE_LOG(OpenCogLog, Log, TEXT("Unregistered agent: %s (Remaining agents: %d)"), *AgentID, RegisteredAgents.Num());
	}
}

UOpenCogAgentComponent* UOpenCogOrchestrationManager::GetAgent(const FString& AgentID) const
{
	if (const auto* AgentPtr = RegisteredAgents.Find(AgentID))
	{
		return *AgentPtr;
	}
	return nullptr;
}

TArray<UOpenCogAgentComponent*> UOpenCogOrchestrationManager::GetAllAgents() const
{
	TArray<UOpenCogAgentComponent*> Agents;
	RegisteredAgents.GenerateValueArray(Agents);
	return Agents;
}

TArray<UOpenCogAgentComponent*> UOpenCogOrchestrationManager::GetAgentsInRange(FVector Location, float Range) const
{
	TArray<UOpenCogAgentComponent*> AgentsInRange;

	for (const auto& Pair : RegisteredAgents)
	{
		UOpenCogAgentComponent* Agent = Pair.Value;
		if (Agent && Agent->GetOwner())
		{
			float Distance = FVector::Dist(Location, Agent->GetOwner()->GetActorLocation());
			if (Distance <= Range)
			{
				AgentsInRange.Add(Agent);
			}
		}
	}

	return AgentsInRange;
}

void UOpenCogOrchestrationManager::SendMessage(const FOpenCogAgentMessage& Message)
{
	// Add to message queue
	MessageQueue.Add(Message);
}

void UOpenCogOrchestrationManager::BroadcastMessage(const FString& SenderID, const FString& MessageType, const FString& Content, float Priority)
{
	for (const auto& Pair : RegisteredAgents)
	{
		if (Pair.Key != SenderID) // Don't send to self
		{
			FOpenCogAgentMessage Message;
			Message.SenderID = SenderID;
			Message.ReceiverID = Pair.Key;
			Message.MessageType = MessageType;
			Message.Content = Content;
			Message.Priority = Priority;
			Message.Timestamp = GetWorld()->GetTimeSeconds();
			
			MessageQueue.Add(Message);
		}
	}

	UE_LOG(OpenCogLog, Verbose, TEXT("Broadcasted %s message from %s to %d agents"), 
		*MessageType, *SenderID, RegisteredAgents.Num() - 1);
}

void UOpenCogOrchestrationManager::SetCoordinationParams(const FOpenCogCoordinationParams& Params)
{
	CoordinationParams = Params;
	UE_LOG(OpenCogLog, Log, TEXT("Coordination parameters updated"));
}

void UOpenCogOrchestrationManager::SetEmergentBehaviorEnabled(bool bEnabled)
{
	CoordinationParams.bEnableEmergentBehavior = bEnabled;
	UE_LOG(OpenCogLog, Log, TEXT("Emergent behavior %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

TArray<UOpenCogAgentComponent*> UOpenCogOrchestrationManager::FindAgentsWithSimilarGoals(const FString& GoalID) const
{
	TArray<UOpenCogAgentComponent*> MatchingAgents;

	for (const auto& Pair : RegisteredAgents)
	{
		UOpenCogAgentComponent* Agent = Pair.Value;
		if (Agent)
		{
			FOpenCogGoal Goal;
			if (Agent->GetGoal(GoalID, Goal))
			{
				MatchingAgents.Add(Agent);
			}
		}
	}

	return MatchingAgents;
}

void UOpenCogOrchestrationManager::CoordinateCollaboration(const TArray<FString>& AgentIDs, const FString& GoalID)
{
	if (AgentIDs.Num() < 2)
		return;

	UE_LOG(OpenCogLog, Log, TEXT("Coordinating collaboration for goal %s with %d agents"), *GoalID, AgentIDs.Num());

	// Send collaboration requests
	for (const FString& AgentID : AgentIDs)
	{
		for (const FString& OtherAgentID : AgentIDs)
		{
			if (AgentID != OtherAgentID)
			{
				FOpenCogAgentMessage Message;
				Message.SenderID = TEXT("Orchestrator");
				Message.ReceiverID = AgentID;
				Message.MessageType = TEXT("CollaborationRequest");
				Message.Content = FString::Printf(TEXT("Goal:%s,Partner:%s"), *GoalID, *OtherAgentID);
				Message.Priority = 0.8f;
				
				SendMessage(Message);
			}
		}
	}
}

void UOpenCogOrchestrationManager::ShareKnowledge(const FString& SourceAgentID, const FString& TargetAgentID, const TArray<FString>& AtomIDs)
{
	UOpenCogAgentComponent* TargetAgent = GetAgent(TargetAgentID);
	if (!TargetAgent || !AtomSpace)
		return;

	for (const FString& AtomID : AtomIDs)
	{
		FOpenCogAtom Atom;
		if (AtomSpace->GetAtom(AtomID, Atom))
		{
			// Send knowledge share message
			FOpenCogAgentMessage Message;
			Message.SenderID = SourceAgentID;
			Message.ReceiverID = TargetAgentID;
			Message.MessageType = TEXT("KnowledgeShare");
			Message.Content = FString::Printf(TEXT("AtomID:%s,AtomType:%s,Name:%s"), 
				*AtomID, *Atom.AtomType, *Atom.Name);
			Message.Priority = 0.6f;
			
			SendMessage(Message);
		}
	}

	UE_LOG(OpenCogLog, Verbose, TEXT("Shared %d knowledge atoms from %s to %s"), 
		AtomIDs.Num(), *SourceAgentID, *TargetAgentID);
}

void UOpenCogOrchestrationManager::UpdateEmergentBehavior(float DeltaTime)
{
	// Detect emergent patterns in agent behavior
	DetectEmergentPatterns();

	// Update emergent behaviors periodically
	static float EmergentUpdateTimer = 0.0f;
	EmergentUpdateTimer += DeltaTime;
	
	if (EmergentUpdateTimer >= 5.0f) // Update every 5 seconds
	{
		EmergentUpdateTimer = 0.0f;
		
		// Analyze agent interactions and create emergent patterns
		if (RegisteredAgents.Num() >= 2)
		{
			// Example: Detect clustering behavior
			// If multiple agents are close together, they might form a group
			TArray<UOpenCogAgentComponent*> Agents = GetAllAgents();
			for (int32 i = 0; i < Agents.Num(); i++)
			{
				TArray<UOpenCogAgentComponent*> NearbyAgents = GetAgentsInRange(
					Agents[i]->GetOwner()->GetActorLocation(), 
					CoordinationParams.CommunicationRange
				);

				if (NearbyAgents.Num() >= 3)
				{
					FString Pattern = FString::Printf(TEXT("Cluster_%d"), FMath::Rand());
					if (!EmergentPatterns.Contains(Pattern))
					{
						EmergentPatterns.Add(Pattern);
						UE_LOG(OpenCogLog, Log, TEXT("Detected emergent pattern: Agent clustering"));
					}
				}
			}
		}
	}
}

void UOpenCogOrchestrationManager::ProcessMessageQueue()
{
	// Sort by priority
	MessageQueue.Sort([](const FOpenCogAgentMessage& A, const FOpenCogAgentMessage& B) {
		return A.Priority > B.Priority;
	});

	// Process all messages
	for (const FOpenCogAgentMessage& Message : MessageQueue)
	{
		UOpenCogAgentComponent* Receiver = GetAgent(Message.ReceiverID);
		if (Receiver)
		{
			Receiver->ReceiveMessage(Message);
		}
	}

	MessageQueue.Empty();
}

void UOpenCogOrchestrationManager::DetectEmergentPatterns()
{
	// Analyze agent behaviors to detect emergent patterns
	// This is a simplified implementation
	
	if (RegisteredAgents.Num() < 2)
		return;

	// Example: Detect synchronized behavior
	TMap<FString, int32> BehaviorCounts;
	for (const auto& Pair : RegisteredAgents)
	{
		UOpenCogAgentComponent* Agent = Pair.Value;
		if (Agent)
		{
			FString Behavior = Agent->ActiveBehavior;
			if (!Behavior.IsEmpty())
			{
				int32& Count = BehaviorCounts.FindOrAdd(Behavior, 0);
				Count++;
			}
		}
	}

	// If many agents are doing the same thing, it's an emergent pattern
	for (const auto& BehaviorPair : BehaviorCounts)
	{
		if (BehaviorPair.Value >= 3)
		{
			FString Pattern = FString::Printf(TEXT("Synchronized_%s"), *BehaviorPair.Key);
			if (!EmergentPatterns.Contains(Pattern))
			{
				EmergentPatterns.Add(Pattern);
				UE_LOG(OpenCogLog, Log, TEXT("Detected emergent pattern: %d agents performing '%s'"), 
					BehaviorPair.Value, *BehaviorPair.Key);
			}
		}
	}
}

void UOpenCogOrchestrationManager::FacilitateKnowledgeSharing()
{
	// Periodically share knowledge between nearby agents
	static float ShareTimer = 0.0f;
	ShareTimer += GetWorld()->GetDeltaSeconds();
	
	if (ShareTimer >= 10.0f) // Share every 10 seconds
	{
		ShareTimer = 0.0f;

		for (const auto& Pair : RegisteredAgents)
		{
			UOpenCogAgentComponent* Agent = Pair.Value;
			if (Agent && Agent->GetOwner())
			{
				// Find nearby agents
				TArray<UOpenCogAgentComponent*> NearbyAgents = GetAgentsInRange(
					Agent->GetOwner()->GetActorLocation(),
					CoordinationParams.CommunicationRange
				);

				// Share knowledge with nearby agents
				for (UOpenCogAgentComponent* NearbyAgent : NearbyAgents)
				{
					if (NearbyAgent != Agent && Agent->KnowledgeAtomIDs.Num() > 0)
					{
						// Share a subset of knowledge
						int32 ShareCount = FMath::Min(3, Agent->KnowledgeAtomIDs.Num());
						TArray<FString> AtomsToShare;
						for (int32 i = 0; i < ShareCount; i++)
						{
							AtomsToShare.Add(Agent->KnowledgeAtomIDs[i]);
						}
						
						ShareKnowledge(Pair.Key, NearbyAgent->GetAgentID(), AtomsToShare);
					}
				}
			}
		}
	}
}

void UOpenCogOrchestrationManager::ManageCollaboration()
{
	// Find agents with complementary goals
	static float CollabTimer = 0.0f;
	CollabTimer += GetWorld()->GetDeltaSeconds();
	
	if (CollabTimer >= 15.0f) // Check every 15 seconds
	{
		CollabTimer = 0.0f;

		// Group agents by goals
		TMap<FString, TArray<FString>> GoalGroups;
		
		for (const auto& Pair : RegisteredAgents)
		{
			UOpenCogAgentComponent* Agent = Pair.Value;
			if (Agent)
			{
				FOpenCogGoal TopGoal;
				if (Agent->GetTopPriorityGoal(TopGoal))
				{
					TArray<FString>& Group = GoalGroups.FindOrAdd(TopGoal.GoalID);
					Group.Add(Pair.Key);
				}
			}
		}

		// Coordinate collaboration for groups with multiple agents
		for (const auto& GroupPair : GoalGroups)
		{
			if (GroupPair.Value.Num() >= 2)
			{
				CoordinateCollaboration(GroupPair.Value, GroupPair.Key);
			}
		}
	}
}

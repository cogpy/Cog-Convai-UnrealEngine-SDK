// Copyright 2022 Convai Inc. All Rights Reserved.

#include "OpenCogAgentComponent.h"
#include "OpenCogAtomSpace.h"
#include "OpenCogOrchestrationManager.h"
#include "ConvaiChatbotComponent.h"
#include "ConvaiDefinitions.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UOpenCogAgentComponent::UOpenCogAgentComponent()
	: bEnableAutonomousBehavior(true)
	, BehaviorUpdateInterval(1.0f)
	, PerceptionRange(2000.0f)
	, AttentionTarget(nullptr)
	, BehaviorUpdateTimer(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Tick 10 times per second
}

void UOpenCogAgentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Generate unique agent ID
	AgentID = FString::Printf(TEXT("Agent_%s_%d"), *AgentName, FMath::Rand());

	// Get AtomSpace subsystem
	if (UWorld* World = GetWorld())
	{
		AtomSpace = World->GetSubsystem<UOpenCogAtomSpace>();
		OrchestrationManager = World->GetSubsystem<UOpenCogOrchestrationManager>();
	}

	// Register with orchestration manager
	if (OrchestrationManager)
	{
		OrchestrationManager->RegisterAgent(this);
	}

	UE_LOG(OpenCogLog, Log, TEXT("OpenCog Agent '%s' (ID: %s) initialized"), *AgentName, *AgentID);
}

void UOpenCogAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from orchestration manager
	if (OrchestrationManager)
	{
		OrchestrationManager->UnregisterAgent(AgentID);
	}

	Super::EndPlay(EndPlayReason);
}

void UOpenCogAgentComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableAutonomousBehavior)
		return;

	// Process goals
	ProcessGoals(DeltaTime);

	// Process messages
	ProcessMessages();

	// Update behavior
	BehaviorUpdateTimer += DeltaTime;
	if (BehaviorUpdateTimer >= BehaviorUpdateInterval)
	{
		BehaviorUpdateTimer = 0.0f;

		// Select and execute behavior
		FString SelectedBehavior = SelectBehavior();
		if (!SelectedBehavior.IsEmpty() && SelectedBehavior != ActiveBehavior)
		{
			ExecuteBehavior(SelectedBehavior);
		}
	}

	// Periodic perception
	if (FMath::Fmod(BehaviorUpdateTimer, 0.5f) < DeltaTime)
	{
		PerceiveEnvironment();
	}
}

void UOpenCogAgentComponent::AddGoal(const FOpenCogGoal& Goal)
{
	// Check if goal already exists
	for (int32 i = 0; i < Goals.Num(); i++)
	{
		if (Goals[i].GoalID == Goal.GoalID)
		{
			Goals[i] = Goal;
			UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Updated goal %s"), *AgentID, *Goal.GoalName);
			OnGoalChanged.Broadcast(this, Goal);
			return;
		}
	}

	Goals.Add(Goal);
	UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Added goal %s (Priority: %.2f)"), *AgentID, *Goal.GoalName, Goal.Priority);
	OnGoalChanged.Broadcast(this, Goal);
}

bool UOpenCogAgentComponent::RemoveGoal(const FString& GoalID)
{
	for (int32 i = 0; i < Goals.Num(); i++)
	{
		if (Goals[i].GoalID == GoalID)
		{
			Goals.RemoveAt(i);
			UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Removed goal %s"), *AgentID, *GoalID);
			return true;
		}
	}
	return false;
}

bool UOpenCogAgentComponent::GetGoal(const FString& GoalID, FOpenCogGoal& OutGoal) const
{
	for (const FOpenCogGoal& Goal : Goals)
	{
		if (Goal.GoalID == GoalID)
		{
			OutGoal = Goal;
			return true;
		}
	}
	return false;
}

void UOpenCogAgentComponent::UpdateGoalSatisfaction(const FString& GoalID, float Satisfaction)
{
	for (FOpenCogGoal& Goal : Goals)
	{
		if (Goal.GoalID == GoalID)
		{
			Goal.Satisfaction = FMath::Clamp(Satisfaction, 0.0f, 1.0f);
			UE_LOG(OpenCogLog, Verbose, TEXT("Agent %s: Goal %s satisfaction updated to %.2f"), *AgentID, *GoalID, Satisfaction);
			OnGoalChanged.Broadcast(this, Goal);
			break;
		}
	}
}

bool UOpenCogAgentComponent::GetTopPriorityGoal(FOpenCogGoal& OutGoal) const
{
	float HighestPriority = -1.0f;
	bool FoundGoal = false;

	for (const FOpenCogGoal& Goal : Goals)
	{
		if (Goal.bIsActive && Goal.Satisfaction < Goal.TargetValue)
		{
			float EffectivePriority = Goal.Priority * (1.0f - Goal.Satisfaction);
			if (EffectivePriority > HighestPriority)
			{
				HighestPriority = EffectivePriority;
				OutGoal = Goal;
				FoundGoal = true;
			}
		}
	}

	return FoundGoal;
}

void UOpenCogAgentComponent::AddBehaviorRule(const FOpenCogBehaviorRule& Rule)
{
	// Check if rule already exists
	for (int32 i = 0; i < BehaviorRules.Num(); i++)
	{
		if (BehaviorRules[i].RuleID == Rule.RuleID)
		{
			BehaviorRules[i] = Rule;
			return;
		}
	}

	BehaviorRules.Add(Rule);
	UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Added behavior rule %s"), *AgentID, *Rule.RuleName);
}

bool UOpenCogAgentComponent::RemoveBehaviorRule(const FString& RuleID)
{
	for (int32 i = 0; i < BehaviorRules.Num(); i++)
	{
		if (BehaviorRules[i].RuleID == RuleID)
		{
			BehaviorRules.RemoveAt(i);
			return true;
		}
	}
	return false;
}

FString UOpenCogAgentComponent::SelectBehavior()
{
	// Get top priority goal
	FOpenCogGoal TopGoal;
	if (!GetTopPriorityGoal(TopGoal))
	{
		return FString();
	}

	// Find best matching behavior rule
	float BestScore = -1.0f;
	FString BestBehavior;

	for (const FOpenCogBehaviorRule& Rule : BehaviorRules)
	{
		// Check if rule applies to current goal
		if (Rule.GoalID == TopGoal.GoalID || Rule.GoalID.IsEmpty())
		{
			// Evaluate context
			if (EvaluateContext(Rule.Context))
			{
				float Score = Rule.Weight * TopGoal.Priority;
				if (Score > BestScore)
				{
					BestScore = Score;
					BestBehavior = Rule.Action;
				}
			}
		}
	}

	return BestBehavior;
}

void UOpenCogAgentComponent::ExecuteBehavior(const FString& BehaviorName)
{
	if (BehaviorName.IsEmpty())
		return;

	ActiveBehavior = BehaviorName;
	UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Executing behavior '%s'"), *AgentID, *BehaviorName);

	// Trigger blueprint event
	OnBehaviorExecuted.Broadcast(this, BehaviorName);

	// If integrated with Convai, trigger narrative behavior
	if (ConvaiChatbot && IsValid(ConvaiChatbot))
	{
		// Execute narrative trigger based on behavior
		ConvaiChatbot->ExecuteNarrativeTrigger(BehaviorName, nullptr, true, true, false);
	}
}

void UOpenCogAgentComponent::PerceiveEnvironment()
{
	if (!AtomSpace)
		return;

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// Find nearby actors
	TArray<AActor*> NearbyActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UConvaiChatbotComponent::StaticClass(), NearbyActors);

	FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* Actor : NearbyActors)
	{
		if (Actor == Owner)
			continue;

		float Distance = FVector::Dist(OwnerLocation, Actor->GetActorLocation());
		if (Distance <= PerceptionRange)
		{
			// Create or update atom for perceived actor
			FOpenCogAtom PerceivedAtom;
			PerceivedAtom.AtomType = TEXT("ConceptNode");
			PerceivedAtom.Name = Actor->GetName();
			PerceivedAtom.AttentionValue = 1.0f - (Distance / PerceptionRange);
			
			FString AtomID = AtomSpace->AddAtom(PerceivedAtom);
			KnowledgeAtomIDs.AddUnique(AtomID);
		}
	}
}

void UOpenCogAgentComponent::SetAttentionTarget(AActor* Target)
{
	AttentionTarget = Target;
	
	if (Target && AtomSpace)
	{
		// Increase attention value for target
		TArray<FOpenCogAtom> Atoms = AtomSpace->FindAtomsByName(Target->GetName());
		for (FOpenCogAtom& Atom : Atoms)
		{
			Atom.AttentionValue = 1.0f;
			AtomSpace->UpdateAtom(Atom);
		}
	}

	UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Attention target set to %s"), *AgentID, Target ? *Target->GetName() : TEXT("None"));
}

void UOpenCogAgentComponent::SendMessage(const FString& ReceiverID, const FString& MessageType, const FString& Content, float Priority)
{
	if (!OrchestrationManager)
		return;

	FOpenCogAgentMessage Message;
	Message.SenderID = AgentID;
	Message.ReceiverID = ReceiverID;
	Message.MessageType = MessageType;
	Message.Content = Content;
	Message.Timestamp = GetWorld()->GetTimeSeconds();
	Message.Priority = Priority;

	OrchestrationManager->SendMessage(Message);
	UE_LOG(OpenCogLog, Verbose, TEXT("Agent %s: Sent %s message to %s"), *AgentID, *MessageType, *ReceiverID);
}

void UOpenCogAgentComponent::ReceiveMessage(const FOpenCogAgentMessage& Message)
{
	MessageQueue.Add(Message);
	OnMessageReceived.Broadcast(this, Message.SenderID, Message);
	UE_LOG(OpenCogLog, Verbose, TEXT("Agent %s: Received %s message from %s"), *AgentID, *Message.MessageType, *Message.SenderID);
}

void UOpenCogAgentComponent::IntegrateWithConvai(UConvaiChatbotComponent* ChatbotComponent)
{
	ConvaiChatbot = ChatbotComponent;
	
	if (ConvaiChatbot)
	{
		UE_LOG(OpenCogLog, Log, TEXT("Agent %s: Integrated with Convai chatbot"), *AgentID);
		
		// Bind to Convai events to update goals based on conversation
		// This allows the agent to be influenced by player interactions
	}
}

void UOpenCogAgentComponent::ProcessGoals(float DeltaTime)
{
	// Update goal satisfaction based on current state
	for (FOpenCogGoal& Goal : Goals)
	{
		if (Goal.bIsActive)
		{
			// Goals naturally decay over time if not actively pursued
			Goal.Satisfaction = FMath::Max(0.0f, Goal.Satisfaction - DeltaTime * 0.01f);
		}
	}
}

bool UOpenCogAgentComponent::EvaluateContext(const FString& Context) const
{
	if (Context.IsEmpty())
		return true;

	// Simple context evaluation
	// In a full implementation, this would use the AtomSpace to evaluate complex conditions
	
	// Check for basic keywords
	if (Context.Contains(TEXT("nearby")) && AttentionTarget != nullptr)
	{
		AActor* Owner = GetOwner();
		if (Owner && AttentionTarget)
		{
			float Distance = FVector::Dist(Owner->GetActorLocation(), AttentionTarget->GetActorLocation());
			return Distance <= PerceptionRange;
		}
	}

	if (Context.Contains(TEXT("idle")) && ActiveBehavior.IsEmpty())
	{
		return true;
	}

	return true; // Default to true for now
}

void UOpenCogAgentComponent::UpdateKnowledge()
{
	// Update knowledge atoms based on current perceptions
	// This is called after perception to consolidate learned information
	
	if (!AtomSpace)
		return;

	// Clean up old knowledge atoms with low attention
	for (int32 i = KnowledgeAtomIDs.Num() - 1; i >= 0; i--)
	{
		FOpenCogAtom Atom;
		if (AtomSpace->GetAtom(KnowledgeAtomIDs[i], Atom))
		{
			if (Atom.AttentionValue < 0.1f)
			{
				KnowledgeAtomIDs.RemoveAt(i);
			}
		}
		else
		{
			KnowledgeAtomIDs.RemoveAt(i);
		}
	}
}

void UOpenCogAgentComponent::ProcessMessages()
{
	// Process messages in priority order
	MessageQueue.Sort([](const FOpenCogAgentMessage& A, const FOpenCogAgentMessage& B) {
		return A.Priority > B.Priority;
	});

	// Process up to 5 messages per tick
	int32 ProcessCount = FMath::Min(5, MessageQueue.Num());
	for (int32 i = 0; i < ProcessCount; i++)
	{
		const FOpenCogAgentMessage& Message = MessageQueue[0];
		
		// Handle different message types
		if (Message.MessageType == TEXT("GoalRequest"))
		{
			// Another agent is requesting help with a goal
			// Could create a collaborative goal here
		}
		else if (Message.MessageType == TEXT("KnowledgeShare"))
		{
			// Another agent is sharing knowledge
			// Add to knowledge base
		}

		MessageQueue.RemoveAt(0);
	}
}

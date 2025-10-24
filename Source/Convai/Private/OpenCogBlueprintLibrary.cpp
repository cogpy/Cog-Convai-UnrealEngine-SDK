// Copyright 2022 Convai Inc. All Rights Reserved.

#include "OpenCogBlueprintLibrary.h"
#include "OpenCogAtomSpace.h"
#include "OpenCogOrchestrationManager.h"
#include "OpenCogAgentComponent.h"
#include "ConvaiDefinitions.h"
#include "Engine/World.h"

FOpenCogGoal UOpenCogBlueprintLibrary::MakeGoal(const FString& GoalName, float Priority, float TargetValue)
{
	FOpenCogGoal Goal;
	Goal.GoalID = FString::Printf(TEXT("Goal_%s_%d"), *GoalName, FMath::Rand());
	Goal.GoalName = GoalName;
	Goal.Priority = FMath::Clamp(Priority, 0.0f, 1.0f);
	Goal.TargetValue = FMath::Clamp(TargetValue, 0.0f, 1.0f);
	Goal.Satisfaction = 0.0f;
	Goal.bIsActive = true;
	return Goal;
}

FOpenCogBehaviorRule UOpenCogBlueprintLibrary::MakeBehaviorRule(const FString& RuleName, const FString& Context, const FString& Action, const FString& GoalID, float Weight)
{
	FOpenCogBehaviorRule Rule;
	Rule.RuleID = FString::Printf(TEXT("Rule_%s_%d"), *RuleName, FMath::Rand());
	Rule.RuleName = RuleName;
	Rule.Context = Context;
	Rule.Action = Action;
	Rule.GoalID = GoalID;
	Rule.Weight = Weight;
	Rule.bIsSatisfied = false;
	return Rule;
}

FOpenCogAtom UOpenCogBlueprintLibrary::MakeAtom(const FString& AtomType, const FString& Name, float TruthStrength, float TruthConfidence)
{
	FOpenCogAtom Atom;
	Atom.AtomType = AtomType;
	Atom.Name = Name;
	Atom.TruthStrength = FMath::Clamp(TruthStrength, 0.0f, 1.0f);
	Atom.TruthConfidence = FMath::Clamp(TruthConfidence, 0.0f, 1.0f);
	Atom.AttentionValue = 0.5f;
	return Atom;
}

UOpenCogAtomSpace* UOpenCogBlueprintLibrary::GetAtomSpace(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	return World->GetSubsystem<UOpenCogAtomSpace>();
}

UOpenCogOrchestrationManager* UOpenCogBlueprintLibrary::GetOrchestrationManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	return World->GetSubsystem<UOpenCogOrchestrationManager>();
}

FOpenCogCoordinationParams UOpenCogBlueprintLibrary::MakeDefaultCoordinationParams()
{
	FOpenCogCoordinationParams Params;
	Params.bEnableCollaboration = true;
	Params.bEnableCompetition = false;
	Params.CommunicationRange = 2000.0f;
	Params.bShareKnowledge = true;
	Params.bEnableEmergentBehavior = true;
	return Params;
}

bool UOpenCogBlueprintLibrary::AgentHasGoal(UOpenCogAgentComponent* Agent, const FString& GoalID)
{
	if (!Agent)
		return false;

	FOpenCogGoal Goal;
	return Agent->GetGoal(GoalID, Goal);
}

TArray<FOpenCogGoal> UOpenCogBlueprintLibrary::GetActiveGoals(UOpenCogAgentComponent* Agent)
{
	TArray<FOpenCogGoal> ActiveGoals;
	
	if (!Agent)
		return ActiveGoals;

	for (const FOpenCogGoal& Goal : Agent->Goals)
	{
		if (Goal.bIsActive)
		{
			ActiveGoals.Add(Goal);
		}
	}

	return ActiveGoals;
}

TArray<UOpenCogAgentComponent*> UOpenCogBlueprintLibrary::FindAgentsNearLocation(const UObject* WorldContextObject, FVector Location, float Range)
{
	TArray<UOpenCogAgentComponent*> Result;
	
	UOpenCogOrchestrationManager* Manager = GetOrchestrationManager(WorldContextObject);
	if (!Manager)
		return Result;

	return Manager->GetAgentsInRange(Location, Range);
}

void UOpenCogBlueprintLibrary::ImportConvaiEnvironment(const UObject* WorldContextObject, UConvaiEnvironment* Environment)
{
	if (!Environment)
		return;

	UOpenCogAtomSpace* AtomSpace = GetAtomSpace(WorldContextObject);
	if (!AtomSpace)
		return;

	AtomSpace->ImportFromConvaiEnvironment(Environment);
}

void UOpenCogBlueprintLibrary::CreateCollaborativeGoal(const TArray<UOpenCogAgentComponent*>& Agents, const FString& GoalName, float Priority)
{
	FOpenCogGoal CollaborativeGoal = MakeGoal(GoalName, Priority);

	for (UOpenCogAgentComponent* Agent : Agents)
	{
		if (Agent)
		{
			Agent->AddGoal(CollaborativeGoal);
		}
	}

	UE_LOG(OpenCogLog, Log, TEXT("Created collaborative goal '%s' for %d agents"), *GoalName, Agents.Num());
}

bool UOpenCogBlueprintLibrary::FindAtomByName(const UObject* WorldContextObject, const FString& AtomName, FOpenCogAtom& OutAtom)
{
	UOpenCogAtomSpace* AtomSpace = GetAtomSpace(WorldContextObject);
	if (!AtomSpace)
		return false;

	TArray<FOpenCogAtom> Atoms = AtomSpace->FindAtomsByName(AtomName);
	if (Atoms.Num() > 0)
	{
		OutAtom = Atoms[0];
		return true;
	}

	return false;
}

FString UOpenCogBlueprintLibrary::LinkConcepts(const UObject* WorldContextObject, const FString& Concept1, const FString& Concept2, const FString& LinkType)
{
	UOpenCogAtomSpace* AtomSpace = GetAtomSpace(WorldContextObject);
	if (!AtomSpace)
		return FString();

	// Find or create concept atoms
	TArray<FOpenCogAtom> Atoms1 = AtomSpace->FindAtomsByName(Concept1);
	TArray<FOpenCogAtom> Atoms2 = AtomSpace->FindAtomsByName(Concept2);

	FString AtomID1;
	FString AtomID2;

	if (Atoms1.Num() > 0)
	{
		AtomID1 = Atoms1[0].AtomID;
	}
	else
	{
		FOpenCogAtom NewAtom = MakeAtom(TEXT("ConceptNode"), Concept1);
		AtomID1 = AtomSpace->AddAtom(NewAtom);
	}

	if (Atoms2.Num() > 0)
	{
		AtomID2 = Atoms2[0].AtomID;
	}
	else
	{
		FOpenCogAtom NewAtom = MakeAtom(TEXT("ConceptNode"), Concept2);
		AtomID2 = AtomSpace->AddAtom(NewAtom);
	}

	return AtomSpace->CreateLink(LinkType, AtomID1, AtomID2);
}

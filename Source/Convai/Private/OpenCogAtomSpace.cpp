// Copyright 2022 Convai Inc. All Rights Reserved.

#include "OpenCogAtomSpace.h"
#include "ConvaiDefinitions.h"
#include "Engine/World.h"

UOpenCogAtomSpace::UOpenCogAtomSpace()
	: AtomIDCounter(0)
	, AttentionDiffusionRate(0.1f)
	, MaxAtoms(10000)
{
}

void UOpenCogAtomSpace::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(OpenCogLog, Log, TEXT("OpenCog AtomSpace initialized"));
	AtomIDCounter = 0;
	Atoms.Empty();
}

void UOpenCogAtomSpace::Deinitialize()
{
	UE_LOG(OpenCogLog, Log, TEXT("OpenCog AtomSpace deinitialized"));
	Atoms.Empty();
	Super::Deinitialize();
}

FString UOpenCogAtomSpace::GenerateAtomID()
{
	return FString::Printf(TEXT("Atom_%d"), ++AtomIDCounter);
}

FString UOpenCogAtomSpace::AddAtom(const FOpenCogAtom& Atom)
{
	FOpenCogAtom NewAtom = Atom;
	
	// Generate ID if not provided
	if (NewAtom.AtomID.IsEmpty())
	{
		NewAtom.AtomID = GenerateAtomID();
	}

	// Check if atom already exists
	if (Atoms.Contains(NewAtom.AtomID))
	{
		UE_LOG(OpenCogLog, Warning, TEXT("Atom %s already exists, updating instead"), *NewAtom.AtomID);
		UpdateAtom(NewAtom);
		return NewAtom.AtomID;
	}

	Atoms.Add(NewAtom.AtomID, NewAtom);
	UE_LOG(OpenCogLog, Verbose, TEXT("Added atom: %s (Type: %s, Name: %s)"), *NewAtom.AtomID, *NewAtom.AtomType, *NewAtom.Name);

	// Perform forgetting if we exceed max atoms
	if (Atoms.Num() > MaxAtoms)
	{
		PerformForgetting(0.1f);
	}

	return NewAtom.AtomID;
}

bool UOpenCogAtomSpace::GetAtom(const FString& AtomID, FOpenCogAtom& OutAtom) const
{
	if (const FOpenCogAtom* FoundAtom = Atoms.Find(AtomID))
	{
		OutAtom = *FoundAtom;
		return true;
	}
	return false;
}

bool UOpenCogAtomSpace::RemoveAtom(const FString& AtomID)
{
	if (Atoms.Remove(AtomID) > 0)
	{
		UE_LOG(OpenCogLog, Verbose, TEXT("Removed atom: %s"), *AtomID);
		return true;
	}
	return false;
}

bool UOpenCogAtomSpace::UpdateAtom(const FOpenCogAtom& Atom)
{
	if (Atoms.Contains(Atom.AtomID))
	{
		Atoms[Atom.AtomID] = Atom;
		UE_LOG(OpenCogLog, Verbose, TEXT("Updated atom: %s"), *Atom.AtomID);
		return true;
	}
	return false;
}

TArray<FOpenCogAtom> UOpenCogAtomSpace::FindAtomsByType(const FString& AtomType) const
{
	TArray<FOpenCogAtom> Results;
	for (const auto& Pair : Atoms)
	{
		if (Pair.Value.AtomType == AtomType)
		{
			Results.Add(Pair.Value);
		}
	}
	return Results;
}

TArray<FOpenCogAtom> UOpenCogAtomSpace::FindAtomsByName(const FString& NamePattern) const
{
	TArray<FOpenCogAtom> Results;
	for (const auto& Pair : Atoms)
	{
		if (Pair.Value.Name.Contains(NamePattern))
		{
			Results.Add(Pair.Value);
		}
	}
	return Results;
}

TArray<FOpenCogAtom> UOpenCogAtomSpace::GetHighAttentionAtoms(float Threshold) const
{
	TArray<FOpenCogAtom> Results;
	for (const auto& Pair : Atoms)
	{
		if (Pair.Value.AttentionValue >= Threshold)
		{
			Results.Add(Pair.Value);
		}
	}
	
	// Sort by attention value (highest first)
	Results.Sort([](const FOpenCogAtom& A, const FOpenCogAtom& B) {
		return A.AttentionValue > B.AttentionValue;
	});
	
	return Results;
}

FString UOpenCogAtomSpace::CreateLink(const FString& LinkType, const FString& SourceAtomID, const FString& TargetAtomID, float Strength, float Confidence)
{
	// Verify source and target atoms exist
	if (!Atoms.Contains(SourceAtomID) || !Atoms.Contains(TargetAtomID))
	{
		UE_LOG(OpenCogLog, Warning, TEXT("Cannot create link: source or target atom not found"));
		return FString();
	}

	// Create link atom
	FOpenCogAtom LinkAtom;
	LinkAtom.AtomType = LinkType;
	LinkAtom.Name = FString::Printf(TEXT("%s_Link"), *LinkType);
	LinkAtom.TruthStrength = Strength;
	LinkAtom.TruthConfidence = Confidence;
	LinkAtom.OutgoingSet.Add(SourceAtomID);
	LinkAtom.OutgoingSet.Add(TargetAtomID);

	return AddAtom(LinkAtom);
}

TArray<FOpenCogAtom> UOpenCogAtomSpace::GetOutgoingAtoms(const FString& LinkAtomID) const
{
	TArray<FOpenCogAtom> Results;
	
	FOpenCogAtom LinkAtom;
	if (GetAtom(LinkAtomID, LinkAtom))
	{
		for (const FString& AtomID : LinkAtom.OutgoingSet)
		{
			FOpenCogAtom OutgoingAtom;
			if (GetAtom(AtomID, OutgoingAtom))
			{
				Results.Add(OutgoingAtom);
			}
		}
	}
	
	return Results;
}

void UOpenCogAtomSpace::UpdateAttentionValues(float DeltaTime)
{
	// Implement attention diffusion algorithm
	// High attention atoms spread attention to connected atoms
	
	TMap<FString, float> AttentionDeltas;
	
	for (auto& Pair : Atoms)
	{
		FOpenCogAtom& Atom = Pair.Value;
		
		// Decay attention over time
		Atom.AttentionValue *= (1.0f - AttentionDiffusionRate * DeltaTime);
		
		// Spread attention to connected atoms
		if (Atom.AttentionValue > 0.1f && Atom.OutgoingSet.Num() > 0)
		{
			float SpreadAmount = Atom.AttentionValue * AttentionDiffusionRate * DeltaTime / Atom.OutgoingSet.Num();
			
			for (const FString& ConnectedID : Atom.OutgoingSet)
			{
				if (!AttentionDeltas.Contains(ConnectedID))
				{
					AttentionDeltas.Add(ConnectedID, 0.0f);
				}
				AttentionDeltas[ConnectedID] += SpreadAmount;
			}
		}
	}
	
	// Apply attention deltas
	for (const auto& DeltaPair : AttentionDeltas)
	{
		if (FOpenCogAtom* Atom = Atoms.Find(DeltaPair.Key))
		{
			Atom->AttentionValue = FMath::Clamp(Atom->AttentionValue + DeltaPair.Value, 0.0f, 1.0f);
		}
	}
}

int32 UOpenCogAtomSpace::PerformForgetting(float MinAttention)
{
	TArray<FString> AtomsToRemove;
	
	for (const auto& Pair : Atoms)
	{
		if (Pair.Value.AttentionValue < MinAttention)
		{
			AtomsToRemove.Add(Pair.Key);
		}
	}
	
	for (const FString& AtomID : AtomsToRemove)
	{
		RemoveAtom(AtomID);
	}
	
	if (AtomsToRemove.Num() > 0)
	{
		UE_LOG(OpenCogLog, Log, TEXT("Forgot %d atoms with attention below %f"), AtomsToRemove.Num(), MinAttention);
	}
	
	return AtomsToRemove.Num();
}

void UOpenCogAtomSpace::ClearAtomSpace()
{
	Atoms.Empty();
	AtomIDCounter = 0;
	UE_LOG(OpenCogLog, Log, TEXT("AtomSpace cleared"));
}

int32 UOpenCogAtomSpace::GetAtomCount() const
{
	return Atoms.Num();
}

void UOpenCogAtomSpace::ImportFromConvaiEnvironment(UConvaiEnvironment* Environment)
{
	if (!Environment)
	{
		UE_LOG(OpenCogLog, Warning, TEXT("Cannot import from null environment"));
		return;
	}

	UE_LOG(OpenCogLog, Log, TEXT("Importing knowledge from Convai environment"));

	// Import objects as ConceptNodes
	for (const FConvaiObjectEntry& Object : Environment->Objects)
	{
		FOpenCogAtom ObjectAtom;
		ObjectAtom.AtomType = TEXT("ConceptNode");
		ObjectAtom.Name = Object.Name;
		ObjectAtom.TruthStrength = 1.0f;
		ObjectAtom.TruthConfidence = 1.0f;
		ObjectAtom.AttentionValue = 0.5f;
		
		FString ObjectAtomID = AddAtom(ObjectAtom);

		// Add description as property
		if (!Object.Description.IsEmpty())
		{
			FOpenCogAtom DescriptionAtom;
			DescriptionAtom.AtomType = TEXT("PredicateNode");
			DescriptionAtom.Name = TEXT("description");
			FString DescAtomID = AddAtom(DescriptionAtom);

			// Create evaluation link
			CreateLink(TEXT("EvaluationLink"), DescAtomID, ObjectAtomID, 1.0f, 1.0f);
		}
	}

	// Import characters as ConceptNodes
	for (const FConvaiObjectEntry& Character : Environment->Characters)
	{
		FOpenCogAtom CharacterAtom;
		CharacterAtom.AtomType = TEXT("ConceptNode");
		CharacterAtom.Name = Character.Name;
		CharacterAtom.TruthStrength = 1.0f;
		CharacterAtom.TruthConfidence = 1.0f;
		CharacterAtom.AttentionValue = 0.7f; // Characters get more attention
		
		FString CharAtomID = AddAtom(CharacterAtom);
	}

	// Import actions as PredicateNodes
	for (const FString& Action : Environment->Actions)
	{
		FOpenCogAtom ActionAtom;
		ActionAtom.AtomType = TEXT("PredicateNode");
		ActionAtom.Name = Action;
		ActionAtom.TruthStrength = 1.0f;
		ActionAtom.TruthConfidence = 1.0f;
		ActionAtom.AttentionValue = 0.3f;
		
		AddAtom(ActionAtom);
	}

	UE_LOG(OpenCogLog, Log, TEXT("Imported %d objects, %d characters, %d actions"), 
		Environment->Objects.Num(), Environment->Characters.Num(), Environment->Actions.Num());
}

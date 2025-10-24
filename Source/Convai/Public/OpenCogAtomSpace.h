// Copyright 2022 Convai Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OpenCogDefinitions.h"
#include "Subsystems/WorldSubsystem.h"
#include "OpenCogAtomSpace.generated.h"

/**
 * Manages the OpenCog AtomSpace - the knowledge representation system
 * This subsystem maintains all atoms and their relationships
 */
UCLASS()
class CONVAI_API UOpenCogAtomSpace : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UOpenCogAtomSpace();

	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Add a new atom to the AtomSpace
	 * @param Atom The atom to add
	 * @return The ID of the added atom
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	FString AddAtom(const FOpenCogAtom& Atom);

	/**
	 * Get an atom by ID
	 * @param AtomID The ID of the atom
	 * @param OutAtom Output parameter for the atom
	 * @return True if atom was found
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	bool GetAtom(const FString& AtomID, FOpenCogAtom& OutAtom) const;

	/**
	 * Remove an atom from the AtomSpace
	 * @param AtomID The ID of the atom to remove
	 * @return True if atom was removed
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	bool RemoveAtom(const FString& AtomID);

	/**
	 * Update an existing atom
	 * @param Atom The atom with updated values
	 * @return True if atom was updated
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	bool UpdateAtom(const FOpenCogAtom& Atom);

	/**
	 * Find atoms by type
	 * @param AtomType The type to search for
	 * @return Array of matching atoms
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	TArray<FOpenCogAtom> FindAtomsByType(const FString& AtomType) const;

	/**
	 * Find atoms by name pattern
	 * @param NamePattern Pattern to match (supports wildcards)
	 * @return Array of matching atoms
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	TArray<FOpenCogAtom> FindAtomsByName(const FString& NamePattern) const;

	/**
	 * Get atoms with high attention values
	 * @param Threshold Minimum attention value
	 * @return Array of atoms above threshold
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	TArray<FOpenCogAtom> GetHighAttentionAtoms(float Threshold = 0.5f) const;

	/**
	 * Create a link between atoms
	 * @param LinkType Type of link (e.g., "InheritanceLink", "SimilarityLink")
	 * @param SourceAtomID Source atom ID
	 * @param TargetAtomID Target atom ID
	 * @param Strength Truth value strength
	 * @param Confidence Truth value confidence
	 * @return ID of created link atom
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	FString CreateLink(const FString& LinkType, const FString& SourceAtomID, const FString& TargetAtomID, float Strength = 1.0f, float Confidence = 1.0f);

	/**
	 * Get all outgoing atoms from a link
	 * @param LinkAtomID The link atom ID
	 * @return Array of connected atoms
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	TArray<FOpenCogAtom> GetOutgoingAtoms(const FString& LinkAtomID) const;

	/**
	 * Update attention values using importance diffusion
	 * Called periodically to spread attention across related atoms
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	void UpdateAttentionValues(float DeltaTime);

	/**
	 * Perform forgetting - remove low attention atoms
	 * @param MinAttention Threshold below which atoms are forgotten
	 * @return Number of atoms removed
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	int32 PerformForgetting(float MinAttention = 0.1f);

	/**
	 * Clear all atoms from the AtomSpace
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	void ClearAtomSpace();

	/**
	 * Get total number of atoms
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	int32 GetAtomCount() const;

	/**
	 * Import knowledge from Convai environment
	 * @param Environment The Convai environment to import from
	 */
	UFUNCTION(BlueprintCallable, Category = "OpenCog|AtomSpace")
	void ImportFromConvaiEnvironment(class UConvaiEnvironment* Environment);

private:
	/** Generate a unique atom ID */
	FString GenerateAtomID();

	/** Storage for all atoms */
	UPROPERTY()
	TMap<FString, FOpenCogAtom> Atoms;

	/** Counter for generating unique IDs */
	int32 AtomIDCounter;

	/** Attention diffusion rate */
	float AttentionDiffusionRate;

	/** Maximum atoms in AtomSpace before forgetting */
	int32 MaxAtoms;
};

# OpenCog Integration Quick Reference

## Core Classes

### UOpenCogAtomSpace (World Subsystem)
**Purpose**: Knowledge representation and semantic memory

**Key Methods**:
- `AddAtom(FOpenCogAtom)` - Add knowledge to the system
- `GetAtom(AtomID, OutAtom)` - Retrieve knowledge
- `CreateLink(LinkType, SourceID, TargetID)` - Create relationships
- `FindAtomsByType(AtomType)` - Query by type
- `ImportFromConvaiEnvironment(Environment)` - Import from Convai

**Usage**:
```cpp
UOpenCogAtomSpace* AtomSpace = GetWorld()->GetSubsystem<UOpenCogAtomSpace>();
FOpenCogAtom PlayerConcept;
PlayerConcept.AtomType = "ConceptNode";
PlayerConcept.Name = "Player";
FString AtomID = AtomSpace->AddAtom(PlayerConcept);
```

---

### UOpenCogAgentComponent (Actor Component)
**Purpose**: Makes an actor an autonomous OpenCog agent

**Key Properties**:
- `AgentName` - Agent identifier
- `bEnableAutonomousBehavior` - Toggle autonomy
- `Goals` - Active goals
- `BehaviorRules` - Behavior rules
- `KnowledgeAtomIDs` - Known atoms

**Key Methods**:
- `AddGoal(Goal)` - Add a goal
- `AddBehaviorRule(Rule)` - Add behavior rule
- `SelectBehavior()` - Choose next behavior
- `ExecuteBehavior(Name)` - Execute behavior
- `SendMessage(ReceiverID, Type, Content)` - Send message
- `IntegrateWithConvai(ChatbotComponent)` - Link with Convai

**Usage**:
```cpp
UOpenCogAgentComponent* Agent = Actor->FindComponentByClass<UOpenCogAgentComponent>();
FOpenCogGoal Goal = UOpenCogBlueprintLibrary::MakeGoal("Patrol", 0.7f);
Agent->AddGoal(Goal);
```

---

### UOpenCogOrchestrationManager (World Subsystem)
**Purpose**: Coordinate multiple agents

**Key Methods**:
- `RegisterAgent(Agent)` - Register agent
- `GetAllAgents()` - Get all agents
- `GetAgentsInRange(Location, Range)` - Find nearby agents
- `SendMessage(Message)` - Route message
- `BroadcastMessage(...)` - Send to all agents
- `CoordinateCollaboration(AgentIDs, GoalID)` - Setup collaboration
- `ShareKnowledge(SourceID, TargetID, AtomIDs)` - Share knowledge

**Usage**:
```cpp
UOpenCogOrchestrationManager* Manager = GetWorld()->GetSubsystem<UOpenCogOrchestrationManager>();
TArray<UOpenCogAgentComponent*> Agents = Manager->GetAllAgents();
```

---

### UOpenCogBlueprintLibrary (Blueprint Function Library)
**Purpose**: Blueprint-accessible utilities

**Key Functions**:
- `MakeGoal(Name, Priority, TargetValue)` - Create goal
- `MakeBehaviorRule(Name, Context, Action, GoalID, Weight)` - Create rule
- `MakeAtom(Type, Name, Strength, Confidence)` - Create atom
- `GetAtomSpace(WorldContext)` - Get AtomSpace subsystem
- `GetOrchestrationManager(WorldContext)` - Get manager subsystem
- `CreateCollaborativeGoal(Agents, GoalName, Priority)` - Create shared goal

---

## Data Structures

### FOpenCogAtom
```cpp
struct FOpenCogAtom {
    FString AtomID;           // Unique identifier
    FString AtomType;         // e.g., "ConceptNode", "PredicateNode"
    FString Name;             // Atom name
    float TruthStrength;      // [0.0, 1.0]
    float TruthConfidence;    // [0.0, 1.0]
    float AttentionValue;     // [0.0, 1.0]
    TArray<FString> OutgoingSet; // Connected atom IDs
};
```

### FOpenCogGoal
```cpp
struct FOpenCogGoal {
    FString GoalID;           // Unique identifier
    FString GoalName;         // Human-readable name
    float Priority;           // [0.0, 1.0] - higher = more important
    float Satisfaction;       // [0.0, 1.0] - current achievement level
    bool bIsActive;           // Is goal active
    float TargetValue;        // [0.0, 1.0] - desired satisfaction level
};
```

### FOpenCogBehaviorRule
```cpp
struct FOpenCogBehaviorRule {
    FString RuleID;           // Unique identifier
    FString RuleName;         // Human-readable name
    FString Context;          // When to apply (simple condition)
    FString Action;           // What to do
    FString GoalID;           // Goal this helps achieve
    float Weight;             // Rule importance multiplier
    bool bIsSatisfied;        // Has been executed successfully
};
```

### FOpenCogAgentMessage
```cpp
struct FOpenCogAgentMessage {
    FString SenderID;         // Sender agent ID
    FString ReceiverID;       // Receiver agent ID
    FString MessageType;      // Type (e.g., "Alert", "KnowledgeShare")
    FString Content;          // Message content
    float Timestamp;          // When sent
    float Priority;           // [0.0, 1.0] - message importance
};
```

---

## Common Patterns

### Pattern 1: Creating an Autonomous Agent
```cpp
// Add component
UOpenCogAgentComponent* Agent = Actor->AddComponentByClass(UOpenCogAgentComponent::StaticClass());
Agent->AgentName = "MyAgent";
Agent->bEnableAutonomousBehavior = true;

// Add goals
FOpenCogGoal Goal1 = UOpenCogBlueprintLibrary::MakeGoal("Survive", 0.9f);
Agent->AddGoal(Goal1);

// Add behavior rules
FOpenCogBehaviorRule Rule1 = UOpenCogBlueprintLibrary::MakeBehaviorRule(
    "SeekFood", "hungry", "FindFood", Goal1.GoalID, 1.0f
);
Agent->AddBehaviorRule(Rule1);
```

### Pattern 2: Multi-Agent Collaboration
```cpp
// Get manager
UOpenCogOrchestrationManager* Manager = GetWorld()->GetSubsystem<UOpenCogOrchestrationManager>();

// Enable collaboration
FOpenCogCoordinationParams Params;
Params.bEnableCollaboration = true;
Params.bShareKnowledge = true;
Manager->SetCoordinationParams(Params);

// Create shared goal
TArray<UOpenCogAgentComponent*> Agents = Manager->GetAllAgents();
UOpenCogBlueprintLibrary::CreateCollaborativeGoal(Agents, "DefendBase", 0.85f);
```

### Pattern 3: Knowledge Management
```cpp
// Get AtomSpace
UOpenCogAtomSpace* AtomSpace = GetWorld()->GetSubsystem<UOpenCogAtomSpace>();

// Add concept
FOpenCogAtom Enemy = UOpenCogBlueprintLibrary::MakeAtom("ConceptNode", "Orc");
FString EnemyID = AtomSpace->AddAtom(Enemy);

// Add location
FOpenCogAtom Location = UOpenCogBlueprintLibrary::MakeAtom("ConceptNode", "Cave");
FString LocationID = AtomSpace->AddAtom(Location);

// Link them
AtomSpace->CreateLink("LocationLink", EnemyID, LocationID, 0.9f, 1.0f);
```

### Pattern 4: Convai Integration
```cpp
// Get components
UOpenCogAgentComponent* Agent = Actor->FindComponentByClass<UOpenCogAgentComponent>();
UConvaiChatbotComponent* Chatbot = Actor->FindComponentByClass<UConvaiChatbotComponent>();

// Integrate
Agent->IntegrateWithConvai(Chatbot);

// Agent behaviors can now trigger Convai narrative triggers
// And Convai conversations can influence agent goals
```

### Pattern 5: Event Handling
```cpp
// Bind to events
Agent->OnBehaviorExecuted.AddDynamic(this, &AMyActor::HandleBehaviorExecuted);
Agent->OnGoalChanged.AddDynamic(this, &AMyActor::HandleGoalChanged);
Agent->OnMessageReceived.AddDynamic(this, &AMyActor::HandleMessageReceived);

// Event handlers
void AMyActor::HandleBehaviorExecuted(UOpenCogAgentComponent* Agent, const FString& BehaviorName) {
    UE_LOG(LogTemp, Log, TEXT("Behavior: %s"), *BehaviorName);
}

void AMyActor::HandleGoalChanged(UOpenCogAgentComponent* Agent, const FOpenCogGoal& Goal) {
    UE_LOG(LogTemp, Log, TEXT("Goal: %s - %.2f"), *Goal.GoalName, Goal.Satisfaction);
}

void AMyActor::HandleMessageReceived(UOpenCogAgentComponent* Agent, const FString& SenderID, const FOpenCogAgentMessage& Message) {
    UE_LOG(LogTemp, Log, TEXT("Message from %s: %s"), *SenderID, *Message.Content);
}
```

---

## Blueprint Quick Start

### 1. Add Agent Component
- Select your Actor
- Add Component → OpenCog Agent
- Set Agent Name
- Enable Autonomous Behavior

### 2. Configure in BeginPlay
```
Event BeginPlay
├─ Make Goal (Name: "MyGoal", Priority: 0.7)
│  └─ Add Goal (Target: OpenCog Agent)
│
└─ Make Behavior Rule (Name: "MyRule", Context: "idle", Action: "DoSomething", GoalID: "MyGoal")
   └─ Add Behavior Rule (Target: OpenCog Agent)
```

### 3. Handle Events
```
Event On Behavior Executed
└─ Branch (Behavior Name)
   ├─ "DoSomething" → Execute custom logic
   └─ Other → Do nothing
```

---

## Console Commands

```
# Enable verbose logging
Log LogOpenCog Verbose

# Display all logs
Log LogOpenCog All

# Disable OpenCog logs
Log LogOpenCog Off
```

---

## Performance Tips

1. **Agent Count**: Keep under 100 for best performance
2. **Update Interval**: Use 1.0s or higher for better performance
3. **Perception Range**: Smaller = better performance
4. **AtomSpace Size**: Max 10,000 atoms recommended
5. **Message Processing**: Limited to 5 per tick per agent

---

## Common Issues

### Agent Not Behaving
**Check**:
- `bEnableAutonomousBehavior` is true
- Goals are added and active
- Behavior rules match current context
- Component is ticking

### Messages Not Received
**Check**:
- Agents are within `CommunicationRange`
- Orchestration Manager is initialized
- Message ReceiverID is correct

### High Memory Usage
**Solutions**:
- Reduce `MaxAtoms` in AtomSpace
- Increase forgetting threshold
- Limit perception range
- Reduce agent count

### Goals Not Updating
**Check**:
- Goal satisfaction is being updated
- Priority is set correctly
- Goal is marked as active
- Behavior rules target the goal

---

## Integration Checklist

- [ ] Add OpenCog Agent Component to actors
- [ ] Configure goals and behavior rules
- [ ] Set up Orchestration Manager coordination params
- [ ] Integrate with Convai (optional)
- [ ] Import environment into AtomSpace
- [ ] Bind to agent events
- [ ] Test multi-agent scenarios
- [ ] Optimize performance settings
- [ ] Add logging for debugging

---

## Further Reading

- **Full Documentation**: [OPENCOG_INTEGRATION.md](OPENCOG_INTEGRATION.md)
- **Examples**: [OPENCOG_EXAMPLES.md](OPENCOG_EXAMPLES.md)
- **Convai Docs**: https://docs.convai.com
- **OpenCog Theory**: https://wiki.opencog.org

# OpenCog Integration for Convai Unreal Engine SDK

## Overview

This integration adds OpenCog autonomous multi-agent orchestration capabilities to the Convai Unreal Engine SDK. It enables:

- **Autonomous Agent Behavior**: AI-driven NPCs that pursue goals independently
- **Multi-Agent Coordination**: Agents collaborate, compete, and share knowledge
- **Knowledge Representation**: AtomSpace-based semantic knowledge storage
- **Emergent Behavior**: Complex group behaviors emerge from simple agent rules
- **Convai Integration**: Seamless integration with conversational AI

## Architecture

### Core Components

#### 1. OpenCog AtomSpace (`UOpenCogAtomSpace`)
- World subsystem managing knowledge representation
- Stores atoms (nodes and links) representing concepts, relationships, and facts
- Implements attention allocation and forgetting mechanisms
- Imports knowledge from Convai environments

**Key Features:**
- Atom creation, retrieval, and deletion
- Link creation between atoms
- Attention value management
- Automatic forgetting of low-attention atoms
- Integration with Convai environments

#### 2. OpenCog Agent Component (`UOpenCogAgentComponent`)
- Actor component that makes any actor an autonomous agent
- Manages goals, behaviors, and knowledge for individual agents
- Integrates with Convai chatbot for conversational capabilities

**Key Features:**
- Goal-driven behavior (OpenPsi-inspired)
- Behavior rule evaluation and execution
- Environment perception
- Inter-agent communication
- Convai chatbot integration

#### 3. OpenCog Orchestration Manager (`UOpenCogOrchestrationManager`)
- World subsystem coordinating all agents
- Manages multi-agent communication and collaboration
- Detects and facilitates emergent behaviors

**Key Features:**
- Agent registration and discovery
- Message routing between agents
- Collaborative goal coordination
- Knowledge sharing facilitation
- Emergent pattern detection

#### 4. Blueprint Library (`UOpenCogBlueprintLibrary`)
- Blueprint-accessible functions for easy integration
- Helper functions for creating goals, rules, and atoms
- Utilities for agent coordination

## Usage

### Setting Up an Autonomous Agent

1. **Add OpenCog Agent Component to your Actor:**

```cpp
// C++
UOpenCogAgentComponent* AgentComp = CreateDefaultSubobject<UOpenCogAgentComponent>(TEXT("OpenCogAgent"));
AgentComp->AgentName = TEXT("MyAgent");
AgentComp->bEnableAutonomousBehavior = true;
```

Or in Blueprint:
- Add `OpenCog Agent` component to your actor
- Set `Agent Name` and enable `Enable Autonomous Behavior`

2. **Define Goals:**

```cpp
// C++
FOpenCogGoal SurvivalGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("Survive"), 0.9f, 1.0f);
AgentComp->AddGoal(SurvivalGoal);
```

Or in Blueprint:
- Use `Make Goal` node
- Call `Add Goal` on your agent component

3. **Add Behavior Rules:**

```cpp
// C++
FOpenCogBehaviorRule SeekFoodRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
    TEXT("SeekFood"),
    TEXT("hungry"),
    TEXT("FindFood"),
    SurvivalGoal.GoalID,
    1.0f
);
AgentComp->AddBehaviorRule(SeekFoodRule);
```

Or in Blueprint:
- Use `Make Behavior Rule` node
- Call `Add Behavior Rule` on your agent component

4. **Integrate with Convai (Optional):**

```cpp
// C++
UConvaiChatbotComponent* ChatbotComp = GetComponentByClass<UConvaiChatbotComponent>();
AgentComp->IntegrateWithConvai(ChatbotComp);
```

### Multi-Agent Coordination

#### Enable Knowledge Sharing:

```cpp
// C++
UOpenCogOrchestrationManager* Manager = GetWorld()->GetSubsystem<UOpenCogOrchestrationManager>();

FOpenCogCoordinationParams Params;
Params.bEnableCollaboration = true;
Params.bShareKnowledge = true;
Params.CommunicationRange = 2000.0f;
Manager->SetCoordinationParams(Params);
```

#### Create Collaborative Goals:

```cpp
// Blueprint
TArray<UOpenCogAgentComponent*> Agents = GetAllAgents();
UOpenCogBlueprintLibrary::CreateCollaborativeGoal(Agents, TEXT("DefendBase"), 0.8f);
```

### Working with AtomSpace

#### Add Knowledge:

```cpp
// C++
UOpenCogAtomSpace* AtomSpace = GetWorld()->GetSubsystem<UOpenCogAtomSpace>();

FOpenCogAtom PlayerAtom = UOpenCogBlueprintLibrary::MakeAtom(
    TEXT("ConceptNode"),
    TEXT("Player"),
    1.0f,
    1.0f
);
FString PlayerAtomID = AtomSpace->AddAtom(PlayerAtom);

FOpenCogAtom NPCAtom = UOpenCogBlueprintLibrary::MakeAtom(
    TEXT("ConceptNode"),
    TEXT("NPC_Guard"),
    1.0f,
    1.0f
);
FString NPCAtomID = AtomSpace->AddAtom(NPCAtom);

// Create relationship
AtomSpace->CreateLink(TEXT("FriendshipLink"), PlayerAtomID, NPCAtomID, 0.8f, 0.9f);
```

#### Import from Convai Environment:

```cpp
// C++
UConvaiEnvironment* Environment = CreateConvaiEnvironment();
// ... populate environment with objects, characters, actions
AtomSpace->ImportFromConvaiEnvironment(Environment);
```

## Integration with Convai

The OpenCog system seamlessly integrates with Convai's conversational AI:

1. **Environment Perception**: Convai environments are automatically imported into AtomSpace
2. **Action Execution**: Agent behaviors can trigger Convai narrative triggers
3. **Conversational Goals**: Agent goals can be influenced by player conversations
4. **Knowledge Sharing**: Agents share knowledge learned from conversations

### Example Integration:

```cpp
// Agent perceives environment and updates AtomSpace
AgentComp->PerceiveEnvironment();

// Agent decides on behavior based on goals
FString Behavior = AgentComp->SelectBehavior();

// Execute behavior via Convai
if (ConvaiChatbot)
{
    ConvaiChatbot->ExecuteNarrativeTrigger(Behavior, Environment, true, true, false);
}
```

## Blueprint Examples

### Creating an Autonomous Guard

1. Create a character with both `OpenCog Agent` and `Convai Chatbot` components
2. Add goals:
   - "Patrol" (Priority: 0.5)
   - "Investigate" (Priority: 0.7)
   - "Defend" (Priority: 0.9)
3. Add behavior rules:
   - Context: "idle" → Action: "PatrolRoute"
   - Context: "nearby suspicious" → Action: "InvestigateArea"
   - Context: "nearby threat" → Action: "EngageThreat"
4. Integrate with Convai for voice commands and conversations

### Multi-Agent Patrol System

1. Create multiple agents with patrol goals
2. Enable collaboration in Orchestration Manager
3. Agents automatically:
   - Share discovered threats
   - Coordinate patrol routes
   - Form groups when threats detected
   - Communicate via Convai when near player

## Configuration

### Agent Component Properties

- `Agent Name`: Display name for the agent
- `Enable Autonomous Behavior`: Toggle autonomous operation
- `Behavior Update Interval`: How often to evaluate new behaviors (seconds)
- `Perception Range`: Range for environment perception (units)

### Coordination Parameters

- `Enable Collaboration`: Agents work together on shared goals
- `Enable Competition`: Agents compete for resources (not recommended with collaboration)
- `Communication Range`: Range for inter-agent communication
- `Share Knowledge`: Automatically share knowledge between nearby agents
- `Enable Emergent Behavior`: Allow complex group behaviors to emerge

## Advanced Features

### Custom Goal Evaluation

Override goal satisfaction evaluation:

```cpp
void UpdateGoalSatisfaction(const FString& GoalID, float NewSatisfaction)
{
    AgentComp->UpdateGoalSatisfaction(GoalID, NewSatisfaction);
}
```

### Direct Messaging

Send messages between specific agents:

```cpp
AgentComp->SendMessage(
    TargetAgentID,
    TEXT("AlertRequest"),
    TEXT("EnemySpotted"),
    0.9f
);
```

### Attention Mechanism

Focus agent attention on specific actors:

```cpp
AgentComp->SetAttentionTarget(ThreatActor);
```

## Performance Considerations

- **Agent Count**: System tested with up to 100 agents
- **Behavior Update Rate**: Default 1 second, adjust based on needs
- **AtomSpace Size**: Automatic forgetting keeps memory bounded (default max 10,000 atoms)
- **Message Processing**: Processes up to 5 messages per agent per tick

## Best Practices

1. **Goal Priorities**: Use 0-1 range, reserve 0.9+ for critical goals
2. **Behavior Rules**: Keep contexts simple and specific
3. **Knowledge Sharing**: Enable for cooperative scenarios, disable for competitive
4. **Integration**: Use Convai for dialogue, OpenCog for autonomous behavior
5. **Testing**: Start with few agents, gradually increase complexity

## Troubleshooting

**Agents not behaving autonomously:**
- Check `Enable Autonomous Behavior` is true
- Verify goals are added and active
- Ensure behavior rules match current context

**Messages not being received:**
- Verify agents are within `Communication Range`
- Check Orchestration Manager is initialized
- Enable verbose logging: `LogOpenCog All` in console

**High memory usage:**
- Reduce `Max Atoms` in AtomSpace
- Increase forgetting threshold
- Limit agent perception range

## API Reference

See header files for full API documentation:
- `OpenCogDefinitions.h` - Core data structures
- `OpenCogAtomSpace.h` - Knowledge representation
- `OpenCogAgentComponent.h` - Individual agent behavior
- `OpenCogOrchestrationManager.h` - Multi-agent coordination
- `OpenCogBlueprintLibrary.h` - Blueprint utilities

## Future Enhancements

Planned features:
- Learning and adaptation from experience
- Advanced planning algorithms (STRIPS-like)
- Emotional state integration
- Long-term memory persistence
- Distributed multi-agent scenarios
- VR/AR agent interaction

## Support

For issues and questions:
- GitHub: https://github.com/Conv-AI/Convai-UnrealEngine-SDK
- Discord: https://discord.gg/UVvBgV3xQ5
- Email: support@convai.com

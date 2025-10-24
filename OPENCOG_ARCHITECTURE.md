# OpenCog Multi-Agent Orchestration - Architecture Overview

## Executive Summary

This integration adds OpenCog-inspired autonomous multi-agent orchestration capabilities to the Convai Unreal Engine SDK. It enables developers to create intelligent NPCs that:

1. **Think Autonomously**: Agents pursue goals independently using behavior rules
2. **Collaborate**: Multiple agents coordinate to achieve shared objectives
3. **Learn and Remember**: AtomSpace provides semantic knowledge representation
4. **Communicate**: Inter-agent messaging enables complex group behaviors
5. **Converse**: Seamless integration with Convai's conversational AI

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Unreal Engine World                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │         OpenCog Orchestration Manager                │   │
│  │              (World Subsystem)                       │   │
│  │  • Agent Registration                                 │   │
│  │  • Message Routing                                    │   │
│  │  • Collaboration Coordination                         │   │
│  │  • Emergent Behavior Detection                        │   │
│  └────────────┬────────────────────────────┬─────────────┘   │
│               │                            │                  │
│  ┌────────────▼────────────────────────────▼─────────────┐   │
│  │              OpenCog AtomSpace                         │   │
│  │              (World Subsystem)                         │   │
│  │  • Knowledge Representation (Atoms & Links)            │   │
│  │  • Attention Allocation                                │   │
│  │  • Forgetting Mechanism                                │   │
│  │  • Convai Environment Import                           │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                               │
│  ┌───────────────────────────────────────────────────────┐   │
│  │                   NPCs (Actors)                        │   │
│  │  ┌─────────────────────────────────────────────────┐  │   │
│  │  │    OpenCog Agent Component                      │  │   │
│  │  │    • Goals (Priority-based)                     │  │   │
│  │  │    • Behavior Rules (Context → Action)          │  │   │
│  │  │    • Perception System                           │  │   │
│  │  │    • Message Handling                            │  │   │
│  │  │    • Autonomous Decision Making                  │  │   │
│  │  └─────────────────────────────────────────────────┘  │   │
│  │  ┌─────────────────────────────────────────────────┐  │   │
│  │  │    Convai Chatbot Component (Optional)          │  │   │
│  │  │    • Conversational AI                           │  │   │
│  │  │    • Voice Interaction                           │  │   │
│  │  │    • Narrative Triggers                          │  │   │
│  │  └─────────────────────────────────────────────────┘  │   │
│  └───────────────────────────────────────────────────────┘   │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. OpenCog AtomSpace (Knowledge Layer)

**Inspiration**: OpenCog's hypergraph knowledge representation

**Implementation**:
- `UOpenCogAtomSpace` - World subsystem
- Stores atoms (nodes) and links (edges)
- Truth values for probabilistic reasoning
- Attention values for importance tracking
- Automatic forgetting of low-attention atoms

**Key Features**:
- Semantic knowledge storage
- Relationship modeling
- Attention diffusion algorithm
- Integration with Convai environments

### 2. OpenCog Agent Component (Behavior Layer)

**Inspiration**: OpenPsi goal-driven behavior framework

**Implementation**:
- `UOpenCogAgentComponent` - Actor component
- Goal management with priority system
- Behavior rule evaluation engine
- Context-sensitive action selection
- Environment perception

**Key Features**:
- Autonomous decision making
- Goal satisfaction tracking
- Dynamic behavior selection
- Convai integration for speech

### 3. Orchestration Manager (Coordination Layer)

**Inspiration**: Multi-agent systems and swarm intelligence

**Implementation**:
- `UOpenCogOrchestrationManager` - World subsystem
- Agent registry and discovery
- Message routing system
- Collaboration coordinator
- Emergent pattern detector

**Key Features**:
- Multi-agent communication
- Knowledge sharing
- Collaborative goal achievement
- Emergent behavior detection

## Information Flow

### 1. Agent Perception → Knowledge
```
Environment
    ↓
Agent Perception System
    ↓
AtomSpace (Knowledge Storage)
    ↓
Agent Knowledge Base
```

### 2. Goal-Driven Behavior
```
Goals (Priority-sorted)
    ↓
Context Evaluation
    ↓
Behavior Rule Selection
    ↓
Action Execution
    ↓
Goal Satisfaction Update
```

### 3. Multi-Agent Interaction
```
Agent A Decision
    ↓
Message to Orchestrator
    ↓
Orchestrator Routing
    ↓
Message to Agent B
    ↓
Agent B Response
```

### 4. Convai Integration
```
Player Voice Input
    ↓
Convai Speech Recognition
    ↓
Conversation Processing
    ↓
Agent Goal Update
    ↓
Behavior Selection
    ↓
Convai Narrative Trigger
    ↓
Agent Voice Output
```

## Design Patterns

### 1. Goal-Driven Architecture
- Agents have multiple concurrent goals
- Goals have priorities and satisfaction levels
- Behaviors target specific goals
- Unsatisfied high-priority goals drive action selection

### 2. Rule-Based Behavior Selection
- Behavior rules: `Context → Action`
- Context evaluation using simple conditions
- Rule weights influence selection
- Most relevant rule wins

### 3. Attention Allocation
- Atoms have attention values
- High-attention atoms spread activation
- Low-attention atoms are forgotten
- Focuses processing on relevant knowledge

### 4. Emergent Coordination
- No centralized controller
- Agents follow local rules
- Global patterns emerge from interactions
- Examples: flocking, market formation, defense coordination

## Scalability Considerations

### Memory Management
- **AtomSpace**: Max 10,000 atoms by default
- **Forgetting**: Automatic cleanup of low-attention atoms
- **Agent Knowledge**: Limited to most relevant atoms

### Performance Optimization
- **Agent Ticking**: Configurable update intervals
- **Message Batching**: Up to 5 messages per tick
- **Perception Range**: Limits environment scanning
- **Subsystem Ticking**: Efficient world subsystems

### Recommended Limits
| System Tier | Max Agents | Update Interval | Perception Range |
|-------------|-----------|-----------------|------------------|
| Low-End     | 20-30     | 2.0s            | 1500 units       |
| Mid-Range   | 50-70     | 1.0s            | 2000 units       |
| High-End    | 100+      | 0.5s            | 2500 units       |

## Integration Points

### 1. Convai SDK Integration
- **Environment Import**: Convai objects/characters → AtomSpace atoms
- **Action Execution**: Agent behaviors → Convai narrative triggers
- **Conversation Influence**: Player dialogue → Agent goal updates
- **Speech Output**: Agent decisions → Convai TTS

### 2. Blueprint Integration
- `UOpenCogBlueprintLibrary` - Helper functions
- Blueprint-exposed events and delegates
- Visual scripting support for all features
- Example configurations provided

### 3. C++ Integration
- Full API access through header files
- Example character class (`AOpenCogExampleCharacter`)
- Event binding for custom logic
- Direct subsystem access

## Use Cases

### 1. Intelligent NPCs
- Guards that patrol, investigate, and alert
- Merchants that greet, sell, and restock
- Quest givers that offer appropriate quests
- Companions that assist and follow

### 2. Dynamic Encounters
- Enemy squads that coordinate attacks
- Villagers that flee from threats
- Animals that hunt in packs
- NPCs that form social groups

### 3. Living Worlds
- Markets that form at gathering spots
- Guards that patrol and coordinate
- Civilians that go about daily routines
- Emergent social behaviors

### 4. Conversational Gameplay
- NPCs remember past conversations (via AtomSpace)
- Goal-driven dialogue (context-aware responses)
- Multi-NPC conversations
- Player influence on NPC behavior

## Comparison with Traditional AI

| Feature | Traditional AI | OpenCog Integration |
|---------|---------------|---------------------|
| Behavior | FSM/Behavior Trees | Goal-driven rules |
| Knowledge | Hard-coded | Dynamic AtomSpace |
| Coordination | Scripted | Emergent |
| Conversation | Dialogue trees | Convai + Goals |
| Learning | Static | Attention-based |
| Scalability | Per-NPC | Shared knowledge |

## Extension Points

### Custom Goal Types
```cpp
// Extend FOpenCogGoal with custom fields
USTRUCT(BlueprintType)
struct FMyCustomGoal : public FOpenCogGoal {
    GENERATED_BODY()
    // Custom fields
};
```

### Custom Behavior Evaluation
```cpp
// Override context evaluation
bool EvaluateContext(const FString& Context) const override {
    // Custom evaluation logic
    return MyEvaluationFunction(Context);
}
```

### Custom Atom Types
```cpp
// Create domain-specific atom types
FOpenCogAtom QuestAtom;
QuestAtom.AtomType = "QuestNode";
QuestAtom.Name = "DragonSlayer";
// Add custom properties via links
```

## Future Enhancements

### Planned Features
1. **Learning from Experience**: Adapt behavior based on outcomes
2. **Advanced Planning**: STRIPS-like planning algorithm
3. **Emotional States**: Integration with emotion system
4. **Long-term Memory**: Persistent AtomSpace storage
5. **Distributed Agents**: Network-synchronized multi-agent

### Research Directions
1. **Procedural Goal Generation**: Create goals dynamically
2. **Social Relationships**: Model NPC relationships in AtomSpace
3. **Cultural Evolution**: Group behaviors evolve over time
4. **Natural Language**: Parse player commands into goals

## Documentation Structure

1. **OPENCOG_INTEGRATION.md** - Complete integration guide
2. **OPENCOG_EXAMPLES.md** - Configuration examples
3. **OPENCOG_QUICK_REFERENCE.md** - API quick reference
4. **This Document** - Architecture overview

## Getting Started

1. **Read**: OPENCOG_INTEGRATION.md
2. **Study**: OPENCOG_EXAMPLES.md
3. **Reference**: OPENCOG_QUICK_REFERENCE.md
4. **Implement**: Use AOpenCogExampleCharacter as template
5. **Customize**: Adapt to your game's needs

## Support and Resources

- **GitHub**: https://github.com/Conv-AI/Convai-UnrealEngine-SDK
- **Convai Docs**: https://docs.convai.com
- **Discord**: https://discord.gg/UVvBgV3xQ5
- **Email**: support@convai.com

## Credits

**Based on**:
- OpenCog AGI Framework: https://opencog.org
- OpenPsi Motivational System
- Attention Allocation Theory

**Integrated with**:
- Convai Conversational AI: https://convai.com
- Unreal Engine 5

**License**: See LICENSE.md for plugin licensing terms

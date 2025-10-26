# OpenCog Agent Configuration Examples

## Example 1: Simple Patrol Guard

### Agent Setup
- **Agent Name**: "PatrolGuard"
- **Enable Autonomous Behavior**: true
- **Behavior Update Interval**: 1.0
- **Perception Range**: 2000.0

### Goals
1. **Patrol** (Priority: 0.5)
   - GoalID: "Goal_Patrol"
   - Target Value: 1.0
   - Active: true

2. **Investigate** (Priority: 0.7)
   - GoalID: "Goal_Investigate"
   - Target Value: 1.0
   - Active: true

3. **Alert** (Priority: 0.9)
   - GoalID: "Goal_Alert"
   - Target Value: 1.0
   - Active: true

### Behavior Rules
1. **Idle Patrol**
   - Rule Name: "IdlePatrol"
   - Context: "idle"
   - Action: "StartPatrol"
   - Goal ID: "Goal_Patrol"
   - Weight: 1.0

2. **Investigate Sound**
   - Rule Name: "InvestigateSound"
   - Context: "nearby suspicious"
   - Action: "InvestigateArea"
   - Goal ID: "Goal_Investigate"
   - Weight: 1.5

3. **Alert Team**
   - Rule Name: "AlertTeam"
   - Context: "nearby threat"
   - Action: "AlertNearbyGuards"
   - Goal ID: "Goal_Alert"
   - Weight: 2.0

### Convai Integration
- Character ID: [Your Convai Character ID]
- Enable voice responses for alerts
- Use narrative triggers for patrol waypoints

---

## Example 2: Collaborative Team (3 Agents)

### Coordination Parameters
```
bEnableCollaboration: true
bEnableCompetition: false
CommunicationRange: 3000.0
bShareKnowledge: true
bEnableEmergentBehavior: true
```

### Shared Goal
- **Goal Name**: "DefendArea"
- **Priority**: 0.85
- **Assigned to**: All 3 agents

### Agent-Specific Roles

#### Agent 1: "TeamLeader"
- Additional Goal: "CoordinateTeam" (Priority: 0.8)
- Behavior: Broadcasts alerts to team
- Higher perception range: 2500.0

#### Agent 2: "Scout"
- Additional Goal: "GatherIntel" (Priority: 0.7)
- Behavior: Shares discovered threats
- Higher movement speed

#### Agent 3: "Support"
- Additional Goal: "AssistTeam" (Priority: 0.75)
- Behavior: Responds to teammate requests
- Larger communication range: 3500.0

---

## Example 3: Knowledge-Based Merchant

### Agent Setup
- **Agent Name**: "Merchant"
- **Enable Autonomous Behavior**: true
- **Behavior Update Interval**: 2.0
- **Perception Range**: 1500.0

### AtomSpace Knowledge
```cpp
// Product atoms
MakeAtom("ConceptNode", "Sword", 1.0, 1.0)
MakeAtom("ConceptNode", "Shield", 1.0, 1.0)
MakeAtom("ConceptNode", "Potion", 1.0, 1.0)

// Price links
CreateLink("HasPrice", "Sword", "100Gold", 1.0, 1.0)
CreateLink("HasPrice", "Shield", "75Gold", 1.0, 1.0)
CreateLink("HasPrice", "Potion", "20Gold", 1.0, 1.0)

// Stock links
CreateLink("InStock", "Sword", "5", 1.0, 1.0)
CreateLink("InStock", "Shield", "3", 1.0, 1.0)
CreateLink("InStock", "Potion", "20", 1.0, 1.0)
```

### Goals
1. **Sell Items** (Priority: 0.8)
   - Satisfaction increases when items sold
   
2. **Restock** (Priority: 0.6)
   - Triggers when stock low
   
3. **Greet Customers** (Priority: 0.5)
   - When player nearby

### Behavior Rules
1. **Greet Customer**
   - Context: "nearby player"
   - Action: "GreetPlayer"
   - Triggers Convai conversation

2. **Offer Deals**
   - Context: "in conversation"
   - Action: "OfferSpecials"
   - Uses Convai TTS

3. **Request Restock**
   - Context: "low inventory"
   - Action: "RequestSupplies"
   - Sends message to supply agent

---

## Example 4: Dynamic Quest Giver

### Agent Setup
- **Agent Name**: "QuestMaster"
- **Enable Autonomous Behavior**: true
- **Behavior Update Interval**: 3.0
- **Perception Range**: 2000.0

### Knowledge Base
```cpp
// Quest atoms
MakeAtom("ConceptNode", "Quest_DragonSlayer", 1.0, 1.0)
MakeAtom("ConceptNode", "Quest_LostArtifact", 1.0, 1.0)
MakeAtom("ConceptNode", "Quest_EscortCaravan", 1.0, 1.0)

// Prerequisites
CreateLink("Requires", "Quest_DragonSlayer", "Level10", 0.9, 1.0)
CreateLink("Requires", "Quest_LostArtifact", "Completed_Quest_Tutorial", 1.0, 1.0)

// Rewards
CreateLink("Rewards", "Quest_DragonSlayer", "LegendarySword", 1.0, 1.0)
CreateLink("Rewards", "Quest_LostArtifact", "AncientKnowledge", 1.0, 1.0)
```

### Goals
1. **Distribute Quests** (Priority: 0.7)
2. **Track Progress** (Priority: 0.6)
3. **Grant Rewards** (Priority: 0.8)

### Behavior Rules
1. **Offer Suitable Quest**
   - Context: "nearby player && player_eligible"
   - Action: "OfferQuest"
   - Evaluates player level/completed quests via AtomSpace

2. **Check Progress**
   - Context: "player_has_active_quest"
   - Action: "QueryProgress"
   - Uses Convai to discuss quest status

3. **Complete Quest**
   - Context: "player_completed_quest"
   - Action: "GrantReward"
   - Updates AtomSpace with completion

---

## Example 5: Emergent Behavior - Village Simulation

### Setup
- 10 agents: 5 villagers, 3 guards, 1 merchant, 1 quest giver
- All have OpenCog components
- Coordination enabled with knowledge sharing

### Emergent Behaviors Observed

#### 1. Market Formation
- Merchant moves to central location
- Villagers gather around merchant
- Guards patrol perimeter
- Pattern: "Market_Gathering"

#### 2. Threat Response
- Guard detects threat
- Broadcasts alert
- Other guards converge
- Villagers flee to safe zones
- Pattern: "Coordinated_Defense"

#### 3. Social Clustering
- Agents with similar goals cluster
- Knowledge sharing intensifies
- Group behaviors emerge
- Pattern: "Social_Groups"

### Configuration
```cpp
// Orchestration Manager Settings
CoordinationParams.bEnableCollaboration = true;
CoordinationParams.CommunicationRange = 2500.0;
CoordinationParams.bShareKnowledge = true;
CoordinationParams.bEnableEmergentBehavior = true;

// All agents enabled
All agents: bEnableAutonomousBehavior = true
```

---

## Blueprint Implementation Tips

### 1. Creating Goals in Blueprint
```
Event BeginPlay
-> Make Goal (Goal Name: "Patrol", Priority: 0.5)
-> Add Goal (Target: OpenCog Agent Component)
```

### 2. Responding to Behavior Execution
```
Event On Behavior Executed (OpenCog Agent Component)
-> Branch (Behavior Name == "StartPatrol")
   -> True: Set Patrol Active = true
   -> False: Check other behaviors
```

### 3. Integrating with Convai
```
Event BeginPlay
-> Get Component By Class (Class: Convai Chatbot Component)
-> Integrate With Convai (Target: OpenCog Agent Component)
```

### 4. Creating Collaborative Goals
```
Event Game Start
-> Get All Actors of Class (Class: Guard)
-> For Each Actor
   -> Get Component By Class (OpenCog Agent Component)
   -> Add to Array
-> Create Collaborative Goal (Agents: Agent Array, Goal Name: "DefendCastle", Priority: 0.9)
```

### 5. Monitoring Agent Status
```
Event Tick
-> Get Top Priority Goal (OpenCog Agent Component)
-> Print String (Goal Name + " - Satisfaction: " + Satisfaction)
```

---

## Performance Tuning

### Low-End Systems (30-60 FPS target)
- Max Agents: 20-30
- Behavior Update Interval: 2.0 seconds
- Perception Range: 1500 units
- Max Atoms: 5000
- Disable emergent behavior detection

### Mid-Range Systems (60-90 FPS target)
- Max Agents: 50-70
- Behavior Update Interval: 1.0 seconds
- Perception Range: 2000 units
- Max Atoms: 10000
- Enable basic emergent behavior

### High-End Systems (90+ FPS target)
- Max Agents: 100+
- Behavior Update Interval: 0.5 seconds
- Perception Range: 2500 units
- Max Atoms: 20000
- Full emergent behavior enabled

---

## Debugging

### Enable Verbose Logging
```
Console Command: Log LogOpenCog Verbose
```

### Monitor Agent Count
```cpp
UOpenCogOrchestrationManager* Manager = GetOrchestrationManager(this);
TArray<UOpenCogAgentComponent*> Agents = Manager->GetAllAgents();
UE_LOG(LogTemp, Log, TEXT("Active Agents: %d"), Agents.Num());
```

### Check AtomSpace Size
```cpp
UOpenCogAtomSpace* AtomSpace = GetAtomSpace(this);
int32 AtomCount = AtomSpace->GetAtomCount();
UE_LOG(LogTemp, Log, TEXT("Total Atoms: %d"), AtomCount);
```

### Monitor Message Queue
```
Check orchestration manager tick logs for message processing stats
```

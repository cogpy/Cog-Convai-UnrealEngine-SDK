// Copyright 2022 Convai Inc. All Rights Reserved.

#include "OpenCogExampleCharacter.h"
#include "OpenCogAgentComponent.h"
#include "OpenCogBlueprintLibrary.h"
#include "ConvaiChatbotComponent.h"

AOpenCogExampleCharacter::AOpenCogExampleCharacter()
{
	// Create OpenCog Agent Component
	OpenCogAgent = CreateDefaultSubobject<UOpenCogAgentComponent>(TEXT("OpenCogAgent"));
	OpenCogAgent->AgentName = TEXT("ExampleAgent");
	OpenCogAgent->bEnableAutonomousBehavior = true;
	OpenCogAgent->BehaviorUpdateInterval = 1.0f;
	OpenCogAgent->PerceptionRange = 2000.0f;

	// Create Convai Chatbot Component
	ConvaiChatbot = CreateDefaultSubobject<UConvaiChatbotComponent>(TEXT("ConvaiChatbot"));
}

void AOpenCogExampleCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind to OpenCog events
	if (OpenCogAgent)
	{
		OpenCogAgent->OnBehaviorExecuted.AddDynamic(this, &AOpenCogExampleCharacter::OnBehaviorExecuted);
		OpenCogAgent->OnGoalChanged.AddDynamic(this, &AOpenCogExampleCharacter::OnGoalChanged);
		OpenCogAgent->OnMessageReceived.AddDynamic(this, &AOpenCogExampleCharacter::OnMessageReceived);

		// Integrate with Convai
		if (ConvaiChatbot)
		{
			OpenCogAgent->IntegrateWithConvai(ConvaiChatbot);
		}
	}

	// Example: Configure as patrol guard by default
	// You can override this in Blueprint or call different configuration functions
	ConfigureAsPatrolGuard();
}

void AOpenCogExampleCharacter::ConfigureAsPatrolGuard()
{
	if (!OpenCogAgent)
		return;

	UE_LOG(OpenCogLog, Log, TEXT("Configuring %s as Patrol Guard"), *GetName());

	// Clear existing goals and rules
	OpenCogAgent->Goals.Empty();
	OpenCogAgent->BehaviorRules.Empty();

	// Add goals
	FOpenCogGoal PatrolGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("Patrol"), 0.5f, 1.0f);
	OpenCogAgent->AddGoal(PatrolGoal);

	FOpenCogGoal InvestigateGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("Investigate"), 0.7f, 1.0f);
	OpenCogAgent->AddGoal(InvestigateGoal);

	FOpenCogGoal AlertGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("Alert"), 0.9f, 1.0f);
	OpenCogAgent->AddGoal(AlertGoal);

	// Add behavior rules
	FOpenCogBehaviorRule IdlePatrolRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("IdlePatrol"),
		TEXT("idle"),
		TEXT("StartPatrol"),
		PatrolGoal.GoalID,
		1.0f
	);
	OpenCogAgent->AddBehaviorRule(IdlePatrolRule);

	FOpenCogBehaviorRule InvestigateRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("InvestigateSound"),
		TEXT("nearby suspicious"),
		TEXT("InvestigateArea"),
		InvestigateGoal.GoalID,
		1.5f
	);
	OpenCogAgent->AddBehaviorRule(InvestigateRule);

	FOpenCogBehaviorRule AlertRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("AlertTeam"),
		TEXT("nearby threat"),
		TEXT("AlertNearbyGuards"),
		AlertGoal.GoalID,
		2.0f
	);
	OpenCogAgent->AddBehaviorRule(AlertRule);
}

void AOpenCogExampleCharacter::ConfigureAsMerchant()
{
	if (!OpenCogAgent)
		return;

	UE_LOG(OpenCogLog, Log, TEXT("Configuring %s as Merchant"), *GetName());

	// Clear existing goals and rules
	OpenCogAgent->Goals.Empty();
	OpenCogAgent->BehaviorRules.Empty();

	// Add merchant goals
	FOpenCogGoal SellGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("SellItems"), 0.8f, 1.0f);
	OpenCogAgent->AddGoal(SellGoal);

	FOpenCogGoal RestockGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("Restock"), 0.6f, 1.0f);
	OpenCogAgent->AddGoal(RestockGoal);

	FOpenCogGoal GreetGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("GreetCustomers"), 0.5f, 1.0f);
	OpenCogAgent->AddGoal(GreetGoal);

	// Add merchant behavior rules
	FOpenCogBehaviorRule GreetRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("GreetCustomer"),
		TEXT("nearby player"),
		TEXT("GreetPlayer"),
		GreetGoal.GoalID,
		1.0f
	);
	OpenCogAgent->AddBehaviorRule(GreetRule);

	FOpenCogBehaviorRule OfferDealsRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("OfferDeals"),
		TEXT("idle"),
		TEXT("OfferSpecials"),
		SellGoal.GoalID,
		1.2f
	);
	OpenCogAgent->AddBehaviorRule(OfferDealsRule);

	// Slower behavior updates for merchant
	OpenCogAgent->BehaviorUpdateInterval = 2.0f;
}

void AOpenCogExampleCharacter::ConfigureAsQuestGiver()
{
	if (!OpenCogAgent)
		return;

	UE_LOG(OpenCogLog, Log, TEXT("Configuring %s as Quest Giver"), *GetName());

	// Clear existing goals and rules
	OpenCogAgent->Goals.Empty();
	OpenCogAgent->BehaviorRules.Empty();

	// Add quest giver goals
	FOpenCogGoal DistributeQuestsGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("DistributeQuests"), 0.7f, 1.0f);
	OpenCogAgent->AddGoal(DistributeQuestsGoal);

	FOpenCogGoal TrackProgressGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("TrackProgress"), 0.6f, 1.0f);
	OpenCogAgent->AddGoal(TrackProgressGoal);

	FOpenCogGoal GrantRewardsGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("GrantRewards"), 0.8f, 1.0f);
	OpenCogAgent->AddGoal(GrantRewardsGoal);

	// Add quest giver behavior rules
	FOpenCogBehaviorRule OfferQuestRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("OfferSuitableQuest"),
		TEXT("nearby player"),
		TEXT("OfferQuest"),
		DistributeQuestsGoal.GoalID,
		1.5f
	);
	OpenCogAgent->AddBehaviorRule(OfferQuestRule);

	FOpenCogBehaviorRule CheckProgressRule = UOpenCogBlueprintLibrary::MakeBehaviorRule(
		TEXT("CheckProgress"),
		TEXT("nearby player"),
		TEXT("QueryProgress"),
		TrackProgressGoal.GoalID,
		1.2f
	);
	OpenCogAgent->AddBehaviorRule(CheckProgressRule);

	// Longer update interval for quest giver
	OpenCogAgent->BehaviorUpdateInterval = 3.0f;
}

void AOpenCogExampleCharacter::OnBehaviorExecuted(UOpenCogAgentComponent* Agent, const FString& BehaviorName)
{
	UE_LOG(OpenCogLog, Log, TEXT("%s executed behavior: %s"), *GetName(), *BehaviorName);

	// Handle specific behaviors
	if (BehaviorName == TEXT("StartPatrol"))
	{
		// Start patrol logic here
		// This could trigger animation, movement, etc.
	}
	else if (BehaviorName == TEXT("AlertNearbyGuards"))
	{
		// Send alert messages to nearby guard agents
		if (Agent)
		{
			Agent->SendMessage(
				TEXT("ALL_GUARDS"),
				TEXT("Alert"),
				TEXT("Threat detected!"),
				0.9f
			);
		}
	}
	else if (BehaviorName == TEXT("GreetPlayer"))
	{
		// Use Convai to greet the player
		if (ConvaiChatbot)
		{
			ConvaiChatbot->ExecuteNarrativeTrigger(
				TEXT("Greeting"),
				nullptr,
				false,
				true,
				false
			);
		}
	}
}

void AOpenCogExampleCharacter::OnGoalChanged(UOpenCogAgentComponent* Agent, const FOpenCogGoal& Goal)
{
	UE_LOG(OpenCogLog, Verbose, TEXT("%s goal changed: %s (Satisfaction: %.2f)"), 
		*GetName(), *Goal.GoalName, Goal.Satisfaction);

	// React to goal changes
	// For example, update UI, change animations, etc.
}

void AOpenCogExampleCharacter::OnMessageReceived(UOpenCogAgentComponent* Agent, const FString& SenderID, const FOpenCogAgentMessage& Message)
{
	UE_LOG(OpenCogLog, Log, TEXT("%s received message from %s: %s - %s"), 
		*GetName(), *SenderID, *Message.MessageType, *Message.Content);

	// Handle different message types
	if (Message.MessageType == TEXT("Alert"))
	{
		// React to alert (e.g., go to alert state, change behavior)
		FOpenCogGoal AlertGoal = UOpenCogBlueprintLibrary::MakeGoal(TEXT("RespondToAlert"), 0.95f, 1.0f);
		if (Agent)
		{
			Agent->AddGoal(AlertGoal);
		}
	}
	else if (Message.MessageType == TEXT("KnowledgeShare"))
	{
		// Process shared knowledge
		// Could update AtomSpace with new information
	}
}

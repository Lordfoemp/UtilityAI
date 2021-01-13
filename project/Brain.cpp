#include "stdafx.h"
#include "Brain.h"

#include "FMatrix.h"
#include <fstream>
//#include "Exam_HelperStructs.h"
#include <filesystem>
#include <iomanip>
#include "Exam_HelperStructs.h"
#include "AgentNormal.h"

#include "EBehaviorTree.h"
#include "EBlackboard.h"

#include "Behaviours.h"

Brain::Brain()
	 : m_pInterface{ nullptr }
	 , m_Target{}
	 , m_RunTimer{}
	 , m_RunTimerMax{ 10.5f }
	 , m_CanRun{ false }
     , m_BehaviorType{ BehaviorType::WANDER }
	 , m_pUtility{ nullptr }
	 , m_WanderTimer{ 1.f }
	 , m_CheckForItemsTimer{ 7.5f }
	 , m_ExitHouseTimer{ 5.f }
	 , m_AttackTimer{ 5.f }
	 , m_FleeTimer{ 5.f }
	 , m_EnterHouseTimer{ 10.f }
	 , m_HouseLocationsKnown{ std::vector<Elite::Vector2>() }
{}
Brain::~Brain()
{
	SAFE_DELETE(m_pUtility);
	SAFE_DELETE(m_pBlackboard);
}

void Brain::Initialize(IExamInterface* pInterface, AgentNormal* pAgent, Inventory* pInventory)
{
	m_pInterface = pInterface;

	m_pBlackboard = CreateBlackboard(pAgent, pInventory);

	m_pUtility = new Utility();
	m_pUtility->Initialize(m_pBlackboard);
}
SteeringPlugin_Output Brain::Update(const float elapsedSec, const std::vector<HouseInfo>& housesInFOV, const std::vector<EntityInfo>& entitiesInFOV, Inventory* pInventory)
{
	AgentInfo agent{ m_pInterface->Agent_GetInfo() };
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	/* UTILITY */
	if (GetBehaviorType() != BehaviorType::MOUSE)
	{
		m_WanderTimer.Update(elapsedSec);
		m_CheckForItemsTimer.Update(elapsedSec);
		m_ExitHouseTimer.Update(elapsedSec);
		m_AttackTimer.Update(elapsedSec);
		m_FleeTimer.Update(elapsedSec);
		m_EnterHouseTimer.Update(elapsedSec);
		
		//if (m_CheckForItemsTimer.Done())
		m_BehaviorType = m_pUtility->Reasoner();
	}

	if ((agent.Bitten || agent.WasBitten) && Elite::AreEqual(m_RunTimer, 0.f))
	{
		m_CanRun = true;
	}
	else
	{
		if (m_RunTimer < m_RunTimerMax)
			m_RunTimer += elapsedSec;
		else
		{
			m_CanRun = false;
			m_RunTimer = 0;
		}
	}

	if (GetBehaviorType() == BehaviorType::MOUSE)
		return UpdateBehaviour_Mouse();
	else if (GetBehaviorType() == BehaviorType::ENEMY_FLEE_NORMAL || 
			 GetBehaviorType() == BehaviorType::ENEMY_FLEE_PURGE ||
			 GetBehaviorType() == BehaviorType::ENEMY_FLEE_BITTEN)
	{
		steering = UpdateState_Flee(elapsedSec, entitiesInFOV);

		if (agent.IsInHouse)
			SetBehaviorType(BehaviorType::HOUSE_EXIT);

		if (!m_FleeTimer.IsTiming())
			 m_FleeTimer.Start();
	}
	else if (GetBehaviorType() == BehaviorType::ENEMY_FLEE_HOUSE)
	{
		steering = UpdateState_ExitHouse(elapsedSec);

		if (!m_ExitHouseTimer.IsTiming())
			 m_ExitHouseTimer.Start();
	}
	else if (GetBehaviorType() == BehaviorType::STAND_FIGHT)
	{
		steering = UpdateState_StandNFight(elapsedSec, entitiesInFOV, pInventory);

		if (!m_AttackTimer.IsTiming())
			 m_AttackTimer.Start();
	}
	else if (GetBehaviorType() == BehaviorType::HOUSE_ENTER)
	{
		steering = UpdateState_House(elapsedSec);

		if (!m_EnterHouseTimer.IsTiming())
			 m_EnterHouseTimer.Start();
	}
	else if (GetBehaviorType() == BehaviorType::HOUSE_CHECK)
	{
		steering = UpdateBehaviour_Items(elapsedSec, entitiesInFOV);

		if (!m_CheckForItemsTimer.IsTiming())
			 m_CheckForItemsTimer.Start();
	}
	else if (GetBehaviorType() == BehaviorType::HOUSE_ITEM)
		return UpdateState_Item(elapsedSec);
	else if (GetBehaviorType() == BehaviorType::HOUSE_EXIT)
	{
		steering = UpdateState_ExitHouse(elapsedSec);
		
		if (!m_ExitHouseTimer.IsTiming())
			 m_ExitHouseTimer.Start();
	}	
	else if (GetBehaviorType() == BehaviorType::WANDER)
	{		
		steering = UpdateBehaviour_Wander(elapsedSec, m_CanRun);

		if (!m_WanderTimer.IsTiming())
			 m_WanderTimer.Start();
	}

	return steering;
}
void Brain::Render(const float elapsedSec)
{
	// Draw the target
	m_pInterface->Draw_SolidCircle(m_Target, 1.f, Elite::Vector2(1, 1), Elite::Vector3(0.5f, 1, 0.5f));
}

Elite::Blackboard* Brain::CreateBlackboard(AgentNormal* pAgent, Inventory* pInventory)
{
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	pBlackboard->AddData("Interface", m_pInterface);
	pBlackboard->AddData("Agent", pAgent);
	pBlackboard->AddData("Brain", this);
	pBlackboard->AddData("entitiesInFOV", &std::vector<EntityInfo>());
	pBlackboard->AddData("housesInFOV", &std::vector<HouseInfo>());
	pBlackboard->AddData("inventory", pInventory);
	pBlackboard->AddData("CheckForItemsTimer", &m_CheckForItemsTimer);
	pBlackboard->AddData("ExitHouseTimer", &m_ExitHouseTimer);
	pBlackboard->AddData("AttackTimer", &m_AttackTimer);
	pBlackboard->AddData("FleeTimer", &m_FleeTimer);
	pBlackboard->AddData("EnterHouseTimer", &m_EnterHouseTimer);
	return pBlackboard;
}

void Brain::AddHouseLocation(const Elite::Vector2& houseLocation)
{
	// Check if the agent knows that location already
	bool houseKnown{ false };
	size_t houseLocationsSize{ m_HouseLocationsKnown.size() };
	for (size_t i = 0; i < houseLocationsSize; ++i)
	{
		if (Elite::AreEqual(houseLocation.x, m_HouseLocationsKnown[i].x) &&
			Elite::AreEqual(houseLocation.y, m_HouseLocationsKnown[i].y))
		{
			houseKnown = true;
			break;
		}
	}

	// If the agent does not know the location then add it
	if (!houseKnown)
		m_HouseLocationsKnown.push_back(houseLocation);
}

SteeringPlugin_Output Brain::UpdateBehaviour_Mouse()
{
	AgentInfo agent{ m_pInterface->Agent_GetInfo() };
	SteeringPlugin_Output steering = SteeringPlugin_Output();
	// ------------------------------------------
	// Check how far the agent is from his target
	// ------------------------------------------
	if (Elite::Distance(GetTarget(), agent.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
		SetBehaviorType(BehaviorType::WANDER);
	}

	return Behavior_Seek(m_pInterface, m_Target, m_CanRun);
}
SteeringPlugin_Output Brain::UpdateBehaviour_Wander(const float elapsedSec, const bool canRun)
{
	if (m_WanderTimer.Done())
		return CalculateNewWanderTarget(m_pInterface, m_Target);
	else
		return Behavior_Seek(m_pInterface, m_Target, canRun);
}
SteeringPlugin_Output Brain::UpdateBehaviour_Items(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV)
{
	if (!m_CheckForItemsTimer.Done())
		return Behaviour_CheckForItems(m_pInterface);
	//else
	//	SetBehaviorType(BehaviorType::WANDER);

	return SteeringPlugin_Output();
}

SteeringPlugin_Output Brain::UpdateState_Flee(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV)
{
	if (!m_FleeTimer.Done())
		return Behaviour_Flee(m_pInterface, m_Target);
	else
		SetBehaviorType(BehaviorType::WANDER);
	
	return SteeringPlugin_Output();
}
SteeringPlugin_Output Brain::UpdateState_House(const float elapsedSec)
{
	AgentInfo agent = m_pInterface->Agent_GetInfo();

	/* Check if the agent reached to flee target */
	/* Check if the timer reached its max time   */
	const float distanceToTarget{ Elite::Distance(GetTarget(), agent.Position) };
	if (distanceToTarget > 0.5f)
		return Behavior_Seek(m_pInterface, m_Target, m_CanRun);
	else
		m_EnterHouseTimer.Stop();

	return SteeringPlugin_Output();
}
SteeringPlugin_Output Brain::UpdateState_Item(const float elapsedSec)
{
	AgentInfo agent{ m_pInterface->Agent_GetInfo() };
	const float distance{ agent.DistanceToTarget(m_Target) };
	if (distance >= (agent.GrabRange - 1.f))
		return Behavior_Seek(m_pInterface, m_Target, m_CanRun);
	else
		SetBehaviorType(BehaviorType::HOUSE_CHECK);

	return SteeringPlugin_Output();
}
SteeringPlugin_Output Brain::UpdateState_ExitHouse(const float elapsedSec)
{
	AgentInfo agent = m_pInterface->Agent_GetInfo();
	const float distanceToTarget{ agent.DistanceToTarget(m_Target) };
	if (distanceToTarget > 2.5f)
		return Behavior_Seek(m_pInterface, m_Target, m_CanRun);
	else
		SetBehaviorType(BehaviorType::WANDER);

	return SteeringPlugin_Output();
}
SteeringPlugin_Output Brain::UpdateState_StandNFight(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV, Inventory* pInventory)
{
	if (!m_pInterface->Agent_GetInfo().HasEnemyInFOV(entitiesInFOV))
	{
		if (!m_AttackTimer.Done())
			return Behaviour_RotateToSeeEnemies(m_pInterface);
	}
	else
	{
		if (m_AttackTimer.IsTiming())
			return Behaviour_AimToSeeEnemies(m_pInterface, entitiesInFOV, this, pInventory);
		else
			return SteeringPlugin_Output();
	}
	return SteeringPlugin_Output();
}
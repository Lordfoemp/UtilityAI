#include "stdafx.h"
#include "Utility.h"

#include <cmath>
#include <math.h>

#include "Exam_HelperStructs.h"

#include "Brain.h"
#include "Inventory.h"

Utility::Utility()
	: m_pBlackboard{ nullptr }
	, m_PI{}
	, m_E{}
{
}

void Utility::Initialize(Elite::Blackboard* pBlackboard)
{
	m_pBlackboard = pBlackboard;

	m_PI = 3.14159265359f;
	m_E = std::exp(1.0);
}

float Utility::ChooseAction(std::vector<float>& weights)
{
	return *std::max_element(weights.begin(), weights.end());
}
BehaviorType Utility::Reasoner()
{
	Brain* pBrain{ nullptr };
	m_pBlackboard->GetData("Brain", pBrain);

	const float weight_Flee_Outside{	Curve_Scoring(CurveType::LINEAR,		Considerations(ActionType::FLEE_OUTSIDE)) };
	const float weight_Flee_Inside{		Curve_Scoring(CurveType::LOGISTIC,		Considerations(ActionType::FLEE_INSIDE)) };
	const float weight_Flee_PurgeZone{	Curve_Scoring(CurveType::NONE,			Considerations(ActionType::FLEE_PURGE)) };
	const float weight_Flee_Bitten{		Curve_Scoring(CurveType::NONE,			Considerations(ActionType::FLEE_BITTEN)) };
	const float weight_Attack{			Curve_Scoring(CurveType::NONE,			Considerations(ActionType::ATTACK)) };
	const float weight_House_Enter{		Curve_Scoring(CurveType::LINEAR,		Considerations(ActionType::HOUSE_ENTER)) };
	const float weight_House_Check{		Curve_Scoring(CurveType::INVERSE_LINEAR,Considerations(ActionType::HOUSE_CHECK)) };
	const float weight_House_Item{		Curve_Scoring(CurveType::LOGISTIC,		Considerations(ActionType::HOUSE_ITEMS)) };
	const float weight_House_Exit{		Curve_Scoring(CurveType::LINEAR,		Considerations(ActionType::HOUSE_EXIT)) };
	const float weight_Wander{			Curve_Scoring(CurveType::NONE,			Considerations(ActionType::WANDER)) };

	std::vector<float> weights{ weight_Flee_Outside,
								weight_Flee_Inside,
								weight_Flee_PurgeZone,
								weight_Flee_Bitten,
								weight_House_Enter, 
								weight_House_Check,
								weight_House_Item,
								weight_House_Exit,
								weight_Attack,
								weight_Wander };
	
	const float max{ ChooseAction(weights) };
	if (Elite::AreEqual(max, weight_Wander))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::WANDER)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Wander \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		return BehaviorType::WANDER;
	}
	else if (Elite::AreEqual(max, weight_Flee_Outside))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_NORMAL)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Flee outside \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::ENEMY_FLEE_NORMAL);
		return BehaviorType::ENEMY_FLEE_NORMAL;
	}
	else if (Elite::AreEqual(max, weight_Flee_Inside))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_HOUSE)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Flee inside \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::ENEMY_FLEE_HOUSE);
		return BehaviorType::ENEMY_FLEE_HOUSE;
	}
	else if (Elite::AreEqual(max, weight_Flee_PurgeZone))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_PURGE)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Flee purge \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::ENEMY_FLEE_PURGE);
		return BehaviorType::ENEMY_FLEE_PURGE;
	}
	else if (Elite::AreEqual(max, weight_Flee_Bitten))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_BITTEN)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Flee bitten \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::ENEMY_FLEE_BITTEN);
		return BehaviorType::ENEMY_FLEE_BITTEN;
	}
	else if (Elite::AreEqual(max, weight_Attack))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::STAND_FIGHT)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: Attack \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::STAND_FIGHT);
		return BehaviorType::STAND_FIGHT;
	}
	else if (Elite::AreEqual(max, weight_House_Enter))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_ENTER)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: House enter \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::HOUSE_ENTER);
		return BehaviorType::HOUSE_ENTER;
	}
	else if (Elite::AreEqual(max, weight_House_Check))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_CHECK)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: House check \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		return BehaviorType::HOUSE_CHECK;
	}
	else if (Elite::AreEqual(max, weight_House_Item))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_ITEM)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: House item \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::HOUSE_ITEM);
		return BehaviorType::HOUSE_ITEM;
	}
	else if (Elite::AreEqual(max, weight_House_Exit))
	{
		if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_EXIT)
		{
			std::cout << "\n| ---------------------------------------------------------- |\n";
			std::cout << "| Action: House exit \n";
			std::cout << "| ---------------------------------------------------------- |\n";
		}
		SetUpData(BehaviorType::HOUSE_EXIT);
		return BehaviorType::HOUSE_EXIT;
	}

	return BehaviorType::WANDER;
}
float Utility::Considerations(const ActionType& actionType)
{
	Brain* pBrain = nullptr;
	IExamInterface* pInterface = nullptr;	
	Timer* pCheckItemTimer = nullptr;
	Timer* pExitHouseTimer = nullptr;
	Timer* pAttackTimer = nullptr;
	Timer* pFleeTimer = nullptr;
	Timer* pEnterHouseTimer = nullptr;
	Inventory* pIventory = nullptr;
	std::vector<EntityInfo>* pEntitiesInFOV = nullptr;
	std::vector<HouseInfo>* pHousesInFOV = nullptr;
	auto dataAvailable = m_pBlackboard->GetData("Interface", pInterface) &&
						 m_pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) &&
						 m_pBlackboard->GetData("housesInFOV", pHousesInFOV) &&
						 m_pBlackboard->GetData("Brain", pBrain) &&
						 m_pBlackboard->GetData("CheckForItemsTimer", pCheckItemTimer) &&
						 m_pBlackboard->GetData("ExitHouseTimer", pExitHouseTimer) &&
						 m_pBlackboard->GetData("AttackTimer", pAttackTimer) &&
						 m_pBlackboard->GetData("FleeTimer", pFleeTimer) &&
						 m_pBlackboard->GetData("EnterHouseTimer", pEnterHouseTimer) &&
						 m_pBlackboard->GetData("inventory", pIventory);
	AgentInfo agent{ pInterface->Agent_GetInfo() };

	switch (actionType)
	{
		case ActionType::FLEE_OUTSIDE:	
		{
			bool hasEnemyInFOV{ agent.HasEnemyInFOV(*pEntitiesInFOV) };

			// Check if the agent is already in fleeing (outside) mode
			if (pBrain->GetBehaviorType() == BehaviorType::ENEMY_FLEE_NORMAL && !hasEnemyInFOV)
			{
				float value{ pFleeTimer->GetTimeToGo() * 20.f };
				return Remap(value, 0.f, value, 0.f, 16.f);
			}

			// Check if the agent is already in fleeing (outside) mode but tries to enter house for cover but has enemy in front
			if (pBrain->GetBehaviorType() == BehaviorType::HOUSE_ENTER && hasEnemyInFOV)
			{
				const float priorityMod{ 10.f };
				float value{ pFleeTimer->GetTimeToGo() * 20.f };
				return Remap(value, 0.f, value, 0.f, 16.f);
			}

			// Check if the agent sees an enemy & if the agent has no gun in its inventory
			UINT gunSlotID{};
			if (hasEnemyInFOV && !pIventory->PistolInInventory(gunSlotID))
			{
				if (!agent.IsInHouse)
				{
					float distance{ FLT_MAX };
					// if the agent is not in house, then calculate the distance to the closest enemy in FOV 
					std::vector<EntityInfo> enemiesInFOV{ agent.EnemiesInFOV(*pEntitiesInFOV) };
					EntityInfo closestEnemy{ agent.ClosestEnemyInFOV(enemiesInFOV) };
					distance = agent.DistanceToTarget(closestEnemy.Location);
					return Remap(distance, 0.f, distance, 0.f, 16.f);
				}
			}
		}
			break;
		case ActionType::FLEE_INSIDE:		
		{
			// Check if the agent is in a house and if he sees an enemy
			if (agent.IsInHouse && agent.HasEnemyInFOV(*pEntitiesInFOV))
			{
				float value{};

				if ((agent.Bitten || agent.WasBitten))
					value += 16.f;

				// Calculate the distance to the closest enemy in FOV 
				std::vector<EntityInfo> enemiesInFOV{ agent.EnemiesInFOV(*pEntitiesInFOV) };
				EntityInfo closestEnemy{ agent.ClosestEnemyInFOV(enemiesInFOV) };
				return value + (agent.DistanceToTarget(closestEnemy.Location) * 30.f);
			}

			if (agent.IsInHouse && !agent.HasEnemyInFOV(*pEntitiesInFOV))
			{
				if ((agent.Bitten || agent.WasBitten))
					return 16.f;
			}
		}
			break;
		case ActionType::FLEE_PURGE:
		{
			if (agent.IsInsidePurgeZone(*pEntitiesInFOV))
			{
				for (const EntityInfo& entity : *pEntitiesInFOV)
				{
					if (entity.Type == eEntityType::PURGEZONE)
					{
						PurgeZoneInfo purgeZone{};
						pInterface->PurgeZone_GetInfo(entity, purgeZone);

						Elite::Vector2 purgeCenter{ purgeZone.Center };
						const float purgeRadius{ purgeZone.Radius };

						const float distanceToCenter{ agent.DistanceToTarget(purgeCenter) };
						const float value{ distanceToCenter * purgeRadius };

						return value;
					}
				}
			}
		}
			break;
		case ActionType::FLEE_BITTEN:
		{
			if (!agent.HasEnemyInFOV(*pEntitiesInFOV) && agent.Bitten || agent.WasBitten)
			{
				return 16.f;
			}
		}
			break;
		case ActionType::HOUSE_ENTER:
		{
			// Check if the agent is already entering a house
			if (pBrain->GetBehaviorType() == BehaviorType::HOUSE_ENTER && pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_NORMAL)
			{
				// Return the distance to the target (center of the house)
				Elite::Vector2 houseCenter{ pBrain->GetTarget() };
				const float distance{ agent.DistanceToTarget(houseCenter) };

				//return 36.f;
				return distance;
			}
			// Check if the agent has a house in its FOV & if the agent is not already in EXIT house mode
			else if (agent.HasHouseInFOV(*pHousesInFOV) && pBrain->GetBehaviorType() != BehaviorType::HOUSE_EXIT)
			{
				const float priorityMod{ 20.f };

				float value{};
				HouseInfo house{ agent.HouseInFOV(*pHousesInFOV) };
				
				// Check if the house in FOV is the last house we checked already
				Elite::Vector2 oldTargetHouse{ pBrain->GetTargetOld() };
				if (Elite::AreEqual(house.Center.x, oldTargetHouse.x) ||
					Elite::AreEqual(house.Center.y, oldTargetHouse.y))
				{
					return 0.f;
				}

				const float distance{ agent.DistanceToTarget(house.Center) };
				// Check if the agent is fleeing (outside), if it is then rush inside
				if (pBrain->GetBehaviorType() == BehaviorType::ENEMY_FLEE_NORMAL)
					value = priorityMod + distance;
				else
					value = distance;

				if (distance > 16)
					return Remap(value, 0, value, 0, 16);

				return value;				
			}
		}
			break;
		case ActionType::HOUSE_CHECK:
		{
			if (agent.IsInHouse)
			{
				float value{};
				const float distance{ agent.DistanceToTarget(pBrain->GetTarget()) };
				if (distance < agent.FOV_Range / 4)
					value = distance/3;				

				return value;
			}
		}
			break;
		case ActionType::HOUSE_ITEMS:
		{
			if (agent.HasItemInFOV(*pEntitiesInFOV))
			{
				float value{};
				ItemInfo item{};
				EntityInfo entity{ agent.ClosestItemInFOV(*pEntitiesInFOV) };
				size_t nbrOfItemsInFOV{ agent.ItemsInFOV(*pEntitiesInFOV).size() };
				pInterface->Item_GetInfo(entity, item);

				if (item.Type != eItemType::GARBAGE)
				{
					value = 0.5f;
					const float distance{ agent.DistanceToTarget(item.Location) };
					if (distance < agent.FOV_Range)
						value += distance * nbrOfItemsInFOV;
				}				

				return value;
			}
		}
			break;
		case ActionType::HOUSE_EXIT:
		{
			// Check if time to check for items is done or not
			// Check if the agent is checking
			if (pCheckItemTimer->Done() && pBrain->GetBehaviorType() == BehaviorType::HOUSE_CHECK)
				return 16.f;

			if (pBrain->GetBehaviorType() == BehaviorType::HOUSE_EXIT)
				return 16.f;
		}
			break;
		case ActionType::ATTACK:
		{
			UINT gunSlotID{};
			// Check if the agent has enemies in its FOV & if the agent has a pistol in its inventory
			if (agent.HasEnemyInFOV(*pEntitiesInFOV) && pIventory->PistolInInventory(gunSlotID))
			{
				EntityInfo entity{ agent.ClosestEnemyInFOV(agent.EnemiesInFOV(*pEntitiesInFOV)) };
				const float distance{ agent.DistanceToTarget(entity.Location) };

				return distance*20.f;
			}
			// Check if the agent was bitten
			// Check if the agent is in a house or not
			// Check if the agent holds a pistol
			if ((agent.Bitten || agent.WasBitten) && (agent.IsInHouse || !agent.IsInHouse) && pIventory->PistolInInventory(gunSlotID))
			{
				return 16.f * 20.f;
			}
			
			// Check if the agent is already in fight mode & check if the attack timer is running
			if (pBrain->GetBehaviorType() == BehaviorType::STAND_FIGHT && pAttackTimer->IsTiming())
			{
				return 16.f * 20.f;
			}
		}
			break;
		case ActionType::WANDER:
		{
			// Calculate stuff for Flee
			if (!agent.HasEnemyInFOV(*pEntitiesInFOV) &&
				!agent.HasHouseInFOV(*pHousesInFOV) && 
				pExitHouseTimer->Done() &&
				pAttackTimer->Done() &&
				pEnterHouseTimer->Done() &&
				pFleeTimer->Done())
			{
				if (pBrain->GetBehaviorType() == BehaviorType::HOUSE_ENTER)
				{
					if (agent.DistanceToTarget(pBrain->GetTarget()) >= 0.5f)
						return 0.f;
				}

				return 1.f;
			}
		}
			break;		
		default:
			break;
	}

	return 0.0f;
}
void Utility::SetUpData(const BehaviorType& behaviorType)
{
	Brain* pBrain = nullptr;
	IExamInterface* pInterface = nullptr;
	Timer* pCheckItemTimer = nullptr;
	Timer* pExitHouseTimer = nullptr;
	Timer* pAttackTimer = nullptr;
	Timer* pFleeTimer = nullptr;
	Timer* pEnterHouseTimer = nullptr;
	Inventory* pIventory = nullptr;
	std::vector<EntityInfo>* pEntitiesInFOV = nullptr;
	std::vector<HouseInfo>* pHousesInFOV = nullptr;
	auto dataAvailable = m_pBlackboard->GetData("Interface", pInterface) &&
						 m_pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) &&
						 m_pBlackboard->GetData("housesInFOV", pHousesInFOV) &&
						 m_pBlackboard->GetData("Brain", pBrain) &&
						 m_pBlackboard->GetData("CheckForItemsTimer", pCheckItemTimer) &&
						 m_pBlackboard->GetData("ExitHouseTimer", pExitHouseTimer) &&
						 m_pBlackboard->GetData("AttackTimer", pAttackTimer) &&
						 m_pBlackboard->GetData("FleeTimer", pFleeTimer) &&
						 m_pBlackboard->GetData("EnterHouseTimer", pEnterHouseTimer) &&
						 m_pBlackboard->GetData("inventory", pIventory);
	AgentInfo agent{ pInterface->Agent_GetInfo() };

	switch (behaviorType)
	{
		case BehaviorType::ENEMY_FLEE_NORMAL:
		{
			UINT gunSlotID{};
			if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_NORMAL)
			{
				if (agent.HasEnemyInFOV(*pEntitiesInFOV) && !pIventory->PistolInInventory(gunSlotID))
				{
					float distance{ FLT_MAX };
					if (!agent.IsInHouse)
					{
						const float dotResult{ Elite::Dot(agent.LinearVelocity, agent.DirectionToTarget(agent.ClosestEnemyInFOV(agent.EnemiesInFOV(*pEntitiesInFOV)).Location)) };
						if (dotResult < 0.3f)
							pBrain->SetTarget(-agent.LinearVelocity);
						else
							pBrain->SetTarget(CalculateDirectionAwayFromTarget(agent, pEntitiesInFOV, distance));
					}
				}
			}
		}
			break;
		case BehaviorType::ENEMY_FLEE_HOUSE:
		{
			UINT gunSlotID{};
			if (pBrain->GetBehaviorType() != BehaviorType::ENEMY_FLEE_HOUSE)
			{
				if (agent.HasEnemyInFOV(*pEntitiesInFOV) && !pIventory->PistolInInventory(gunSlotID))
				{
					HouseInfo house{ agent.HouseInFOV(*pHousesInFOV) };
					float distance{ FLT_MAX };
					if (!agent.IsInHouse)
						pBrain->SetTarget(pBrain->CalculateRandomTarget(house.Center, 20.f, 30.f));
				}
			}
		}
			break;
		case BehaviorType::ENEMY_FLEE_BITTEN:
		{
			std::vector<Elite::Vector2> knownHouseCenterLocations{};
			size_t houseLocSize{ knownHouseCenterLocations.size() };
			if (knownHouseCenterLocations.empty())
			{
				pBrain->SetCanRun(true);
			}
			else
			{
				// Find the closest known house location
				size_t closestIdx{};
				float closestDistance{FLT_MAX};
				for (size_t i{}; i < houseLocSize; ++i)
				{
					const float distanceToCenter{ agent.DistanceToTarget(knownHouseCenterLocations[i]) };
					if (distanceToCenter < closestDistance)
					{
						closestDistance = distanceToCenter;
						closestIdx = i;
					}
				}

				// Check if the house is in front or at the back of the agent
				const float dotResult{ Elite::Dot(agent.LinearVelocity, agent.DirectionToTarget(knownHouseCenterLocations[closestIdx])) };
				if (dotResult < 1)
				{
					// Set the closest known house location as a target and RUN
					pBrain->SetTarget(knownHouseCenterLocations[closestIdx]);
					pBrain->SetCanRun(true);
				}
			}
		}
			break;
		case BehaviorType::HOUSE_ENTER:
		{
			if (agent.HasHouseInFOV(*pHousesInFOV) && pBrain->GetBehaviorType() != BehaviorType::HOUSE_EXIT)
			{
				if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_ENTER)
				{
					Elite::Vector2 house_center{ agent.HouseInFOV(*pHousesInFOV).Center };
					pBrain->SetTarget(house_center);
					pBrain->SetTargetOld(house_center);
					pBrain->SetCanRun(true);
					pBrain->AddHouseLocation(house_center);
				}
			}
		}
			break;
		case BehaviorType::HOUSE_CHECK:
		{
			if (agent.HasHouseInFOV(*pHousesInFOV) && pBrain->GetBehaviorType() != BehaviorType::HOUSE_EXIT)
			{
				if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_ENTER)
				{
					Elite::Vector2 house_center{ agent.HouseInFOV(*pHousesInFOV).Center };
					pBrain->SetTarget(house_center);
				}
			}
		}
			break;
		case BehaviorType::HOUSE_ITEM:
		{
			if (agent.HasItemInFOV(*pEntitiesInFOV))
			{
				std::vector<EntityInfo> itemsInFOV{ agent.ItemsInFOV(*pEntitiesInFOV) };
				EntityInfo closestItem{ agent.ClosestItemInFOV(*pEntitiesInFOV) };
				
				ItemInfo item{};
				pInterface->Item_GetInfo(closestItem, item);
				if (item.Type != eItemType::GARBAGE)
					pBrain->SetTarget(item.Location);
			}
		}
			break;
		case BehaviorType::HOUSE_EXIT:
		{
			if (pCheckItemTimer->Done() && (pBrain->GetBehaviorType() == BehaviorType::HOUSE_CHECK ||
											pBrain->GetBehaviorType() == BehaviorType::HOUSE_EXIT))
			{
				if (pBrain->GetBehaviorType() != BehaviorType::HOUSE_EXIT)
				{
					HouseInfo house{ agent.HouseInFOV(*pHousesInFOV) };
					pBrain->SetTarget(pBrain->CalculateRandomTarget(house.Center, house.Size.x + 20, house.Size.y + 20));
				}
			}
		}
			break;
		case BehaviorType::STAND_FIGHT:
		{
			UINT gunSlotID{};
			if (agent.HasEnemyInFOV(*pEntitiesInFOV) && pIventory->PistolInInventory(gunSlotID))
			{
				EntityInfo entity{ agent.ClosestEnemyInFOV(agent.EnemiesInFOV(*pEntitiesInFOV)) };
				pBrain->SetTarget(entity.Location);
			}
		}
			break;
		case BehaviorType::ENEMY_FLEE_PURGE:
		{
			if (agent.IsInsidePurgeZone(*pEntitiesInFOV))
			{
				for (const EntityInfo& entity : *pEntitiesInFOV)
				{
					if (entity.Type == eEntityType::PURGEZONE)
					{
						if (!agent.IsInHouse)
						{
							// Get the purge zone
							PurgeZoneInfo purgeZone{};
							pInterface->PurgeZone_GetInfo(entity, purgeZone);

							// Extract the needed info
							Elite::Vector2 purgeCenter{ purgeZone.Center };
							const float purgeRadius{ purgeZone.Radius };

							// Calculate the direction away from the purge zone
							const Elite::Vector2 direction{ agent.DirectionToTarget(purgeCenter) };

							pBrain->SetTarget(-direction);
							pBrain->SetCanRun(true);
						}
						else
						{
							HouseInfo house{ agent.HouseInFOV(*pHousesInFOV) };
							// Calculate a random target outside the house and RUN
							Elite::Vector2 randomPos{ pBrain->CalculateRandomTarget(house.Center, house.Size.x + 10, house.Size.y + 10) };
							// Calculate the direction away from the purge zone
							const Elite::Vector2 direction{ agent.DirectionToTarget(pInterface->NavMesh_GetClosestPathPoint(randomPos)) };

							pBrain->SetTarget(direction);
						}						
					}
				}
			}
		}
			break;
		default:
			break;
	}
}
float Utility::Curve_Scoring(const CurveType& curveType, const float value)
{
	if (Elite::AreEqual(value, 0.f))
		return value;

	// to replace values
	float a{};
	float b{};

	float score{};
	switch (curveType)
	{
		case CurveType::NONE:
		{
			return value;
		}
			break;
		case CurveType::LINEAR:
		{	// Interactive Demo: https://www.desmos.com/calculator/ucwghwlawq
			score = value / 16;
		}
			break;
		case CurveType::INVERSE_LINEAR:
		{	// Interactive Demo: https://www.desmos.com/calculator/f7q9ipki8c
			score = (-value/16) + 1.f;
		}
			break;
		case CurveType::EXPONENTIAL:
		{	// Interactive Demo: https://www.desmos.com/calculator/dvsqwyfjd4
			score = 1 - ((1 - powf(value, a)) / 1) + b;
		}
			break;
		case CurveType::SINE:
		{	// Interactive Demo: https://www.desmos.com/calculator/iywenft729
			score = sinf(value * m_PI * a) + b;
		}
			break;
		case CurveType::COSINE:
		{	// Interactive Demo: https://www.desmos.com/calculator/p9kanbukwd
			score = 1 - cosf(value * m_PI * a) + b;
		}
			break;
		case CurveType::LOGISTIC:
		{	// Interactive Demo: https://www.desmos.com/calculator/bspslggkhv
			// Also known as a sigmoid function
			const float k{ 2.31f };
			const float x0{ -0.25f };
			const float exponent{ -k * (4 * float(m_E) * (value - x0) - 2 * float(m_E)) };
			score = 1 / (1 + powf(float(m_E), exponent));
		}
			break;
		case CurveType::LOGIT:
		{	// Interactive Demo: https://www.desmos.com/calculator/fmz1hgplok
			const float nominator{ Log(a, (value / (1 - value)) + 2 * float(m_E)) };
			const float denominator{ 4 * float(m_E) };

			score = nominator / denominator;
		}
			break;
		case CurveType::SMOOTHSTEP:
		{	// Interactive Demo: https://www.desmos.com/calculator/fmvfxndlff
			score = value * value * (3 - 2 * value);
		}
			break;
		case CurveType::SMOOTHERSTEP:
		{	// Interactive Demo: https://www.desmos.com/calculator/wanqbm1ygl
			score = value * value * value * (value * ((6 * value) - 15) +10);
		}
			break;
		default:
			break;
	}

	return Remap(score, 0, 20, 0, 1);
}

float Utility::Remap(const float value, const float min_Old, const float max_Old, const float min_New, const float max_New)
{
	// REFERENCE: https://stackoverflow.com/questions/929103/convert-a-number-range-to-another-range-maintaining-ratio
	float OldRange = (max_Old - min_Old);
	float NewRange = (max_New - min_New);
	float NewValue = (((value - min_Old) * NewRange) / OldRange) + min_New;
	return  NewValue;
}
Elite::Vector2 Utility::CalculateDirectionAwayFromTarget(AgentInfo& agent, std::vector<EntityInfo>* pEntitiesInFOV, float& distance)
{
	// Find all nearby enemies
	std::vector<EntityInfo> enemiesInFOV{ agent.EnemiesInFOV(*pEntitiesInFOV) };

	// Find the closest enemy to evade
	EntityInfo closestEnemy{ agent.ClosestEnemyInFOV(enemiesInFOV) };

	// Find if the enemy is on the left or right
	Elite::Vector2 directionToTarget{ agent.DirectionToTarget(closestEnemy.Location) };
	float cross = Elite::Cross(directionToTarget, agent.LinearVelocity);
	int mod{ 0 };
	if (cross < 0)
	{
		// LEFT
		directionToTarget.x -= directionToTarget.x * float(std::cos(6.14)) - directionToTarget.y * float(std::sin(6.14));
		directionToTarget.y -= directionToTarget.x * float(std::sin(6.14)) - directionToTarget.y * float(std::cos(6.14));
	}
	else
	{
		// RIGHT
		directionToTarget.x += directionToTarget.x * float(std::cos(6.14)) - directionToTarget.y * float(std::sin(6.14));
		directionToTarget.y += directionToTarget.x * float(std::sin(6.14)) - directionToTarget.y * float(std::cos(6.14));
	}

	return directionToTarget;
}
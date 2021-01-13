#pragma once
#include <vector>

#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

#include "EDecisionMaking.h"
#include "EBehaviorTree.h"
#include "EBlackboard.h"

#include "Utility.h"
#include "Timer.h"

class IBaseInterface;
class AgentNormal;
class Inventory;
class Brain
{
	public:
		Brain();
		~Brain();

		Brain(const Brain&) = delete;
		Brain(Brain&&) noexcept = delete;
		Brain& operator=(const Brain&) = delete;
		Brain& operator=(Brain&&) noexcept = delete;

		void Initialize(IExamInterface* pInterface, AgentNormal* pAgent, Inventory* pInventory);
		SteeringPlugin_Output Update(const float elapsedSec, const std::vector<HouseInfo>& housesInFOV, const std::vector<EntityInfo>& entitiesInFOV, Inventory* pInventory);
		void Render(const float elapsedSec);

		void SetCanRun(const bool canRun) { m_CanRun = canRun; }
		void SetTarget(const Elite::Vector2& target) { m_Target = target; };
		const Elite::Vector2& GetTarget() const { return m_Target; };
		void SetTargetOld(const Elite::Vector2& targetOld) { m_TargetOld = targetOld; };
		const Elite::Vector2& GetTargetOld() const { return m_TargetOld; };
		void SetBehaviorType(const BehaviorType& type) { m_BehaviorType = type; };
		const BehaviorType& GetBehaviorType() const { return m_BehaviorType; };
		void SetEnemyType(const eEnemyType eType) { m_CurrentEnemyType = eType; }
		const eEnemyType& GetEnemyType() const { return m_CurrentEnemyType; }
		Elite::Blackboard* GetBlackboard() const { return m_pBlackboard; }

		Elite::Vector2 CalculateRandomTarget(const Elite::Vector2& startPos, const float minDistance, const float maxDistance)
		{
			int randomMultiplierX{ int(Elite::randomFloat(-1.f, 1.f)) };
			int randomMultiplierY{ int(Elite::randomFloat(-1.f, 1.f)) };
			if (randomMultiplierX == m_LastRandomPosMod)
			{
				if (randomMultiplierX == -1)
					randomMultiplierX = 1;
				else
					randomMultiplierX = -1;
			}
			m_LastRandomPosMod = randomMultiplierX;

			const float randomDistanceAwayX{ Elite::randomFloat(minDistance, maxDistance) };
			const float randomDistanceAwayY{ Elite::randomFloat(minDistance, maxDistance) };

			const float randomX{ startPos.x * ((Elite::AreEqual(randomMultiplierX, 0)) ? 1 : randomMultiplierX) + randomDistanceAwayX };
			const float randomY{ startPos.y * ((Elite::AreEqual(randomMultiplierY, 0)) ? 1 : randomMultiplierY) + randomDistanceAwayY };
			const Elite::Vector2 randomPos{ randomX, randomY };

			return randomPos;
		}
		void AddHouseLocation(const Elite::Vector2& houseLocation);
	private:
		Timer m_WanderTimer,
		      m_CheckForItemsTimer,
		      m_ExitHouseTimer,
		      m_AttackTimer,
		      m_FleeTimer,
		      m_EnterHouseTimer;

		Utility* m_pUtility;
		IExamInterface* m_pInterface;		// Managed by Plugin
		Elite::Blackboard* m_pBlackboard;

		Elite::Vector2 m_Target, m_TargetOld;	
		BehaviorType m_BehaviorType;
		eEnemyType m_CurrentEnemyType;

		float m_RunTimer, m_RunTimerMax;
		int m_LastRandomPosMod;
		bool m_CanRun;

		std::vector<Elite::Vector2> m_HouseLocationsKnown;

		// ------------------------
		// Functions
		// ------------------------
		SteeringPlugin_Output UpdateSteering(AgentInfo& agent, const bool run, const bool goInhouse, const bool eat, const bool regenEnergy, const std::vector<EntityInfo>& entitiesInFOV, const std::vector<HouseInfo>& housesInFOV);

		SteeringPlugin_Output UpdateBehaviour_Mouse();
		SteeringPlugin_Output UpdateBehaviour_Wander(const float elapsedSec, const bool canRun);
		SteeringPlugin_Output UpdateBehaviour_Items(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV);

		SteeringPlugin_Output UpdateState_Flee(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV);
		SteeringPlugin_Output UpdateState_House(const float elapsedSec);
		SteeringPlugin_Output UpdateState_Item(const float elapsedSec);
		SteeringPlugin_Output UpdateState_ExitHouse(const float elapsedSec);
		SteeringPlugin_Output UpdateState_StandNFight(const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV, Inventory* pInventory);
		
		Elite::Blackboard* CreateBlackboard(AgentNormal* pAgent, Inventory* pInventory);
};
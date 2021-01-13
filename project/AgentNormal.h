#pragma once
#include "IExamInterface.h"
#include "Exam_HelperStructs.h"

#include "Brain.h"
#include "EDecisionMaking.h"
class Inventory;
class AgentNormal
{
	public:
		AgentNormal();
		~AgentNormal();

		AgentNormal(const AgentNormal&) = delete;
		AgentNormal(AgentNormal&&) noexcept = delete;
		AgentNormal& operator=(const AgentNormal&) = delete;
		AgentNormal& operator=(AgentNormal&&) noexcept = delete;

		void Initialize(IExamInterface* pInterface);
		SteeringPlugin_Output Update(const float elapsedSec);
		void Render(const float elapsedSec);

		void SetAngularSpeed(const float angSpeed, const bool addition) { m_AngSpeed = angSpeed; }
		void SetCanRun(const bool canRun) { m_AngSpeed = canRun; }
		void SetGrabItem(const bool grabItem) { m_AngSpeed = grabItem; }
		void SetUseItem(const bool useItem) { m_AngSpeed = useItem; }
		void SetRemoveItem(const bool removeItem) { m_AngSpeed = removeItem; }
		Brain* GetBrain() const { return m_pBrain; }

	private:		
		Brain* m_pBrain;
		Inventory* m_pInventory;
		IExamInterface* m_pInterface;
		
		bool m_CanRun;		
		bool m_GrabItem;	
		bool m_UseItem;		
		bool m_RemoveItem;	

		std::vector<int> m_InventorySlots;

		float m_AngSpeed;	
		float m_CheckBehindTimer;
		
		eEnemyType m_CurrentEnemyType;

		// ------------
		// Functions
		// ------------	
		void UpdateState(AgentInfo& agent);
		SteeringPlugin_Output UpdateSteering(const float elapsedSec, const std::vector<HouseInfo>& housesInFOV, const std::vector<EntityInfo>& entitiesInFOV);

		SteeringPlugin_Output Updates_Target_StandFight(AgentInfo& agent, const std::vector<EntityInfo>& entitiesInFOV);
		SteeringPlugin_Output Updates_Target_None(AgentInfo& agent, const float elapsedSec);
		SteeringPlugin_Output Updates_Target_EnemyFlee(AgentInfo& agent, bool& returnSteering, const float elapsedSec, const std::vector<EntityInfo>& entitiesInFOV, const std::vector<HouseInfo>& housesInFOV);

		std::vector<HouseInfo> GetHousesInFOV() const
		{
			vector<HouseInfo> vHousesInFOV = {};

			HouseInfo hi = {};
			for (int i = 0;; ++i)
			{
				if (m_pInterface->Fov_GetHouseByIndex(i, hi))
				{
					vHousesInFOV.push_back(hi);
					continue;
				}

				break;
			}

			return vHousesInFOV;
		}
		std::vector<EntityInfo> GetEntitiesInFOV() const
		{
			vector<EntityInfo> vEntitiesInFOV = {};

			EntityInfo ei = {};
			for (int i = 0;; ++i)
			{
				if (m_pInterface->Fov_GetEntityByIndex(i, ei))
				{
					vEntitiesInFOV.push_back(ei);
					continue;
				}

				break;
			}

			return vEntitiesInFOV;
		}
		void GetRequiredData(AgentInfo& agent, Elite::Vector2& nextTargetPos, std::vector<HouseInfo>& housesInFOV, std::vector<EntityInfo>& entitiesInFOV)
		{
			//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
			agent = m_pInterface->Agent_GetInfo();
			nextTargetPos = m_pBrain->GetTarget();			//To start you can use the mouse position as guidance
			housesInFOV = GetHousesInFOV();					//uses m_pInterface->Fov_GetHouseByIndex(...)
			entitiesInFOV = GetEntitiesInFOV();				//uses m_pInterface->Fov_GetEntityByIndex(...)
		}

		void RenderOuterEdges();
};
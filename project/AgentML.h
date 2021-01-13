#pragma once
#include "IExamInterface.h"

class Brain;
class AgentML
{
	public:
		AgentML();
		~AgentML();

		AgentML(const AgentML&) = delete;
		AgentML(AgentML&&) noexcept = delete;
		AgentML& operator=(const AgentML&) = delete;
		AgentML& operator=(AgentML&&) noexcept = delete;

		void Initialize(IExamInterface* pInterface);
		SteeringPlugin_Output Update(const float elapsedSec);

		void SetAngularSpeed(const float angSpeed, const bool addition) { m_AngSpeed = angSpeed; }
		float GetAngularSpeed() const { return m_AngSpeed; }
		void SetCanRun(const bool canRun) { m_AngSpeed = canRun; }
		bool GetCanRun() const { return m_CanRun; }
		void SetGrabItem(const bool grabItem) { m_AngSpeed = grabItem; }
		bool GetGrabItem() const { return m_GrabItem; }
		void SetUseItem(const bool useItem) { m_AngSpeed = useItem; }
		bool GetUseItem() const { return m_UseItem; }
		void SetRemoveItem(const bool removeItem) { m_AngSpeed = removeItem; }
		bool GetRemoveItem() const { return m_RemoveItem; }
		Brain* GetBrain() const { return m_pBrain; }

	private:
		Brain* m_pBrain;
		IExamInterface* m_pInterface;

		bool m_CanRun;		
		bool m_GrabItem;	
		bool m_UseItem;		
		bool m_RemoveItem;	

		float m_AngSpeed;	

		// ------------
		// Functions
		// ------------		
		std::vector<HouseInfo> GetHousesInFOV() const;
		std::vector<EntityInfo> GetEntitiesInFOV() const;
		void GetRequiredData(AgentInfo& agentInfo, Elite::Vector2& nextTargetPos, std::vector<HouseInfo>& vHouseInfo, std::vector<EntityInfo>& vEntityInfo);
};
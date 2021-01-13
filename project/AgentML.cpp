#include "stdafx.h"
#include "AgentML.h"

#include "Brain.h"
#include "AgentNormal.h"

#include "Inventory.h"

AgentML::AgentML()
	: m_pInterface{ nullptr }
	, m_pBrain{ nullptr }
	, m_CanRun{ false }
	, m_GrabItem{ false }
	, m_UseItem{ false }
	, m_RemoveItem{ false }
	, m_AngSpeed{}
{}
AgentML::~AgentML()
{}

void AgentML::Initialize(IExamInterface* pInterface)
{
	m_pInterface = pInterface;

	m_pBrain = new Brain();
	m_pBrain->Initialize(m_pInterface, new AgentNormal(), new Inventory());
	//m_pBrain->LoadBrain();
}
SteeringPlugin_Output AgentML::Update(const float elapsedSec)
{
	bool returnEarly{ false };
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	//// ----
	//// Get All the required data needed for this function
	//// ----
	//AgentInfo agentInfo{};
	//Elite::Vector2 nextTargetPos{};
	//std::vector<HouseInfo> housesInFOV{};
	//std::vector<EntityInfo> entitiesInFOV{};
	//GetRequiredData(agentInfo, nextTargetPos, housesInFOV, entitiesInFOV);
	//
	//// Save current brain		
	//if (agentInfo.Health < 2)
	//	//m_pBrain->SaveBrain();
	//
	//steering = m_pBrain->Update(agentInfo, housesInFOV, entitiesInFOV);

	return steering;
}

// ----------------
// Helper functions
// ----------------
std::vector<HouseInfo> AgentML::GetHousesInFOV() const
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
std::vector<EntityInfo> AgentML::GetEntitiesInFOV() const
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

void AgentML::GetRequiredData(AgentInfo& agentInfo, Elite::Vector2& nextTargetPos, std::vector<HouseInfo>& vHousesInFOV, std::vector<EntityInfo>& vEntitiesInFOV)
{
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	agentInfo = m_pInterface->Agent_GetInfo();
	nextTargetPos = m_pBrain->GetTarget();			//To start you can use the mouse position as guidance
	vHousesInFOV = GetHousesInFOV();				//uses m_pInterface->Fov_GetHouseByIndex(...)
	vEntitiesInFOV = GetEntitiesInFOV();			//uses m_pInterface->Fov_GetEntityByIndex(...)
}
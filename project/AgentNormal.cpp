#include "stdafx.h"
#include "AgentNormal.h"

#include "Brain.h"
#include "Inventory.h"

#include"EBlackboard.h"

AgentNormal::AgentNormal()
		   : m_pInterface{ nullptr }
		   , m_pInventory{ nullptr }
		   , m_pBrain{ nullptr }
		   , m_CanRun{ false }
		   , m_GrabItem{ false }
		   , m_UseItem{ false }
		   , m_RemoveItem{ false }
		   , m_AngSpeed{ 0.f }
		   , m_CheckBehindTimer{ 0.f }
		   , m_InventorySlots{ -1, -1, -1, -1, -1 }
{}
AgentNormal::~AgentNormal()
{
	SAFE_DELETE(m_pBrain);
	SAFE_DELETE(m_pInventory);
}

void AgentNormal::Initialize(IExamInterface * pInterface)
{
	m_pInterface = pInterface;

	m_pBrain = new Brain();

	m_pInventory = new Inventory();
	m_pInventory->Initialize(m_pInterface, m_pBrain);

	m_pBrain->Initialize(m_pInterface, this, m_pInventory);
}
SteeringPlugin_Output AgentNormal::Update(const float elapsedSec)
{
	// -------------------
	// Get All the required data needed for this function
	// -------------------
	AgentInfo agent{};
	Elite::Vector2 nextTargetPos{};
	std::vector<HouseInfo> housesInFOV{};
	std::vector<EntityInfo> entitiesInFOV{};
	GetRequiredData(agent, nextTargetPos, housesInFOV, entitiesInFOV);

	// -------------------
	// Update the FOV variables
	// -------------------
	Elite::Blackboard* pBlackboard = m_pBrain->GetBlackboard();
	pBlackboard->ChangeData("entitiesInFOV", &entitiesInFOV);
	pBlackboard->ChangeData("housesInFOV", &housesInFOV);
	// -------------------
	// Update agent state
	// -------------------
	UpdateState(agent);

	// -------------------
	// Update agent steering
	// -------------------
	SteeringPlugin_Output steering = UpdateSteering(elapsedSec, housesInFOV, entitiesInFOV);

	// -------------------
	// Update inventory
	// -------------------
	m_pInventory->Update(elapsedSec, entitiesInFOV, agent);

	return steering;
}
void AgentNormal::Render(const float elapsedSec)
{
	// Draw linear velocity agent
	m_pInterface->Draw_Direction(m_pInterface->Agent_GetInfo().Position, m_pInterface->Agent_GetInfo().LinearVelocity, 20.f, Elite::Vector3(0, 1, 0));
	m_pInterface->Draw_Direction(m_pInterface->Agent_GetInfo().Position, -m_pInterface->Agent_GetInfo().LinearVelocity, 10.f, Elite::Vector3(0, 1, 0));

	// Draw target direction
	const Elite::Vector2 direction{ m_pBrain->GetTarget() - m_pInterface->Agent_GetInfo().Position };
	m_pInterface->Draw_Direction(m_pInterface->Agent_GetInfo().Position, direction, 20.f, Elite::Vector3(0, 0, 1));

	m_pBrain->Render(elapsedSec);

	RenderOuterEdges();
}

void AgentNormal::RenderOuterEdges()
{
	//     MIN_LEFT     MIN_RIGHT
	// X:  -10			90
	//     MIN_DOWN     MIN_UP
	// Y:  -10			20

	auto in = m_pInterface->World_GetInfo();
	auto edg = in.Dimensions;
	auto c = in.Center;

	const Elite::Vector2 center{ c.x, c.y };
	const Elite::Vector2 corner1{ -edg.x / 1.5f , edg.y / 1.5f };
	const Elite::Vector2 corner2{ edg.x / 1.5f, edg.y / 1.5f };
	const Elite::Vector2 corner3{ edg.x / 1.5f, -edg.y / 1.5f };
	const Elite::Vector2 corner4{ -edg.x / 1.5f, -edg.y / 1.5f };
	const float radius{ 2.f };
	const Elite::Vector2 axis{ 1, 0 };
	const Elite::Vector3 color{ 1.f, 0.5f, 0.5f };
	m_pInterface->Draw_SolidCircle(center, radius, axis, color);
	m_pInterface->Draw_SolidCircle(corner1, radius, axis, color);
	m_pInterface->Draw_SolidCircle(corner2, radius, axis, color);
	m_pInterface->Draw_SolidCircle(corner3, radius, axis, color);
	m_pInterface->Draw_SolidCircle(corner4, radius, axis, color);
}
void AgentNormal::UpdateState(AgentInfo& agent)
{
	if (agent.Stamina < 2.f)
		m_CanRun = false;
}
SteeringPlugin_Output AgentNormal::UpdateSteering(const float elapsedSec, const std::vector<HouseInfo>& housesInFOV, const std::vector<EntityInfo>& entitiesInFOV)
{
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	// Major steering decisions (Evade/Attack/House/Wander)
	steering = m_pBrain->Update(elapsedSec, housesInFOV, entitiesInFOV, m_pInventory);

	return steering;
}
#pragma once
#include "EBehaviorTree.h"
#include "AgentNormal.h"
#include "Inventory.h"

SteeringPlugin_Output Behaviour_Flee(IExamInterface* pInterface, Elite::Vector2 direction)
{
	AgentInfo agent{ pInterface->Agent_GetInfo() };
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	steering.LinearVelocity = direction;					
	steering.LinearVelocity.Normalize();					
	steering.LinearVelocity *= agent.MaxLinearSpeed;		
	steering.AutoOrient = true;
	steering.RunMode = true;

	return steering;
}
SteeringPlugin_Output Behaviour_CheckForItems(IExamInterface* pInterface)
{
	AgentInfo agent = pInterface->Agent_GetInfo();
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	Elite::Vector2 direction{ agent.LinearVelocity };
	direction.x += direction.x * float(std::cos(5)) - direction.y * float(std::sin(5));
	direction.y += direction.x * float(std::sin(5)) - direction.y * float(std::cos(5));

	steering.LinearVelocity = direction;	
	steering.LinearVelocity.Normalize();	

	steering.AutoOrient = true;
	steering.RunMode = false;

	return steering;
}
SteeringPlugin_Output Behaviour_RotateToSeeEnemies(IExamInterface* pInterface)
{
	AgentInfo agent = pInterface->Agent_GetInfo();
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	Elite::Vector2 direction{ -agent.LinearVelocity };

	steering.LinearVelocity = direction;				// Desired Velocity
	steering.LinearVelocity.Normalize();				// Normalize Desired Velocity
	steering.LinearVelocity *= agent.MaxLinearSpeed;	// Rescale to Max Speed
	steering.AutoOrient = true;
	steering.RunMode = false;

	return steering;
}
SteeringPlugin_Output Behaviour_AimToSeeEnemies(IExamInterface* pInterface, const std::vector<EntityInfo>& entitiesInFOV, Brain* pBrain, Inventory* pInventory)
{
	AgentInfo agent = pInterface->Agent_GetInfo();
	EntityInfo entity{ agent.ClosestEnemyInFOV(agent.EnemiesInFOV(entitiesInFOV)) };
	EnemyInfo enemy{};
	pInterface->Enemy_GetInfo(entity, enemy);

	pBrain->SetTarget(entity.Location);
	pBrain->SetEnemyType(enemy.Type);

	SteeringPlugin_Output steering = SteeringPlugin_Output();
	steering.LinearVelocity = agent.DirectionToTarget(entity.Location);
	steering.LinearVelocity.Normalize();							//Normalize Desired Velocity
	steering.AutoOrient = true;

	Elite::Vector2 agentDir = agent.LinearVelocity;
	Elite::Vector2 steeringDIr = steering.LinearVelocity;

	const float maxAngle22{ 2.f };
	float cosTheta22 = (Elite::Dot(agentDir, steeringDIr)) / (agentDir.Magnitude() * steeringDIr.Magnitude());
	float rAdians{ acosf(cosTheta22) };
	float angle22 = Elite::ToDegrees(rAdians);

	const float distance{ agent.DistanceToTarget(entity.Location) };
	const float angle{ Elite::ToDegrees(Elite::Cross(steeringDIr, agentDir)) };
	const float MaxShootAngle{ Elite::ToDegrees(agent.FOV_Angle) / 30 };

	//std::cout << "Angle dot/mag*mag :  " << angle22 << '\n';
	//std::cout << "Angle cross :  " << angle << '\n';
	if (distance > 3)
	{
		if (-MaxShootAngle < angle22 && angle22 < MaxShootAngle)
		{
			pInventory->ConsumeItem_IfCarrying(eItemType::PISTOL, enemy.Type);
		}
	}
	else
	{
		if (-MaxShootAngle * 30 / 4 < angle22 && angle22 < MaxShootAngle * 30 / 4)
		{
			pInventory->ConsumeItem_IfCarrying(eItemType::PISTOL, enemy.Type);
		}
	}
	

	return steering;
}
SteeringPlugin_Output Behavior_Seek(IExamInterface* pInterface, const Elite::Vector2& target, const bool canRun)
{
	AgentInfo agent{ pInterface->Agent_GetInfo() };
	SteeringPlugin_Output steering = SteeringPlugin_Output();

	const Elite::Vector2 direction{ pInterface->NavMesh_GetClosestPathPoint(target) - agent.Position };
	steering.LinearVelocity = direction;							
	steering.LinearVelocity.Normalize();							
	steering.LinearVelocity *= agent.MaxLinearSpeed;				

	steering.AutoOrient = true;
	steering.RunMode = canRun;

	return steering;
}

SteeringPlugin_Output CalculateNewWanderTarget(IExamInterface* pInterface, Elite::Vector2& target)
{
	AgentInfo agent{ pInterface->Agent_GetInfo() };

	//Add offset
	float offsetMod{ 15.f };
	Elite::Vector2 offset = agent.LinearVelocity;
	offset.Normalize();
	offset *= offsetMod;

	const float radius = 10.f;
	const float halfJitter{ 0.5f };
	const float randomOffsetX = Elite::randomFloat(-halfJitter, halfJitter);
	const float randomOffsetY = Elite::randomFloat(-halfJitter, halfJitter);
	Elite::Vector2 randomOffset = Elite::Vector2{ randomOffsetX, randomOffsetY };

	randomOffset.Normalize();
	randomOffset *= radius;

	target = agent.Position + offset + randomOffset;
	if (!agent.IsInsideWorldBoundaries(agent.Position, pInterface->World_GetInfo().Dimensions))
		target = pInterface->World_GetInfo().Center;

	return SteeringPlugin_Output();
}
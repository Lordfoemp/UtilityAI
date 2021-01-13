#pragma once

//#include "../project/Inventory.h"
#include <vector>

#pragma region MISC
struct SteeringPlugin_Output
{
	Elite::Vector2 LinearVelocity = { 0.f, 0.f };
	float AngularVelocity = 0.f;
	bool AutoOrient = true;
	bool RunMode = false;
};

struct PluginInfo
{
	string BotName = "Minion";
	string Student_FirstName = "Not Set";
	string Student_LastName = "Not Set";
	string Student_Class = "Not Set";
};

//Do not change here but overwrite in the following function in the Plugin.cpp
//void Plugin::InitGameDebugParams(GameDebugParams& params)
struct GameDebugParams //Debuggin Purposes only (Ignored during release build)
{
	GameDebugParams(bool spawnEnemies = true, int enemyCount = 20, bool godMode = false, bool autoFollowCam = false)
	{
		SpawnEnemies = spawnEnemies;
		GodMode = godMode;
		AutoFollowCam = autoFollowCam;
		EnemyCount = enemyCount;
	}

	bool SpawnEnemies = true;					// Spawn enemies?
	int EnemyCount = 20;						// Amount of enemies?
	int ItemCount = 40;							// Amount of items?
	bool GodMode = false;						// Use GodMode? (Invincible)
	bool IgnoreEnergy = false;					// Ignore energy depletion
	bool AutoFollowCam = false;					// Auto follow the player
	bool RenderUI = false;						// Render Player UI (Parameters)
	bool AutoGrabClosestItem = false;			// Auto Grab closest item (Item_Grab)
	string LevelFile = "GameLevel.gppl";		// Level to load?
	int Seed = 1234;							// Seed for random generator	(1234)
	int StartingDifficultyStage = 0;			// Overwrites the difficulty stage
	bool InfiniteStamina = false;				// Agent has infinite stamina
	bool SpawnDebugPistol = false;				// Spawns pistol with 1000 ammo at start
	bool SpawnPurgeZonesOnMiddleClick = false;	// Middle mouse clicks spawn purge zones
};
#pragma endregion

#pragma region ENTITIES
//Enumerations
//************
enum class eEntityType
{
	ITEM,
	ENEMY,
	PURGEZONE,

	//@END
	_LAST = PURGEZONE
};

enum class eItemType
{
	PISTOL,
	MEDKIT,
	FOOD,
	GARBAGE,
	RANDOM_DROP, //Internal Only
	RANDOM_DROP_WITH_CHANCE, //Internal Only

	//@END
	_LAST = GARBAGE
};

enum class eEnemyType
{
	DEFAULT,
	ZOMBIE_NORMAL,
	ZOMBIE_RUNNER,
	ZOMBIE_HEAVY,
	RANDOM_ENEMY, //Internal Only
	//...

	//@END
	_LAST = ZOMBIE_HEAVY
};

enum class BehaviorType
{
	WANDER,
	ENEMY_FLEE_NORMAL,
	ENEMY_FLEE_HOUSE,
	ENEMY_FLEE_PURGE,
	ENEMY_FLEE_BITTEN,
	HOUSE_ENTER,
	HOUSE_CHECK,
	HOUSE_ITEM,
	HOUSE_EXIT,
	MOUSE,
	STAND_FIGHT
};

enum class AgentState
{
	HEALTHY,
	HEALTH_LOW,
	ENERGY_LOW,
	SHOOTING,
	BITTEN
};

//Structures
//**********
struct StatisticsInfo
{
	int Score; //Current Score
	float Difficulty; //Current difficulty (0 > 1 > ... / Easy > Normal > Hard)
	float TimeSurvived; //Total time survived
	float KillCountdown; //Make sure to make a kill before this timer runs out

	int NumEnemiesKilled; //Amount of enemies killed
	int NumEnemiesHit; //Amount of enemy hits
	int NumItemsPickUp; //Amount of items picked up
	int NumMissedShots; //Shots missed after firing
	int NumChkpntsReached; //Amount of checkpoints reached

};

struct HouseInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Size;
};

struct EnemyInfo
{
	eEnemyType Type;
	Elite::Vector2 Location;
	Elite::Vector2 LinearVelocity;

	int EnemyHash = 0;
	float Size;
	int Health;
};

struct ItemInfo
{
	eItemType Type;
	Elite::Vector2 Location;

	int ItemHash = 0;
};

struct PurgeZoneInfo
{
	Elite::Vector2 Center;
	float Radius = 0.0f;

	int ZoneHash = 0;
};

struct EntityInfo
{
	eEntityType Type;
	Elite::Vector2 Location;

	int EntityHash = 0;
};

struct WorldInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Dimensions;
};

struct AgentInfo
{
	float Stamina;
	float Health;
	float Energy;
	bool RunMode;
	bool IsInHouse;
	bool Bitten;
	bool WasBitten;
	bool Death;

	float FOV_Angle;
	float FOV_Range;

	Elite::Vector2 LinearVelocity;
	float AngularVelocity;
	float CurrentLinearSpeed;
	Elite::Vector2 Position;
	float Orientation;
	float MaxLinearSpeed;
	float MaxAngularSpeed;
	float GrabRange;
	float AgentSize;

	bool IsHealthLow()
	{
		const float agentLowHealth{ 5 };
		if (Health <= agentLowHealth)
			return true;

		return false;
	}
	bool IsEnergyLow()
	{
		const float agentLowEnergy{ 5 };
		if (Energy <= agentLowEnergy)
			return true;

		return false;
	}


	bool HasEnemyInFOV(const std::vector<EntityInfo>& entitiesInFOV)
	{
		for (const EntityInfo& entity : entitiesInFOV)
		{
			if (entity.Type == eEntityType::ENEMY)
				return true;
		}

		return false;
	}
	std::vector<EntityInfo> EnemiesInFOV(const std::vector<EntityInfo>& entitiesInFOV)
	{
		std::vector<EntityInfo> enemiesInFOV{};
		for (const EntityInfo& entity : entitiesInFOV)
		{
			if (entity.Type == eEntityType::ENEMY)
				enemiesInFOV.push_back(entity);
		}

		return enemiesInFOV;
	}
	EntityInfo ClosestEnemyInFOV(const std::vector<EntityInfo>& enemiesInFOV)
	{
		// Find the closest enemy to evade
		int idxClosestEnemy{};
		float distance{ FLT_MAX };
		for (size_t i{}; i < enemiesInFOV.size(); ++i)
		{
			const float distanceToTarget{ DistanceToTarget(enemiesInFOV[i].Location) };
			if (distanceToTarget < distanceToTarget)
			{
				distance = distanceToTarget;
				idxClosestEnemy = i;
			}
		}

		return enemiesInFOV[idxClosestEnemy];
	}

	bool HasItemInFOV(const std::vector<EntityInfo>& entitiesInFOV)
	{
		for (const EntityInfo& entity : entitiesInFOV)
		{
			if (entity.Type == eEntityType::ITEM)
			{
				return true;
			}
		}

		return false;
	}
	std::vector<EntityInfo> ItemsInFOV(const std::vector<EntityInfo>& entitiesInFOV)
	{
		std::vector<EntityInfo> itemsInFOV{};
		for (const EntityInfo& entity : entitiesInFOV)
		{
			if (entity.Type == eEntityType::ITEM)
				itemsInFOV.push_back(entity);
		}

		return itemsInFOV;
	}
	EntityInfo ClosestItemInFOV(const std::vector<EntityInfo>& itemsInFOV)
	{
		// Find the closest enemy to evade
		int idxClosestItem{};
		float distance{ FLT_MAX };
		for (size_t i{}; i < itemsInFOV.size(); ++i)
		{
			const float distanceToTarget{ DistanceToTarget(itemsInFOV[i].Location) };
			if (distanceToTarget < distanceToTarget)
			{
				distance = distanceToTarget;
				idxClosestItem = i;
			}
		}

		return itemsInFOV[idxClosestItem];
	}

	bool HasHouseInFOV(const std::vector<HouseInfo>& housesInFOV)
	{
		for (const HouseInfo& house : housesInFOV)
			return true;

		return false;
	}
	HouseInfo HouseInFOV(const std::vector<HouseInfo>& housesInFOV)
	{
		for (const HouseInfo& house : housesInFOV)
			return house;

		return HouseInfo();
	}

	bool IsInsidePurgeZone(const std::vector<EntityInfo>& entitiesInFOV)
	{
		for (const EntityInfo& entity : entitiesInFOV)
			if (entity.Type == eEntityType::PURGEZONE)
				return true;

		return false;
	}

	float DistanceToTarget(const Elite::Vector2 targetPos)
	{
		return Elite::Distance(Position, targetPos);
	}
	Elite::Vector2 DirectionToTarget(const Elite::Vector2 targetPos)
	{
		return (targetPos - Position);
	}
	bool IsInsideWorldBoundaries(const Elite::Vector2 agentPos, const Elite::Vector2 edges)
	{
		const float leftEdge  { -edges.x / 1.5f };
		const float rightEdge {  edges.x / 1.5f };
		const float topEdge   {  edges.x / 1.5f };
		const float bottomEdge{ -edges.x / 1.5f };

		if (agentPos.x < leftEdge)
			return false;
		if (agentPos.x > rightEdge)
			return false;
		if (agentPos.y < bottomEdge)
			return false;
		if (agentPos.y > topEdge)
			return false;

		return true;
	}
};
#pragma endregion
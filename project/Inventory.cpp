#include "stdafx.h"
#include "Inventory.h"

#include "Exam_HelperStructs.h"
#include "Brain.h"

Inventory::Inventory()
		 : m_pInterface{ nullptr }
		 , m_pBrain{ nullptr }
		 , m_InventorySlots{ -1, -1, -1, -1, -1 }
		 , m_ShootAmount{}
{}
Inventory::~Inventory()
{
	m_InventorySlots.clear();
}

void Inventory::Initialize(IExamInterface* pInterface, Brain* pBrain)
{
	m_pInterface = pInterface;
	m_pBrain = pBrain;
}
void Inventory::Update(const float elapsedSec, const std::vector<EntityInfo>& entities, AgentInfo& agent)
{
	// -------------------
	// Picking up items
	// -------------------
	if (agent.HasItemInFOV(entities))
	{
		EntityInfo entity{ agent.ClosestItemInFOV(entities) };
		/* Retrieve the ITEM information */
		ItemInfo itemGround{};
		bool isItem{ Item_GetInfo(entity, itemGround) };
		if (isItem && itemGround.Type != eItemType::GARBAGE)
		{
			/* If the item is in range grab and store it */
			if (agent.DistanceToTarget(itemGround.Location) < agent.GrabRange - 1.f)
			{
				// Check for empty slots and if there are some then just store it in the first empty one
				std::vector<int>::iterator emptySlotFound{std::find(m_InventorySlots.begin(), m_InventorySlots.end(), -1)};
				if (emptySlotFound != m_InventorySlots.end())
				{
 					Grab_StoreItem(entity, itemGround);
				}
				// Otherwise replace with a simular item
				else
				{
					for (size_t i{}; i < m_InventorySlots.size(); ++i)
					{
						if (m_InventorySlots[i] != -1)
						{
							ItemInfo itemHolding{};
							bool itemHold = Inventory_GetItem(UINT(i), itemHolding);
							if (itemHold)
							{
								if (itemHolding.Type == itemGround.Type)
								{
									if (CheckNReplace(itemGround, itemHolding, itemHolding.Type, entity, UINT(i)))
										break;
								}
							}
						}
					}
				}
			}
		}
	}

	// -------------------
	// Using items when needed
	// -------------------
	/* Health check */
	if (agent.Health < 10.f)
	{
		// Calculate the amount of energy lost
		const float healthDifference{ 10.f - agent.Health };
		if (healthDifference > 2.f)
			CheckInventoryForNeededItem(healthDifference, eItemType::MEDKIT);
	}
	/* Energy check */
	if (agent.Energy < 10.f)
	{
		// Calculate the amount of energy lost
		const float energyDifference{ 10.f - agent.Energy };
		if (energyDifference > 2.f)
			CheckInventoryForNeededItem(energyDifference, eItemType::FOOD);
	}
}

AgentState Inventory::ConsumeItem_IfCarrying(const eItemType& typeNeeded, const eEnemyType& enemyType)
{
	ItemInfo item{};
	UINT slotID{};
	if (ItemPresentInInventory(typeNeeded, item, slotID))
	{
		// -------------------
		// Use the requested type of item if it still holds enough MEDS/ENERGY/BULLETS
		// -------------------
		m_ShootAmount = (m_ShootAmount == 0) ? ((int)enemyType + 1) : m_ShootAmount;

		//std::cout << "Shots remaining before: " << m_ShootAmount << '\n';
		//std::cout << "Enemies killed before: " << m_pInterface->World_GetStats().NumEnemiesKilled << '\n';
		int killsBefore{ m_pInterface->World_GetStats().NumEnemiesKilled };
		bool usedItem{ Inventory_UseItem(slotID) };
		m_ShootAmount--;
		//std::cout << "Shots remaining after: " << m_ShootAmount << '\n';
		//std::cout << "Enemies killed after: " << m_pInterface->World_GetStats().NumEnemiesKilled << '\n';
		int killsAfter{ m_pInterface->World_GetStats().NumEnemiesKilled };

		if (!usedItem || m_pInterface->Weapon_GetAmmo(item) <= 0)
		{
			/* Remove the item from the inventory */
			Inventory_RemoveItem(slotID);
			/* Set the index of the tracker to -1 (empty slot) */
			m_InventorySlots[slotID] = -1;
		}
		
		
		if (m_ShootAmount <= 0)
		{
			/* Se */
			m_ShootAmount = 0;
			m_pBrain->SetBehaviorType(BehaviorType::WANDER);
			m_pBrain->SetTarget(Elite::Vector2(200.f, 200.f));

			return AgentState::HEALTHY;
		}

		if (killsBefore < killsAfter)
		{
			/* Se */
			m_ShootAmount = 0;
			m_pBrain->SetBehaviorType(BehaviorType::WANDER);
			m_pBrain->SetTarget(Elite::Vector2(200.f, 200.f));

			return AgentState::HEALTHY;
		}
	}

	return AgentState::HEALTHY;
}
bool Inventory::HasEmptySlots()
{
	for (size_t i{}; i < m_InventorySlots.size(); ++i)
	{
		/* If the inventory slot tracker (Vector) has an empty slot */
		if (m_InventorySlots[i] == -1)
			return true;
	}

	return false;
}
bool Inventory::CheckNReplace(ItemInfo& itemGround, ItemInfo& itemHolding, const eItemType& itemType, EntityInfo& entity, UINT idx)
{
	int holdingAmount{};
	int groundAmount{};

	if (itemType == eItemType::FOOD)
	{
		holdingAmount = m_pInterface->Food_GetEnergy(itemHolding);
		groundAmount = m_pInterface->Food_GetEnergy(itemGround);
	}
	else if (itemType == eItemType::MEDKIT)
	{
		holdingAmount = m_pInterface->Medkit_GetHealth(itemHolding);
		groundAmount = m_pInterface->Medkit_GetHealth(itemGround);
	}
	else
	{
		holdingAmount = m_pInterface->Weapon_GetAmmo(itemHolding);
		groundAmount = m_pInterface->Weapon_GetAmmo(itemGround);
	}

	if (holdingAmount < groundAmount)
	{
		RemoveItem(0, idx);
		Grab_StoreItem(entity, itemGround);

		return true;
	}

	return false;
}
bool Inventory::ItemPresentInInventory(const eItemType& itemType, ItemInfo& item, UINT& slotID)
{
	if (Inventory_GetCapacity() <= 0)
		return false;

	// -------------------
	// Go over all the items to see it the requested item type is here
	// -------------------
	for (size_t i{}; i < m_InventorySlots.size(); ++i)
	{
		/* If an item slot is not -1 (-1 is the sign that it is an empty slot) */
		if (m_InventorySlots[i] != -1)
		{
			/* Get the item and store it in the reference variable 'item' */
			bool itemFound = Inventory_GetItem(i, item);
			if (itemFound)
			{
				/* If the type is the same as the requested type */
				if (item.Type == itemType)
				{
					/* Store the slotID */
					slotID = i;
					/* Return true because we found the item in the inventory */
					return true;
				}
			}
		}
	}
	return false;
}

void Inventory::RemoveItem(const int amount, UINT slotID)
{
	if (amount <= 0)
	{
		/* Remove the item from the inventory */
		Inventory_RemoveItem(slotID);
		/* Set the index of the tracker to -1 (empty slot) */
		m_InventorySlots[slotID] = -1;
	}
}

bool Inventory::CheckInventoryForNeededItem(const float difference, const eItemType& itemType)
{
	// Go over all the inventory slots 
	for (size_t i{}; i < m_InventorySlots.size(); ++i)
	{
		// If there is a slot not empty then check the item in it
		if (m_InventorySlots[i] != -1)
		{
			ItemInfo item{};
			if (Inventory_GetItem(UINT(i), item))
			{
				// If the item is food
				if (item.Type == itemType)
				{
					// Get the amount of ... the item contains
					float amount{};
					if (itemType == eItemType::FOOD)
						amount = float(m_pInterface->Food_GetEnergy(item));
					else
						amount = float(m_pInterface->Medkit_GetHealth(item));
					// Check if the amount contained in the food is lower then the need so the AI does not waste any
					if (amount <= difference)
					{
						bool usedItem{ Inventory_UseItem(i) };
						if (usedItem)
						{
							amount -= difference;
							/* If the item is used or empty */
							RemoveItem(int(amount), i);

							return true;
						}						
					}
				}
			}
		}
	}

	return false;
}

bool Inventory::PistolInInventory(UINT& gunSlotID)
{
	// If there is a pistol in the inventory STOP moving and turn towards enemy
	ItemInfo item{};
	for (size_t i{}; i < m_InventorySlots.size(); ++i)
	{
		if (m_InventorySlots[i] != -1)
		{
			bool pistolPresent{ Inventory_GetItem(i, item) };
			if (pistolPresent && item.Type == eItemType::PISTOL)
				return true;
		}
	}

	return false;
}
bool Inventory::Grab_StoreItem(const EntityInfo& entity, ItemInfo& item)
{
	// CHeck if there is place
	auto it = std::find_if(m_InventorySlots.begin(), m_InventorySlots.end(), [](const int val) { return val == -1; });
	if (it == m_InventorySlots.end())
	{
		return false;
	}

	/* Grab the item if it is close enough */
	bool itemGrab{ Item_Grab(entity, item) };
	if (itemGrab)
	{
		/* Check for empty inventory slots */
		for (size_t i{}; i < m_InventorySlots.size(); ++i)
		{
			/* If the inventory slot tracker (Vector) has an empty slot */
			if (m_InventorySlots[i] == -1)
			{
				/* Add the item */
				Inventory_AddItem(i, item);
				/* Set the index of the tracker (Vector) to 0 so it knows, that slot is taken */
				m_InventorySlots[i] = 0;

				/* Return true because an item is picked up */
				return true;
			}
		}
	}

	return false;
}
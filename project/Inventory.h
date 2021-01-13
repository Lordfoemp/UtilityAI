#pragma once
#include "IExamInterface.h"

struct ItemInfo;
struct EntityInfo;
struct AgentInfo;
class Brain;
class Inventory
{
	public:
		Inventory();
		~Inventory();

		Inventory(const Inventory&) = delete;
		Inventory(Inventory&&) noexcept = delete;
		Inventory& operator=(const Inventory&) = delete;
		Inventory& operator=(Inventory&&) noexcept = delete;

		void Initialize(IExamInterface* pInterface, Brain* pBrain);
		void Update(const float elapsedSec, const std::vector<EntityInfo>& entities, AgentInfo& agent);
		
		bool PistolInInventory(UINT& gunSlotID);
		bool Grab_StoreItem(const EntityInfo& entity, ItemInfo& item);
		AgentState ConsumeItem_IfCarrying(const eItemType& typeNeeded, const eEnemyType& enemyType);

		bool HasEmptySlots();

	private:
		Brain* m_pBrain;					// Managed by AgentNormal
		IExamInterface* m_pInterface;		// Managed by Plugin
		std::vector<int> m_InventorySlots;
		int m_ShootAmount;

		bool CheckNReplace(ItemInfo& itemGround, ItemInfo& itemHolding, const eItemType& itemType, EntityInfo& entity, UINT idx);
		
		bool ItemPresentInInventory(const eItemType& itemType, ItemInfo& item, UINT& slotID);
		void RemoveItem(const int amount, UINT slotID);

		bool Item_GetInfo(EntityInfo entity, ItemInfo& item) { return m_pInterface->Item_GetInfo(entity, item); }
		bool Item_Grab(EntityInfo entity, ItemInfo& item) { return m_pInterface->Item_Grab(entity, item); }
		bool Item_Destroy(EntityInfo entity) { return m_pInterface->Item_Destroy(entity); }

		bool Inventory_AddItem(UINT slotID, ItemInfo& item) { return m_pInterface->Inventory_AddItem(slotID, item); };
		bool Inventory_UseItem(UINT slotID) { return m_pInterface->Inventory_UseItem(slotID); };
		bool Inventory_GetItem(UINT slotID, ItemInfo& item) { return m_pInterface->Inventory_GetItem(slotID, item); };
		bool Inventory_RemoveItem(UINT slotID) { return m_pInterface->Inventory_RemoveItem(slotID); };
		UINT Inventory_GetCapacity() { return m_pInterface->Inventory_GetCapacity(); };

		bool CheckInventoryForNeededItem(const float difference, const eItemType& itemType);
};
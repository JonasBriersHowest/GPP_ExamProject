#pragma once
#include <Exam_HelperStructs.h>
#include "vector"
#include "Actions.h"
#include "WorldState.h"
#include "HouseManager.h"
#include "PickupManager.h"
#include "InventoryManager.h"

class IBaseInterface;
class IExamInterface;

class InterfaceWrapper
{
public:
	using time_point = std::chrono::time_point<std::chrono::system_clock>;
	InterfaceWrapper( );
	void Initialize( IBaseInterface* pInterface, PluginInfo& info );
	void UpdateValues( );
	WorldState GetWorldState( ) const;

	const std::vector<HouseInfo>& GetHousesInFOV( ) const;
	HouseManager& GetHouseManager( ) const;

	const AgentInfo& Agent_GetInfo( ) const;
	Elite::Vector2 NavMesh_GetClosestPathPoint( const Elite::Vector2& targetPos ) const;
	const WorldInfo& World_GetInfo( ) const;
	bool Fov_GetHouseByIndex( UINT i, HouseInfo& hi ) const;

	void SetSteering( const SteeringPlugin_Output& steering );
	const SteeringPlugin_Output& GetSteering( ) const;

	const std::vector<Pickup>& GetItemsInFOV( ) const;
	bool InventoryGetItem( UINT idx, ItemInfo& itemInfo ) const;
	const InventoryManager& GetInventoryManager( ) const;
	bool PickupItem( const Pickup& info ) const;

	InventoryManager& GetInventoryManager( );
	PickupManager& GetPickupManager( ) const;
	void UseItem( UINT idx ) const;
	const EnemyInfo& GetClosestEnemy( ) const;
	float GetElapsedTimeSinceLastHit( ) const;
private:
	void UpdateHousesInFOV( );
	void UpdateEntitiesInFOV( );
	bool IsAimedAtEnemy( ) const;
	void ResetHitTimer( );
	void ResetSpinTimer( );
	float GetElapsedSpinTimer( ) const;
	bool ShouldSpin( ) const;
	void UpdateShouldSpin( );

	IExamInterface* m_pInterface;
	AgentInfo m_AgentInfo;
	WorldInfo m_WorldInfo;
	EnemyInfo m_ClosestEnemy;
	time_point m_LastHitTime;
	time_point m_SpinTimer;
	std::vector<HouseInfo> m_HousesInFOV;
	std::vector<Pickup> m_ItemsInFOV;
	std::vector<EnemyInfo> m_EnemiesInFOV;
	std::vector<PurgeZoneInfo> m_PurgeZonesInFOV;
	std::unique_ptr<HouseManager> m_pHouseManager;
	std::unique_ptr<PickupManager> m_pPickupManager;
	std::unique_ptr<InventoryManager> m_pInventoryManager;

	SteeringPlugin_Output m_Steering;
};

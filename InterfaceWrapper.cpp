#include "stdafx.h"
#include "InterfaceWrapper.h"
#include "IExamInterface.h"
#include "GlobalFunctions.h"

InterfaceWrapper::InterfaceWrapper( )
	: m_pInterface{ }
	, m_AgentInfo{ }
{
	m_pHouseManager = std::make_unique<HouseManager>( *this );
	m_pPickupManager = std::make_unique<PickupManager>( *this );
}

void InterfaceWrapper::Initialize( IBaseInterface* pInterface, PluginInfo& info )
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);
	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "BotNameTEST";
	info.Student_FirstName = "Jonas";
	info.Student_LastName = "Briers";
	info.Student_Class = "2DAE1";


	m_pInventoryManager = std::make_unique<InventoryManager>( *this, m_pInterface->Inventory_GetCapacity( ), 2, 2, 1 );
}

void InterfaceWrapper::UpdateValues( )
{
	m_AgentInfo = m_pInterface->Agent_GetInfo( );
	m_WorldInfo = m_pInterface->World_GetInfo( );
	// m_Steering = { };
	UpdateHousesInFOV( );
	UpdateEntitiesInFOV( );
	m_pHouseManager->Update( );
	m_pPickupManager->Update( );
	m_pInventoryManager->Update( );

	if( m_AgentInfo.WasBitten )
		ResetHitTimer( );
	UpdateShouldSpin( );
}

WorldState InterfaceWrapper::GetWorldState( ) const
{
	WorldState ws{ };
	ws.housesAvailable = m_pHouseManager->UnvisitedHousesCount( );

	// Inventory
	ws.foodsInInventory = m_pInventoryManager->GetFoodCount( );
	ws.pistolsInInventory = m_pInventoryManager->GetPistolCount( );
	ws.medkitsInInventory = m_pInventoryManager->GetMedkitCount( );

	// Pickups
	ws.foodsAvailable = m_pPickupManager->GetCount( eItemType::FOOD );
	ws.pistolsAvailable = m_pPickupManager->GetCount( eItemType::PISTOL );
	ws.medkitsAvailable = m_pPickupManager->GetCount( eItemType::MEDKIT );

	// Agent
	ws.health = UINT( m_AgentInfo.Health );
	ws.energy = UINT( m_AgentInfo.Energy );

	// Enemies
	ws.enemiesInFov = uint8_t( m_EnemiesInFOV.size( ) );
	ws.aimedAtEnemy = IsAimedAtEnemy( );

	// Danger
	ws.wasBitten = GetElapsedTimeSinceLastHit( ) < 2.f;
	ws.wantsToSpin = ShouldSpin( );
	ws.isInDanger = ws.wasBitten || ws.enemiesInFov != 0u;

	return ws;
}

const std::vector<HouseInfo>& InterfaceWrapper::GetHousesInFOV( ) const
{
	return m_HousesInFOV;
}

const AgentInfo& InterfaceWrapper::Agent_GetInfo( ) const
{
	return m_AgentInfo;
}

Elite::Vector2 InterfaceWrapper::NavMesh_GetClosestPathPoint( const Elite::Vector2& targetPos ) const
{
	return m_pInterface->NavMesh_GetClosestPathPoint( targetPos );
}

const WorldInfo& InterfaceWrapper::World_GetInfo( ) const
{
	return m_WorldInfo;
}

bool InterfaceWrapper::Fov_GetHouseByIndex( UINT i, HouseInfo& hi ) const
{
	return m_pInterface->Fov_GetHouseByIndex( i, hi );
}

void InterfaceWrapper::SetSteering( const SteeringPlugin_Output& steering )
{
	m_Steering = steering;
}

const SteeringPlugin_Output& InterfaceWrapper::GetSteering( ) const
{
	return m_Steering;
}

HouseManager& InterfaceWrapper::GetHouseManager( ) const
{
	return *m_pHouseManager;
}

const std::vector<Pickup>& InterfaceWrapper::GetItemsInFOV( ) const
{
	return m_ItemsInFOV;
}

bool InterfaceWrapper::InventoryGetItem( UINT idx, ItemInfo& itemInfo ) const
{
	return m_pInterface->Inventory_GetItem( idx, itemInfo );
}

const InventoryManager& InterfaceWrapper::GetInventoryManager( ) const
{
	return *m_pInventoryManager;
}

bool InterfaceWrapper::PickupItem( const Pickup& info ) const
{
	ItemInfo grabbedItem{ };
	auto consolePos{ GetConsoleCursorPosition( ) };
	if( m_pInterface->Item_Grab( info.entity, grabbedItem ) )
	{
		GoToConsoleCursorPosition( consolePos );
		std::cout << "Item grab successful!\n";
		auto consolePos2{ GetConsoleCursorPosition( ) };
		if( m_pInterface->Inventory_AddItem( m_pInventoryManager->GetNextFreeSlot( ), grabbedItem ) )
		{
			GoToConsoleCursorPosition( consolePos2 );
			std::cout << "Successfully added item\n";
			return true;
		}
		GoToConsoleCursorPosition( consolePos2 );
		std::cout << "Failed to add item to inventory\n";
	}
	else
	{
		GoToConsoleCursorPosition( consolePos );
		std::cout << "Item grab failed!\n";
	}
	return false;
}

InventoryManager& InterfaceWrapper::GetInventoryManager( )
{
	return *m_pInventoryManager;
}

PickupManager& InterfaceWrapper::GetPickupManager( ) const
{
	return *m_pPickupManager;
}

void InterfaceWrapper::UseItem( UINT idx ) const
{
	ItemInfo itemInfo{ };
	m_pInterface->Inventory_UseItem( idx );
	m_pInterface->Inventory_GetItem( idx, itemInfo );
	switch( itemInfo.Type )
	{
	case eItemType::PISTOL:
		if( m_pInterface->Weapon_GetAmmo( itemInfo ) == 0 )
			m_pInterface->Inventory_RemoveItem( idx );
		break;
	case eItemType::MEDKIT:
		if( m_pInterface->Medkit_GetHealth( itemInfo ) == 0 )
			m_pInterface->Inventory_RemoveItem( idx );
		break;
	case eItemType::FOOD:
		if( m_pInterface->Food_GetEnergy( itemInfo ) == 0 )
			m_pInterface->Inventory_RemoveItem( idx );
		break;
	default: ;
	}
}

const EnemyInfo& InterfaceWrapper::GetClosestEnemy( ) const
{
	return m_ClosestEnemy;
}

void InterfaceWrapper::UpdateHousesInFOV( )
{
	m_HousesInFOV.clear( );
	HouseInfo hi = { };
	for( UINT i{ };; ++i )
	{
		if( m_pInterface->Fov_GetHouseByIndex( i, hi ) )
		{
			m_HousesInFOV.push_back( hi );
			continue;
		}

		break;
	}
}

void InterfaceWrapper::UpdateEntitiesInFOV( )
{
	m_ItemsInFOV.clear( );
	m_EnemiesInFOV.clear( );
	m_PurgeZonesInFOV.clear( );

	EntityInfo ei{ };
	EnemyInfo enemy{ };
	ItemInfo item{ };
	PurgeZoneInfo purgeZone{ };
	auto cursorPos{ GetConsoleCursorPosition( ) };
	float distanceSqrClosestEnemy{ FLT_MAX };
	for( int i = 0;; ++i )
	{
		if( m_pInterface->Fov_GetEntityByIndex( i, ei ) )
		{
			switch( ei.Type )
			{
			case eEntityType::ITEM:
				if( m_pInterface->Item_GetInfo( ei, item ) )
				{
					m_ItemsInFOV.push_back( { ei, item } );
					//std::cout << "Item Detected\n";
				}
				break;
			case eEntityType::ENEMY:
				if( m_pInterface->Enemy_GetInfo( ei, enemy ) )
				{
					m_EnemiesInFOV.push_back( enemy );
					float newDistanceSqr{ DistanceSquared( enemy.Location, m_AgentInfo.Position ) };
					if( newDistanceSqr < distanceSqrClosestEnemy )
					{
						m_ClosestEnemy = enemy;
						distanceSqrClosestEnemy = newDistanceSqr;
					}
					//std::cout << "Enemy Detected\n";
				}
				break;
			case eEntityType::PURGEZONE:
				if( m_pInterface->PurgeZone_GetInfo( ei, purgeZone ) )
				{
					m_PurgeZonesInFOV.push_back( purgeZone );
					//std::cout << "PurgeZone Detected\n";
				}
				break;
			default: ;
			}
		}
		GoToConsoleCursorPosition( cursorPos );
		break;
	}
}

bool InterfaceWrapper::IsAimedAtEnemy( ) const
{
	if( m_EnemiesInFOV.empty( ) ) return false;
	auto toEnemy{ ( m_ClosestEnemy.Location - m_AgentInfo.Position ).GetNormalized( ) };
	Elite::Vector2 facing{ cosf( m_AgentInfo.Orientation + float( E_PI_2 ) ), sinf( m_AgentInfo.Orientation + float( E_PI_2 ) ) };

	auto dot{ Dot( facing, toEnemy ) };
	return abs( dot ) > 0.998f;
}

float InterfaceWrapper::GetElapsedTimeSinceLastHit( ) const
{
	auto now{ std::chrono::system_clock::now( ) };
	return float( std::chrono::duration_cast<std::chrono::seconds>( now - m_LastHitTime ).count( ) );
}

void InterfaceWrapper::ResetHitTimer( )
{
	m_LastHitTime = std::chrono::system_clock::now( );
}

void InterfaceWrapper::ResetSpinTimer( )
{
	m_SpinTimer = std::chrono::system_clock::now( );
}

float InterfaceWrapper::GetElapsedSpinTimer( ) const
{
	auto now{ std::chrono::system_clock::now( ) };
	return float( std::chrono::duration_cast<std::chrono::seconds>( now - m_SpinTimer ).count( ) );
}

bool InterfaceWrapper::ShouldSpin( ) const
{
	float elapsed{ GetElapsedSpinTimer( ) };
	return elapsed > 6.f;
}

void InterfaceWrapper::UpdateShouldSpin( )
{
	float elapsed{ GetElapsedSpinTimer( ) };
	if( elapsed > 8.f )
		ResetSpinTimer( );
}

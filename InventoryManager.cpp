#include "stdafx.h"
#include "InventoryManager.h"
#include "InterfaceWrapper.h"
#include "WorldState.h"
#include "GlobalFunctions.h"

InventoryManager::InventoryManager( InterfaceWrapper& interfaceWrapper, UINT capacity, UINT maxPistolCount, UINT maxFoodCount, UINT maxMedkitCount )
	: m_InterfaceWrapper{ interfaceWrapper }
	, m_MaxPistolCount{ maxPistolCount }
	, m_MaxFoodCount{ maxFoodCount }
	, m_MaxMedkitCount{ maxMedkitCount }
	, m_InventoryCapacity{ capacity }
{
	if( maxPistolCount + maxFoodCount + maxMedkitCount != capacity )
		throw std::exception{ "Invalid inventory capacity\n" };
}

UINT InventoryManager::GetPistolCount( ) const
{
	return UINT( m_PistolIndices.size( ) );
}

UINT InventoryManager::GetMedkitCount( ) const
{
	return UINT( m_MedkitsIndices.size( ) );
}

UINT InventoryManager::GetFoodCount( ) const
{
	return UINT( m_FoodIndices.size( ) );
}

UINT InventoryManager::MaxPistolCount( ) const
{
	return m_MaxPistolCount;
}

UINT InventoryManager::MaxFoodCount( ) const
{
	return m_MaxFoodCount;
}

UINT InventoryManager::MaxMedkitCount( ) const
{
	return m_MaxMedkitCount;
}

UINT InventoryManager::InventoryCapacity( ) const
{
	return m_InventoryCapacity;
}

UINT InventoryManager::GetNextFreeSlot( ) const
{
	return m_NextFreeSlot;
}

void InventoryManager::UseMedkit( ) const
{
	UINT idx{ m_MedkitsIndices[0] };
	m_InterfaceWrapper.get( ).UseItem( idx );
}

void InventoryManager::UsePistol( ) const
{
	UINT idx{ m_PistolIndices[0] };
	m_InterfaceWrapper.get( ).UseItem( idx );
}

void InventoryManager::UseFood( ) const
{
	UINT idx{ m_FoodIndices[0] };
	m_InterfaceWrapper.get( ).UseItem( idx );
}

void InventoryManager::Update( )
{
	m_FoodIndices.clear( );
	m_MedkitsIndices.clear( );
	m_PistolIndices.clear( );
	m_NextFreeSlot = 0u;
	auto cursorPos{ GetConsoleCursorPosition( ) };
	for( UINT i{ }; i < m_InventoryCapacity; ++i )
	{
		ItemInfo info{ };
		if( m_InterfaceWrapper.get( ).InventoryGetItem( i, info ) )
		{
			if( i == m_NextFreeSlot )
				++m_NextFreeSlot;

			switch( info.Type )
			{
			case eItemType::PISTOL:
				m_PistolIndices.push_back( i );
				break;
			case eItemType::MEDKIT:
				m_MedkitsIndices.push_back( i );
				break;
			case eItemType::FOOD:
				m_FoodIndices.push_back( i );
				break;
			default: ;
			}
		}
		else
		{
			GoToConsoleCursorPosition( cursorPos );
		}
	}
}

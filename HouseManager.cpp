#include "stdafx.h"
#include "HouseManager.h"
#include "InterfaceWrapper.h"

HouseManager::HouseManager( const InterfaceWrapper& interfaceWrapper )
	: m_ClosestUnvisitedHousePos{ FLT_MAX, FLT_MAX }
	, m_ForgetInterval{ 500.f }
	, m_InterfaceWrapper{ interfaceWrapper }
	, m_UnvisitedHousesCount{ false }
{
}

void HouseManager::Update( )
{
	UpdateHouseTimers( );
	UpdateVisibleHouses( );
}

void HouseManager::MarkVisited( const Elite::Vector2& housePos )
{
	float maxDistanceSqr{ 25.f };

	for( auto& house : m_Houses )
	{
		if( DistanceSquared( house.info.Center, housePos ) < maxDistanceSqr )
		{
			house.hasBeenVisited = true;
			house.lastVisited = std::chrono::system_clock::now( );
			std::cout << "Visited house\n";
			return;
		}
	}
}

void HouseManager::UpdateHouseTimers( )
{
	m_UnvisitedHousesCount = false;
	auto playerPos{ m_InterfaceWrapper.get( ).Agent_GetInfo( ).Position };

	auto now{ std::chrono::system_clock::now( ) };
	bool firstUnvisitedHouse{ true };
	m_UnvisitedHousesCount = 0u;
	for( auto& house : m_Houses )
	{
		if( house.hasBeenVisited )
		{
			float timeDiff{ float( std::chrono::duration_cast<std::chrono::seconds>( now - house.lastVisited ).count( ) ) };
			if( timeDiff > m_ForgetInterval )
			{
				house.hasBeenVisited = false;
			}
		}
		if( !house.hasBeenVisited )
		{
			++m_UnvisitedHousesCount;
			if( firstUnvisitedHouse || DistanceSquared( playerPos, house.info.Center ) < DistanceSquared( playerPos, m_ClosestUnvisitedHousePos ) )
			{
				firstUnvisitedHouse = false;
				m_ClosestUnvisitedHousePos = house.info.Center;
			}
		}
	}
}

void HouseManager::UpdateVisibleHouses( )
{
	auto& visibleHouses{ m_InterfaceWrapper.get( ).GetHousesInFOV( ) };
	if( visibleHouses.empty( ) )
		return;
	const auto& visibleHouse{ visibleHouses[0] };
	for( const auto& house : m_Houses )
	{
		if( house.info.Center == visibleHouse.Center )
		{
			return;
		}
	}
	m_Houses.push_back( House{ visibleHouse, std::chrono::system_clock::now( ), false } );
}

const Elite::Vector2& HouseManager::GetClosestUnvisitedHousePos( ) const
{
	return m_ClosestUnvisitedHousePos;
}

uint32_t HouseManager::UnvisitedHousesCount( ) const
{
	return m_UnvisitedHousesCount;
}

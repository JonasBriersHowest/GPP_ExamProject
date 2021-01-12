#include "stdafx.h"
#include "PickupManager.h"
#include "InterfaceWrapper.h"
#include "GlobalFunctions.h"

PickupManager::PickupManager( const InterfaceWrapper& interfaceWrapper )
	: m_InterfaceWrapper{ interfaceWrapper }
{
}

void PickupManager::Update( )
{
	const auto& itemsInFov{ m_InterfaceWrapper.get( ).GetItemsInFOV( ) };

	for( const auto& pickupInFov : itemsInFov )
	{
		if( pickupInFov.item.Type == eItemType::GARBAGE )
			continue;
		//switch( pickupInFov.item.Type )
		//{
		//case eItemType::PISTOL:
		//	std::cout << "Pistol\n";
		//	break;
		//case eItemType::MEDKIT:
		//	std::cout << "Medkit\n";
		//	break;
		//case eItemType::FOOD:
		//	std::cout << "Food\n";
		//	break;
		//default: ;
		//}

		auto& items{ GetItemsOfType( pickupInFov.item.Type ) };
		bool hasItemAlready{ false };
		for( auto& item : items )
		{
			if( item.item == pickupInFov.item )
			{
				hasItemAlready = true;
				item = pickupInFov;
				break;
			}
		}
		if( !hasItemAlready )
		{
			items.push_back( pickupInFov );
		}
	}
}

Pickup PickupManager::GetClosestPickup( eItemType type, const Elite::Vector2& pos ) const
{
	const auto& entities{ CGetItemsOfType( type ) };
	Pickup item{ };
	float distanceSqr{ FLT_MAX };

	for( const auto& entity : entities )
	{
		float newDistanceSqr{ DistanceSquared( pos, entity.item.Location ) };
		if( newDistanceSqr < distanceSqr )
		{
			distanceSqr = newDistanceSqr;
			item = entity;
		}
	}
	assert( distanceSqr != FLT_MAX );
	return item;
}

bool PickupManager::HasItemOfType( eItemType type ) const
{
	return !CGetItemsOfType( type ).empty( );
}

UINT PickupManager::GetCount( eItemType item ) const
{
	return UINT( CGetItemsOfType( item ).size( ) );
}

void PickupManager::PickupItem( const Pickup& pickup )
{
	Pickup tmp{ };
	auto& pickups{ GetItemsOfType( pickup.item.Type ) };
	if( !m_InterfaceWrapper.get( ).PickupItem( pickup ) )
		return;
	for( size_t i{ }; i < pickups.size( ); ++i )
	{
		if( pickups[i].item.Location == pickup.item.Location )
		{
			tmp = pickups[i];
			pickups[i] = pickups[pickups.size( ) - 1u];
			pickups.pop_back( );
			break;
		}
	}
}

const std::vector<Pickup>& PickupManager::CGetItemsOfType( eItemType et ) const
{
	switch( et )
	{
	case eItemType::PISTOL:
		return m_Pistols;
	case eItemType::MEDKIT:
		return m_Medkits;
	case eItemType::FOOD:
		return m_Foods;
	default:
		throw std::exception{ "Invalid item type!" };
	}
}

std::vector<Pickup>& PickupManager::GetItemsOfType( eItemType et )
{
	switch( et )
	{
	case eItemType::PISTOL:
		return m_Pistols;
	case eItemType::MEDKIT:
		return m_Medkits;
	case eItemType::FOOD:
		return m_Foods;
	default:
		throw std::exception{ "Invalid item type!" };
	}
}

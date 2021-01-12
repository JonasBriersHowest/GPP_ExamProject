#pragma once
#include <Exam_HelperStructs.h>

class InterfaceWrapper;

struct Pickup
{
	EntityInfo entity;
	ItemInfo item;
};

class PickupManager
{
public:
	PickupManager( const InterfaceWrapper& interfaceWrapper );
	void Update( );
	Pickup GetClosestPickup( eItemType type, const Elite::Vector2& pos ) const;
	bool HasItemOfType( eItemType type ) const;
	UINT GetCount( eItemType item ) const;
	void PickupItem( const Pickup& pickup ) ;
private:
	const std::vector<Pickup>& CGetItemsOfType( eItemType et ) const;
	std::vector<Pickup>& GetItemsOfType( eItemType et );

	std::reference_wrapper<const InterfaceWrapper> m_InterfaceWrapper;
	std::vector<Pickup> m_Pistols;
	std::vector<Pickup> m_Medkits;
	std::vector<Pickup> m_Foods;
};

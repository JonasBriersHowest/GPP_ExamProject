#pragma once
#include <vector>

struct WorldState;
class InterfaceWrapper;

class InventoryManager
{
public:
	InventoryManager( InterfaceWrapper& interfaceWrapper, UINT capacity, UINT maxPistolCount, UINT maxFoodCount, UINT maxMedkitCount );

	UINT GetPistolCount( ) const;
	UINT GetMedkitCount( ) const;
	UINT GetFoodCount( ) const;

	UINT MaxPistolCount( ) const;
	UINT MaxFoodCount( ) const;
	UINT MaxMedkitCount( ) const;
	UINT InventoryCapacity( ) const;
	UINT GetNextFreeSlot( ) const;

	void UseMedkit( ) const;
	void UsePistol( ) const;
	void UseFood( ) const;

	void Update( );
private:
	std::reference_wrapper<InterfaceWrapper> m_InterfaceWrapper;
	std::vector<UINT> m_PistolIndices;
	std::vector<UINT> m_FoodIndices;
	std::vector<UINT> m_MedkitsIndices;

	UINT m_MaxPistolCount;
	UINT m_MaxFoodCount;
	UINT m_MaxMedkitCount;
	UINT m_InventoryCapacity;
	UINT m_NextFreeSlot;
};

#pragma once
#include "GlobalFunctions.h"
#include <Exam_HelperStructs.h>
#include <chrono>
#include <vector>
class InterfaceWrapper;

class HouseManager
{
	struct House
	{
		HouseInfo info;
		time_point lastVisited;
		bool hasBeenVisited = false;
	};

public:
	HouseManager( const InterfaceWrapper& interfaceWrapper );
	void Update( );
	void MarkVisited( const Elite::Vector2& housePos );
	const Elite::Vector2& GetClosestUnvisitedHousePos( ) const;
	uint32_t UnvisitedHousesCount( ) const;
private:
	void UpdateHouseTimers( );
	void UpdateVisibleHouses( );

	Elite::Vector2 m_ClosestUnvisitedHousePos;
	std::vector<House> m_Houses;
	float m_ForgetInterval;
	std::reference_wrapper<const InterfaceWrapper> m_InterfaceWrapper;
	uint32_t m_UnvisitedHousesCount;
};

#include "stdafx.h"
#include "AGAP.h"

AGAP::AGAP( )
	: m_CurrentWorldState{ }
	, m_pCurrentAction{ }
	, m_Laziness{ 0.1f }
	, m_ShortTermBias{ 1.5f }
	, m_PlanDepth{ 5u }
{
}

float AGAP::EvaluateWorldState( const WorldState& worldState )
{
	float score{ };

	score += 80.f * float( min( worldState.pistolsAvailable, 2u ) );
	score += 50.f * float( min( worldState.medkitsAvailable, 2u ) );
	score += 50.f * float( min( worldState.foodsAvailable, 2u ) );


	score += float( worldState.medkitsInInventory ) * 80.f;
	score += float( worldState.pistolsInInventory ) * 90.f;
	score += float( worldState.foodsInInventory ) * 70.f;
	if( worldState.foodsInInventory != 0 )
		score += 70.f;
	if( worldState.pistolsInInventory != 0 )
		score += 70.f;
	score += float( worldState.health ) * 50.f;
	if( worldState.health > 4u )
		score += 100.f;
	score += float( worldState.energy ) * 20.f;
	score += float( worldState.killCount ) * 150.f;
	score += Elite::Clamp( 3.f - float( worldState.enemiesInFov ), 0.f, 10.f ) * 200.f;
	if( !worldState.isInDanger )
		score += 1600;
	if( !worldState.purgeZoneInFov )
		score += 2600;

	return score;
}

std::tuple<IAction*, std::string> AGAP::GetBestAction( const WorldState& worldState, depth_t depth )
{
	IAction* pBest{ };
	std::string descriptor{ };
	ExploreActionResult bestActionResult{ FLT_MAX, FLT_MIN };

	ForAllPossibleActions( worldState, [&]( IAction& action )
	{
		auto cost{ action.GetCost( ) };
		WorldState newWorldState{ action.GetResult( worldState ) };
		ExploreActionResult result{ };
		std::string desc{ action.GetDescriptor( ) };
		if( depth >= m_PlanDepth )
		{
			result = { cost, EvaluateWorldState( newWorldState ) };
		}
		else
		{
			auto [pBestNext, nextDesc]{ GetBestAction( newWorldState, depth + 1u ) };
			if( pBestNext )
			{
				result =
				{
					pBestNext->GetCost( ) + cost,
					EvaluateWorldState( pBestNext->GetResult( newWorldState ) ) / ( float( depth + 1u ) * m_ShortTermBias ) + EvaluateWorldState( newWorldState )
				};
				desc += " -> " + nextDesc;
			}
			else
			{
				std::cout << "No best next action found!\n";
				result = { cost, EvaluateWorldState( newWorldState ) };
			}
		}
		if( IsBetterActionPath( result, bestActionResult ) )
		{
			pBest = &action;
			bestActionResult = result;
			descriptor = desc;
		}
	} );

	return { pBest, descriptor };
}

bool operator==( const WorldState& lhs, const WorldState& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( WorldState ) ) == 0;
}

void AGAP::UpdateWorldState( const WorldState& worldState )
{
	if( worldState == m_CurrentWorldState )
		return;
	std::cout << "Devised new plan!\n";
	auto [pAction, descriptor]{ GetBestAction( worldState, 0u ) };
	m_pCurrentAction = pAction;
	std::cout << "New plan: " << descriptor << '\n';
	m_pCurrentAction->Start( );
	m_CurrentWorldState = worldState;
}

void AGAP::ExecutePlan( float dt ) const
{
	if( m_pCurrentAction )
		m_pCurrentAction->Update( dt );
}

void AGAP::AddAction( std::unique_ptr<IAction> pAction )
{
	m_pActions.emplace_back( std::move( pAction ) );
}

void AGAP::ForAllPossibleActions( const WorldState& worldState, const std::function<void( IAction& )>& func ) const
{
	for( auto& pAction : m_pActions )
	{
		if( !pAction->IsAvailable( worldState ) )
			continue;
		func( *pAction );
	}
}

bool AGAP::IsBetterActionPath( const ExploreActionResult& ear0, const ExploreActionResult& ear1 ) const
{
	float activeness{ 1.f - m_Laziness };
	float score0{ -ear0.cost * m_Laziness + ear0.score * activeness };
	float score1{ -ear1.cost * m_Laziness + ear1.score * activeness };
	return score0 > score1;
}

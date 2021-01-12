#pragma once
#include "Actions.h"
#include "functional"
#include "WorldState.h"


// ReSharper disable once CppInconsistentNaming
bool operator==( const WorldState& lhs, const WorldState& rhs );

class GOAP
{
	using depth_t = uint16;

	struct ExploreActionResult
	{
		float cost;
		float score;
	};

public:
	GOAP( );
	static float EvaluateWorldState( const WorldState& worldState );
	std::tuple<IAction*, std::string> GetBestAction( const WorldState& worldState, depth_t depth );
	void UpdateWorldState( const WorldState& worldState );
	void ExecutePlan( float dt ) const;
	void AddAction( std::unique_ptr<IAction> pAction );
private:
	void ForAllPossibleActions( const WorldState& worldState, const std::function<void( IAction& )>& func ) const;
	bool IsBetterActionPath( const ExploreActionResult& ear0, const ExploreActionResult& ear1 ) const;
	std::vector<std::unique_ptr<IAction>> m_pActions;
	WorldState m_CurrentWorldState;
	IAction* m_pCurrentAction;
	float m_Laziness; // Range: [0 - 1] How much do I prefer a low cost path over a high reward path?
	float m_ShortTermBias; // Range: [1 - *] how much do I prefer short term reward?
	depth_t m_PlanDepth; // How many steps into the future should I look?
};

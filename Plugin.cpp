#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "Actions.h"



//Called only once, during initialization
void Plugin::Initialize( IBaseInterface* pInterface, PluginInfo& info )
{
	// srand( 4576086u );// Hardcoded value for debugging purposes
	m_InterfaceWrapper.Initialize( pInterface, info );


	m_Planner.AddAction( std::make_unique<Wander>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<EnterHouse>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<PickupMedkit>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<PickupPistol>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<PickupFood>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<UseMedkit>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<UseFood>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<ShootEnemy>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<AimAtEnemy>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<Spin>( m_InterfaceWrapper ) );
	m_Planner.AddAction( std::make_unique<FleePurgeZone>( m_InterfaceWrapper ) );
}

//Called only once
void Plugin::DllInit( )
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown( )
{
	//Called when the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams( GameDebugParams& params )
{
	// params.SpawnPurgeZonesOnMiddleClick = true;
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.AutoGrabClosestItem = false; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update( float dt )
{
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering( float dt )
{
	m_InterfaceWrapper.UpdateValues( );
	m_Planner.UpdateWorldState( m_InterfaceWrapper.GetWorldState( ) );
	m_Planner.ExecutePlan( dt );


	return m_InterfaceWrapper.GetSteering( );
}

//This function should only be used for rendering debug elements
void Plugin::Render( float dt ) const
{
}

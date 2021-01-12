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
	////Demo Event Code
	////In the end your AI should be able to walk around without external input
	//if( m_pInterface->Input_IsMouseButtonUp( Elite::InputMouseButton::eLeft ) )
	//{
	//	//Update target based on input
	//	Elite::MouseData mouseData = m_pInterface->Input_GetMouseData( Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft );
	//	const Elite::Vector2 pos = Elite::Vector2( static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) );
	//	m_Target = m_pInterface->Debug_ConvertScreenToWorld( pos );
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_Space ) )
	//{
	//	m_CanRun = true;
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_Left ) )
	//{
	//	m_AngSpeed -= Elite::ToRadians( 10 );
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_Right ) )
	//{
	//	m_AngSpeed += Elite::ToRadians( 10 );
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_G ) )
	//{
	//	m_GrabItem = true;
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_U ) )
	//{
	//	m_UseItem = true;
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyDown( Elite::eScancode_R ) )
	//{
	//	m_RemoveItem = true;
	//}
	//else if( m_pInterface->Input_IsKeyboardKeyUp( Elite::eScancode_Space ) )
	//{
	//	m_CanRun = false;
	//}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering( float dt )
{
	m_InterfaceWrapper.UpdateValues( );
	m_Planner.UpdateWorldState( m_InterfaceWrapper.GetWorldState( ) );
	m_Planner.ExecutePlan( dt );

	auto steering = SteeringPlugin_Output( );

	return m_InterfaceWrapper.GetSteering( );
}

//This function should only be used for rendering debug elements
void Plugin::Render( float dt ) const
{
}

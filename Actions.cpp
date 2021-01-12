#include "stdafx.h"
#include "Actions.h"
#include "WorldState.h"
#include "Exam_HelperStructs.h"
#include "InterfaceWrapper.h"
#include "HouseManager.h"

IAction::IAction( InterfaceWrapper& examInterface, std::string&& descriptor )
	: m_ExamInterface{ examInterface }
	, m_Descriptor{ std::move( descriptor ) }
{
}

const std::string& IAction::GetDescriptor( ) const
{
	return m_Descriptor;
}

MoveTo::MoveTo( InterfaceWrapper& examInterface, std::string&& descriptor, float arriveRadiusSqr, float intermediateArriveRadiusSqr )
	: IAction{ examInterface, std::move( descriptor ) }
	, m_ArriveRadiusSqr{ arriveRadiusSqr }
	, m_IntermediateArriveRadiusSqr{ intermediateArriveRadiusSqr }
	, m_FirstArrive{ true }
{
}

void MoveTo::Update( float dt )
{
	SteeringPlugin_Output steering{ };
	auto pos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };

	float distanceTargetSqr{ DistanceSquared( m_TargetPos, pos ) };
	if( distanceTargetSqr < m_ArriveRadiusSqr )
	{
		m_NextPos = m_TargetPos;
		if( m_FirstArrive )
		{
			m_FirstArrive = false;
			std::cout << "Arrived\n";
			OnArrive( );
		}
		return;
	}

	float distanceNextSqr{ DistanceSquared( pos, m_NextPos ) };
	if( distanceNextSqr < m_IntermediateArriveRadiusSqr )
	{
		// Check if point is behind us
		auto toPoint = ( m_NextPos - pos );
		float orientation{ m_ExamInterface.get( ).Agent_GetInfo( ).Orientation - float( E_PI_2 ) };
		Elite::Vector2 currentDirection{ cosf( orientation ), sinf( orientation ) };
		if( Dot( toPoint, currentDirection ) < 0.f )
			m_NextPos = m_ExamInterface.get( ).NavMesh_GetClosestPathPoint( m_TargetPos );
	}

	// Steer to point
	steering.LinearVelocity = m_NextPos - pos; //Desired Velocity
	steering.LinearVelocity.Normalize( ); //Normalize Desired Velocity
	steering.LinearVelocity *= m_ExamInterface.get( ).Agent_GetInfo( ).MaxLinearSpeed; //Rescale to Max Speed

	//if( Distance( m_NextPos, pos ) < 2.f )
	//{
	//	steering.LinearVelocity = Elite::ZeroVector2;
	//}
	steering.RunMode = m_ExamInterface.get( ).GetElapsedTimeSinceLastHit( ) < 6.f;
	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	steering.AutoOrient = true; //Setting AutoOrientate to True overrides the AngularVelocity
	m_ExamInterface.get( ).SetSteering( steering );
}

void MoveTo::SetTargetPos( const Elite::Vector2& targetPos )
{
	m_FirstArrive = true;
	m_TargetPos = targetPos;
	m_NextPos = m_ExamInterface.get( ).NavMesh_GetClosestPathPoint( m_TargetPos );
}

Elite::Vector2 MoveTo::GetTarget( ) const
{
	return m_TargetPos;
}

Wander::Wander( InterfaceWrapper& examInterface )
	: MoveTo{ examInterface, "Wander", 400.f }
	, m_Cost{ 60.f }
{
}

float Wander::GetCost( ) const
{
	return m_Cost;
}

bool Wander::IsAvailable( const WorldState& worldState ) const
{
	return true;
}

WorldState Wander::GetResult( WorldState worldState )
{
	++worldState.housesAvailable;
	if( worldState.enemiesInFov == 0u )
		worldState.enemiesInFov = 1u;

	return worldState;
}

void Wander::Start( )
{
	NewRandomPos( );
}

void Wander::NewRandomPos( )
{
	auto worldInfo{ m_ExamInterface.get( ).World_GetInfo( ) };
	Elite::Vector2 botLeft{ worldInfo.Center - worldInfo.Dimensions / 2.f };
	Elite::Vector2 topRight{ worldInfo.Center + worldInfo.Dimensions / 2.f };
	SetTargetPos( { Elite::randomFloat( botLeft.x, topRight.x ), Elite::randomFloat( botLeft.y, topRight.y ) } );
}

void Wander::OnArrive( )
{
	NewRandomPos( );
}

EnterHouse::EnterHouse( InterfaceWrapper& examInterface )
	: MoveTo{ examInterface, "Enter House" }
	, m_CostFactor{ 1.f }
{
}

float EnterHouse::GetCost( ) const
{
	if( !m_ExamInterface.get( ).GetHouseManager( ).UnvisitedHousesCount( ) )
	{
		// std::cout << "No house found, guessing cost\n";
		return 150.f;
	}

	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	auto housePos{ m_ExamInterface.get( ).GetHouseManager( ).GetClosestUnvisitedHousePos( ) };

	return Distance( agentPos, housePos ) * m_CostFactor;
}

bool EnterHouse::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.housesAvailable == 0u ) return false;
	// if( worldState.isInHouse ) return false;

	return true;
}

WorldState EnterHouse::GetResult( WorldState worldState )
{
	worldState.isInHouse = true;
	--worldState.housesAvailable;
	++worldState.foodsAvailable;
	++worldState.medkitsAvailable;
	++worldState.pistolsAvailable;
	return worldState;
}

void EnterHouse::Start( )
{
	if( !m_ExamInterface.get( ).GetHouseManager( ).UnvisitedHousesCount( ) )
		std::cout << "Information provided not consistent with world state\n";

	SetTargetPos( m_ExamInterface.get( ).GetHouseManager( ).GetClosestUnvisitedHousePos( ) );
}

void EnterHouse::OnArrive( )
{
	m_ExamInterface.get( ).GetHouseManager( ).MarkVisited( GetTarget( ) );
}

PickupMedkit::PickupMedkit( InterfaceWrapper& examInterface )
	: MoveTo{ examInterface, "Pick up Medkit", 2.f }
	, m_Medkit{ }
	, m_CostFactor{ 0.3f }
{
}

float PickupMedkit::GetCost( ) const
{
	// Guess cost if no medkit found
	if( !m_ExamInterface.get( ).GetPickupManager( ).HasItemOfType( eItemType::MEDKIT ) )
		return 100;

	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	auto medkit{ m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::MEDKIT, agentPos ) };

	return Distance( agentPos, medkit.item.Location ) * m_CostFactor;
}

bool PickupMedkit::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.medkitsAvailable == 0u )
		return false;
	if( worldState.medkitsInInventory >= m_ExamInterface.get( ).GetInventoryManager( ).MaxMedkitCount( ) )
		return false;

	return true;
}

WorldState PickupMedkit::GetResult( WorldState worldState )
{
	++worldState.medkitsInInventory;
	--worldState.medkitsAvailable;

	return worldState;
}

void PickupMedkit::Start( )
{
	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	m_Medkit = m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::MEDKIT, agentPos );
	SetTargetPos( m_Medkit.item.Location );
}

void PickupMedkit::OnArrive( )
{
	m_ExamInterface.get( ).GetPickupManager( ).PickupItem( m_Medkit );
}

PickupPistol::PickupPistol( InterfaceWrapper& examInterface )
	: MoveTo{ examInterface, "Pick up Pistol", 2.f }
	, m_Pistol{ }
	, m_CostFactor{ 0.3f }
{
}

float PickupPistol::GetCost( ) const
{
	// Guess cost if no pistol found
	if( !m_ExamInterface.get( ).GetPickupManager( ).HasItemOfType( eItemType::PISTOL ) )
		return 100;


	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	auto pistol{ m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::PISTOL, agentPos ) };

	return Distance( agentPos, pistol.item.Location ) * m_CostFactor;
}

bool PickupPistol::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.pistolsAvailable == 0u )
		return false;
	if( worldState.pistolsInInventory >= m_ExamInterface.get( ).GetInventoryManager( ).MaxPistolCount( ) )
		return false;

	return true;
}

WorldState PickupPistol::GetResult( WorldState worldState )
{
	++worldState.pistolsInInventory;
	--worldState.pistolsAvailable;

	return worldState;
}

void PickupPistol::Start( )
{
	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	m_Pistol = m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::PISTOL, agentPos );
	SetTargetPos( m_Pistol.item.Location );
}

void PickupPistol::OnArrive( )
{
	m_ExamInterface.get( ).GetPickupManager( ).PickupItem( m_Pistol );
}

PickupFood::PickupFood( InterfaceWrapper& examInterface )
	: MoveTo{ examInterface, "Pick up Food", 2.f }
	, m_Food{ }
	, m_CostFactor{ 0.3f }
{
}

float PickupFood::GetCost( ) const
{
	// Guess cost if no food found
	if( !m_ExamInterface.get( ).GetPickupManager( ).HasItemOfType( eItemType::FOOD ) )
		return 100;

	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	auto food{ m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::FOOD, agentPos ) };

	return Distance( agentPos, food.item.Location ) * m_CostFactor;
}

bool PickupFood::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.foodsAvailable == 0u )
		return false;
	if( worldState.foodsInInventory >= m_ExamInterface.get( ).GetInventoryManager( ).MaxFoodCount( ) )
		return false;

	return true;
}

WorldState PickupFood::GetResult( WorldState worldState )
{
	++worldState.foodsInInventory;
	--worldState.foodsAvailable;

	return worldState;
}

void PickupFood::Start( )
{
	auto agentPos{ m_ExamInterface.get( ).Agent_GetInfo( ).Position };
	m_Food = m_ExamInterface.get( ).GetPickupManager( ).GetClosestPickup( eItemType::FOOD, agentPos );
	SetTargetPos( m_Food.item.Location );
}

void PickupFood::OnArrive( )
{
	m_ExamInterface.get( ).GetPickupManager( ).PickupItem( m_Food );
}

UseMedkit::UseMedkit( InterfaceWrapper& examInterface )
	: IAction{ examInterface, "Use Medkit" }
{
}

float UseMedkit::GetCost( ) const
{
	return 5.f;
}

bool UseMedkit::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.medkitsInInventory == 0 )
		return false;
	if( worldState.health >= 8 )
		return false;

	return true;
}

WorldState UseMedkit::GetResult( WorldState worldState )
{
	--worldState.medkitsInInventory;
	worldState.health += 2u;

	return worldState;
}

void UseMedkit::Start( )
{
	m_ExamInterface.get( ).GetInventoryManager( ).UseMedkit( );
}

void UseMedkit::Update( float dt )
{
}

UseFood::UseFood( InterfaceWrapper& examInterface )
	: IAction{ examInterface, "Use food" }
{
}

float UseFood::GetCost( ) const
{
	return 5.f;
}

bool UseFood::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.foodsInInventory == 0 )
		return false;
	if( worldState.energy > 4 )
		return false;

	return true;
}

WorldState UseFood::GetResult( WorldState worldState )
{
	--worldState.foodsInInventory;
	worldState.energy += 8u;

	return worldState;
}

void UseFood::Start( )
{
	m_ExamInterface.get( ).GetInventoryManager( ).UseFood( );
}

void UseFood::Update( float dt )
{
}

ShootEnemy::ShootEnemy( InterfaceWrapper& examInterface )
	: IAction{ examInterface, "Shoot Enemy" }
{
}

float ShootEnemy::GetCost( ) const
{
	return 1.f;
}

bool ShootEnemy::IsAvailable( const WorldState& worldState ) const
{
	if( !worldState.aimedAtEnemy ) return false;
	if( worldState.pistolsInInventory == 0u ) return false;

	return true;
}

WorldState ShootEnemy::GetResult( WorldState worldState )
{
	++worldState.killCount;
	--worldState.enemiesInFov;
	worldState.isInDanger = false;

	return worldState;
}

void ShootEnemy::Start( )
{
	m_ExamInterface.get( ).GetInventoryManager( ).UsePistol( );
}

void ShootEnemy::Update( float dt )
{
}

AimAtEnemy::AimAtEnemy( InterfaceWrapper& examInterface )
	: IAction{ examInterface, "Aim at Enemy" }
{
}

float AimAtEnemy::GetCost( ) const
{
	return 30.f;
}

bool AimAtEnemy::IsAvailable( const WorldState& worldState ) const
{
	if( worldState.enemiesInFov == 0u ) return false;
	if( worldState.pistolsInInventory == 0u ) return false;
	if( worldState.aimedAtEnemy ) return false;

	return true;
}

WorldState AimAtEnemy::GetResult( WorldState worldState )
{
	worldState.aimedAtEnemy = true;

	return worldState;
}

void AimAtEnemy::Start( )
{
}

void AimAtEnemy::Update( float dt )
{
	constexpr float evadeRange{ 70.f };
	
	/*
	 * Totally came up with this myself and didn't copy this formula from the internet
	 * But if I had copied it I would've copied it from here:
	 * https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
	 * a = targetA - sourceA
	 * a -= 360 if a > 180
	 * a += 360 if a < -180
	 */

	auto enemyPos{ m_ExamInterface.get( ).GetClosestEnemy( ).Location };
	const auto& agentInfo{ m_ExamInterface.get( ).Agent_GetInfo( ) };


	SteeringPlugin_Output steering{ };

	Elite::Vector2 toTarget{ enemyPos - agentInfo.Position };
	float targetAngle{ atan2( toTarget.y, toTarget.x ) };
	float currentAngle{ agentInfo.Orientation - float( E_PI_2 ) };
	currentAngle = atan2f( sinf( currentAngle ), cosf( currentAngle ) ); // This line makes this code compatible with the new framework

	// Find smallest difference in angle between target and current angle
	constexpr float pi2{ float( E_PI * 2 ) };
	float desiredAngularVelocity{ targetAngle - currentAngle };
	if( desiredAngularVelocity > E_PI )
		desiredAngularVelocity -= pi2;
	else if( desiredAngularVelocity < -float( E_PI ) )
		desiredAngularVelocity += pi2;


	steering.AngularVelocity = desiredAngularVelocity * 30.f;
	steering.AutoOrient = false;

	// Move away while aiming
	steering.LinearVelocity = { };
	if( toTarget.Magnitude( ) < evadeRange )
		steering.LinearVelocity = -toTarget * agentInfo.MaxLinearSpeed;

	m_ExamInterface.get( ).SetSteering( steering );
}

Spin::Spin( InterfaceWrapper& examInterface )
	: IAction{ examInterface, "Spin" }
{
}

float Spin::GetCost( ) const
{
	return 50.f;
}

bool Spin::IsAvailable( const WorldState& worldState ) const
{
	if( !worldState.wasBitten && !worldState.wantsToSpin ) return false;
	// if( worldState.pistolsInInventory == 0u ) return false;
	// if( worldState.enemiesInFov != 0u ) return false;

	return true;
}

WorldState Spin::GetResult( WorldState worldState )
{
	++worldState.enemiesInFov;

	return worldState;
}

void Spin::Start( )
{
	m_ForwardVelocity = m_ExamInterface.get( ).Agent_GetInfo( ).LinearVelocity;
}

void Spin::Update( float dt )
{
	SteeringPlugin_Output steering{ };
	steering.AutoOrient = false;
	steering.AngularVelocity = 30.f;
	steering.LinearVelocity = m_ForwardVelocity;
	m_ExamInterface.get( ).SetSteering( steering );
}

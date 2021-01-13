#pragma once
#include "EliteMath/EMath.h"
#include "PickupManager.h"
#include "GlobalFunctions.h"

struct WorldState;
class InterfaceWrapper;

class IAction
{
public:
	IAction( InterfaceWrapper& examInterface, std::string&& descriptor );

	virtual float GetCost( ) const = 0;
	virtual bool IsAvailable( const WorldState& worldState ) const = 0;
	virtual WorldState GetResult( WorldState worldState ) = 0; // Copy intentional
	virtual void Update( float dt ) = 0;
	virtual void Start( ) = 0;

	const std::string& GetDescriptor( ) const;

	virtual ~IAction( ) = default;
	IAction( const IAction& other ) = delete;
	IAction( IAction&& other ) noexcept = delete;
	IAction& operator=( const IAction& other ) = delete;
	IAction& operator=( IAction&& other ) noexcept = delete;
protected:
	std::reference_wrapper<InterfaceWrapper> m_ExamInterface;
private:
	std::string m_Descriptor;
};

class MoveTo : public IAction
{
public:
	MoveTo( InterfaceWrapper& examInterface, std::string&& descriptor, float arriveRadiusSqr = 16.f, float intermediateArriveRadiusSqr = 12.f );
	void Update( float dt ) override;
	virtual ~MoveTo( ) = default;
protected:
	void SetTargetPos( const Elite::Vector2& targetPos );
	Elite::Vector2 GetTarget( ) const;

	virtual void OnArrive( )
	{
	}

private:
	Elite::Vector2 m_TargetPos;
	Elite::Vector2 m_NextPos;
	float m_ArriveRadiusSqr;
	float m_IntermediateArriveRadiusSqr;
	bool m_FirstArrive;
};

class Wander final : public MoveTo
{
public:
	Wander( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;

	~Wander( ) override = default;
	Wander( const Wander& other ) = delete;
	Wander( Wander&& other ) noexcept = delete;
	Wander& operator=( const Wander& other ) = delete;
	Wander& operator=( Wander&& other ) noexcept = delete;
private:
	float m_Cost;

	void NewRandomPos( );
	void OnArrive( ) override;
};

class EnterHouse final : public MoveTo
{
public:
	EnterHouse( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;

	~EnterHouse( ) override = default;
	EnterHouse( const EnterHouse& other ) = delete;
	EnterHouse( EnterHouse&& other ) noexcept = delete;
	EnterHouse& operator=( const EnterHouse& other ) = delete;
	EnterHouse& operator=( EnterHouse&& other ) noexcept = delete;
private:
	void OnArrive( ) override;
	float m_CostFactor;
};

class PickupMedkit final : public MoveTo
{
public:
	PickupMedkit( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;

	~PickupMedkit( ) override = default;
	PickupMedkit( const PickupMedkit& other ) = delete;
	PickupMedkit( PickupMedkit&& other ) noexcept = delete;
	PickupMedkit& operator=( const PickupMedkit& other ) = delete;
	PickupMedkit& operator=( PickupMedkit&& other ) noexcept = delete;
private:
	void OnArrive( ) override;
	Pickup m_Medkit;
	float m_CostFactor;
};

class PickupPistol final : public MoveTo
{
public:
	PickupPistol( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;

	~PickupPistol( ) override = default;
	PickupPistol( const PickupPistol& other ) = delete;
	PickupPistol( PickupPistol&& other ) noexcept = delete;
	PickupPistol& operator=( const PickupPistol& other ) = delete;
	PickupPistol& operator=( PickupPistol&& other ) noexcept = delete;
private:
	void OnArrive( ) override;
	Pickup m_Pistol;
	float m_CostFactor;
};

class PickupFood final : public MoveTo
{
public:
	PickupFood( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;

	~PickupFood( ) override = default;
	PickupFood( const PickupFood& other ) = delete;
	PickupFood( PickupFood&& other ) noexcept = delete;
	PickupFood& operator=( const PickupFood& other ) = delete;
	PickupFood& operator=( PickupFood&& other ) noexcept = delete;
private:
	void OnArrive( ) override;
	Pickup m_Food;
	float m_CostFactor;
};

class UseMedkit final : public IAction
{
public:
	UseMedkit( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	virtual ~UseMedkit( ) = default;
	UseMedkit( const UseMedkit& other ) = delete;
	UseMedkit( UseMedkit&& other ) noexcept = delete;
	UseMedkit& operator=( const UseMedkit& other ) = delete;
	UseMedkit& operator=( UseMedkit&& other ) noexcept = delete;
};

class UseFood final : public IAction
{
public:
	UseFood( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	virtual ~UseFood( ) = default;
	UseFood( const UseFood& other ) = delete;
	UseFood( UseFood&& other ) noexcept = delete;
	UseFood& operator=( const UseFood& other ) = delete;
	UseFood& operator=( UseFood&& other ) noexcept = delete;
};

class ShootEnemy final : public IAction
{
public:
	ShootEnemy( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	virtual ~ShootEnemy( ) = default;
	ShootEnemy( const ShootEnemy& other ) = delete;
	ShootEnemy( ShootEnemy&& other ) noexcept = delete;
	ShootEnemy& operator=( const ShootEnemy& other ) = delete;
	ShootEnemy& operator=( ShootEnemy&& other ) noexcept = delete;
};

class AimAtEnemy final : public IAction
{
public:
	AimAtEnemy( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	virtual ~AimAtEnemy( ) = default;
	AimAtEnemy( const AimAtEnemy& other ) = delete;
	AimAtEnemy( AimAtEnemy&& other ) noexcept = delete;
	AimAtEnemy& operator=( const AimAtEnemy& other ) = delete;
	AimAtEnemy& operator=( AimAtEnemy&& other ) noexcept = delete;
};

class Spin final : public IAction
{
public:
	Spin( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	virtual ~Spin( ) = default;
	Spin( const Spin& other ) = delete;
	Spin( Spin&& other ) noexcept = delete;
	Spin& operator=( const Spin& other ) = delete;
	Spin& operator=( Spin&& other ) noexcept = delete;
private:
	Elite::Vector2 m_ForwardVelocity;
};

class FleePurgeZone final : public MoveTo
{
public:
	FleePurgeZone( InterfaceWrapper& examInterface );

	float GetCost( ) const override;
	bool IsAvailable( const WorldState& worldState ) const override;
	WorldState GetResult( WorldState worldState ) override;
	void Start( ) override;
	void Update( float dt ) override;

	~FleePurgeZone( ) override = default;
	FleePurgeZone( const FleePurgeZone& other ) = delete;
	FleePurgeZone( FleePurgeZone&& other ) noexcept = delete;
	FleePurgeZone& operator=( const FleePurgeZone& other ) = delete;
	FleePurgeZone& operator=( FleePurgeZone&& other ) noexcept = delete;
private:
	time_point m_LastEscapePosCalculated;

	void UpdateEscapePos( );
};

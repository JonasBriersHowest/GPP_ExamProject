#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "GOAP.h"
#include "InterfaceWrapper.h"

class IBaseInterface;
class IExamInterface;

class Plugin : public IExamPlugin
{
public:
	Plugin( ) = default;

	virtual ~Plugin( ) = default;

	void Initialize( IBaseInterface* pInterface, PluginInfo& info ) override;
	void DllInit( ) override;
	void DllShutdown( ) override;

	void InitGameDebugParams( GameDebugParams& params ) override;
	void Update( float dt ) override;

	SteeringPlugin_Output UpdateSteering( float dt ) override;
	void Render( float dt ) const override;

private:
	GOAP m_Planner;
	InterfaceWrapper m_InterfaceWrapper;
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
__declspec (dllexport) IPluginBase* Register( )
{
	return new Plugin( );
}
}

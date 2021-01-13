#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

class AgentNormal;
class AgentML;
class IBaseInterface;
class IExamInterface;
class Plugin :public IExamPlugin
{
	public:
		Plugin();
		virtual ~Plugin();

		void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
		void DllInit() override;
		void DllShutdown() override;

		void InitGameDebugParams(GameDebugParams& params) override;
		void Update(float dt) override;

		SteeringPlugin_Output UpdateSteering(float dt) override;
		void Render(float dt) const override;

	private:
		// ------------
		// Variables
		// ------------
		//Interface, used to request data from/perform actions with the AI Framework
		IExamInterface* m_pInterface = nullptr;
		AgentNormal* m_pAgentNormal = nullptr;
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}
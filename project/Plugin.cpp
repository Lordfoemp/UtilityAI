#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"

#include <random>

#include "AgentNormal.h"
#include "Brain.h"

Plugin::Plugin()
	: m_pInterface{ nullptr }
	, m_pAgentNormal{ nullptr }
{}

Plugin::~Plugin()
{
	SAFE_DELETE(m_pAgentNormal);
}

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	m_pAgentNormal = new AgentNormal();
	m_pAgentNormal->Initialize(m_pInterface);
	
	srand(unsigned int(time(nullptr)));

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "CARLOS THE CHEESER";
	info.Student_FirstName = "Steve";
	info.Student_LastName = "Verhoeven";
	info.Student_Class = "2DAE02";
}

//Called only once
void Plugin::DllInit()
{}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
	SAFE_DELETE(m_pAgentNormal);
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	//Automatically follow the AI? (Default = true)
	params.AutoFollowCam = true;
	//Render the IMGUI Panel? (Default = true)
	params.RenderUI = false;
	//Do you want to spawn enemies? (Default = true)
	params.SpawnEnemies = true;
	//How many enemies? (Default = 20)
	params.EnemyCount = 20;
	//GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.GodMode = true;
	//A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.AutoGrabClosestItem = true;	

	params.SpawnDebugPistol = false;
	params.SpawnPurgeZonesOnMiddleClick = false;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_pAgentNormal->GetBrain()->SetTarget(m_pInterface->Debug_ConvertScreenToWorld(pos));
		m_pAgentNormal->GetBrain()->SetBehaviorType(BehaviorType::MOUSE);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
		m_pAgentNormal->SetCanRun(true);
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
		m_pAgentNormal->SetAngularSpeed(Elite::ToRadians(10), false);
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
		m_pAgentNormal->SetAngularSpeed(Elite::ToRadians(10), true);
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
		m_pAgentNormal->SetGrabItem(true);
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
		m_pAgentNormal->SetUseItem(true);
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
		m_pAgentNormal->SetRemoveItem(true);
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
		m_pAgentNormal->SetCanRun(false);
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_0))
		m_pAgentNormal->GetBrain()->SetBehaviorType(BehaviorType::STAND_FIGHT);
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	return m_pAgentNormal->Update(dt);
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_pAgentNormal->GetBrain()->GetTarget(), 0.7f, { 0, 0 }, { 1, 0, 0 });
	m_pAgentNormal->Render(dt);
}
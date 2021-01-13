#include "stdafx.h"
#include "Timer.h"

Timer::Timer(const float maxTime)
	: m_CurrentTime{}
	, m_MaxTime{ maxTime }
	, m_TimeToGo{}
	, m_IsDone{ true }
	, m_IsTiming{ false }
{}

void Timer::Start()
{
	m_IsTiming = true;
	m_IsDone = false;

	m_CurrentTime = 0.f;
	m_TimeToGo = m_MaxTime - m_CurrentTime;
}

void Timer::Update(const float elapsedSec)
{
	if (m_IsTiming)
	{
		if (m_CurrentTime < m_MaxTime)
		{
			m_CurrentTime += elapsedSec;
			m_TimeToGo = m_MaxTime - m_CurrentTime;
		}
		else
		{
			Stop();
		}
	}	
}

void Timer::Stop()
{
	m_IsTiming = false;
	m_IsDone = true;

	m_CurrentTime = 0;
}
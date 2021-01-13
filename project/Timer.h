#pragma once
class Timer
{
	public:
		Timer(const float maxTime);
		~Timer() = default;

		Timer(const Timer&) = delete;
		Timer(Timer&&) noexcept = delete;
		Timer& operator=(const Timer&) = delete;
		Timer& operator=(Timer&&) noexcept = delete;

		void Start();		
		void Update(const float elapsedSec);
		void Stop();

		bool Done() const { return m_IsDone; };
		bool IsTiming() const { return m_IsTiming; }
		float GetTimeToGo() const { return m_TimeToGo; }

	private:
		bool m_IsTiming;
		bool m_IsDone;
		float m_CurrentTime;
		float m_MaxTime;
		float m_TimeToGo;
};


#pragma once

#include <chrono>

class cExplosionBudget
{
public:

	cExplosionBudget(unsigned a_MaxExplosions = 4, std::chrono::milliseconds a_MaxDuration = std::chrono::milliseconds(10)) :
		m_MaxExplosions(a_MaxExplosions),
		m_MaxDuration(a_MaxDuration)
	{
	}

	void Reset()
	{
		m_Explosions = 0;
	}

	bool TryConsume(std::chrono::steady_clock::time_point a_Now = std::chrono::steady_clock::now())
	{
		if ((m_Explosions >= m_MaxExplosions) || ((m_Explosions > 0) && (a_Now - m_Start >= m_MaxDuration)))
		{
			return false;
		}
		if (m_Explosions == 0)
		{
			m_Start = a_Now;
		}
		++m_Explosions;
		return true;
	}

private:
	unsigned m_MaxExplosions;
	std::chrono::milliseconds m_MaxDuration;
	unsigned m_Explosions = 0;
	std::chrono::steady_clock::time_point m_Start;
};

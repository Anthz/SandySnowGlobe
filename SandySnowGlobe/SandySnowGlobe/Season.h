#pragma once
#include <string>
#include "ParticleSystem.h"

class Season
{
public:
	enum Seasons
	{
		SPRING,
		SUMMER,
		AUTUMN,
		WINTER,
		END
	};

	Season();
	explicit Season(Seasons startSeason);
	~Season();

	Season::Seasons *CurrentSeason() { return &currentSeason; }
	void CurrentSeason(const Season::Seasons &val) { currentSeason = val; }

	std::string *GetSeasonString() { return &seasonString; }

	float GetRainChance(Seasons s) const;
	float GetSnowChance(Seasons s) const;

	friend Season& const operator++(Season &s) {
		s.currentSeason++;
		s.seasonString = s.ToString(s.currentSeason);
		return s;
	}

private:
	Seasons currentSeason;
	std::string seasonString;

	std::string ToString(Seasons s) const;

	friend const Seasons& operator++(Seasons &s) {
		s = static_cast<Seasons>(static_cast<int>(s)+1);
		if(s == Seasons::END)
			s = Seasons::SPRING;
		return s;
	}
};


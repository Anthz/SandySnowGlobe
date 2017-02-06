#include "Season.h"

Season::Season(Seasons startSeason)
{
	currentSeason = startSeason;
	seasonString = ToString(currentSeason);
}

Season::Season()
{
	currentSeason = SPRING;
}

Season::~Season()
{
}

float Season::GetRainChance(Seasons s) const
{
	switch(s)
	{
		case Season::SPRING:
			return 0.3f;
			break;
		case Season::SUMMER:
			return 0.2f;
			break;
		case Season::AUTUMN:
			return 0.3f;
			break;
		case Season::WINTER:
			return 0.5f;
			break;
	}
}

float Season::GetSnowChance(Seasons s) const
{
	switch(s)
	{
		case Season::SPRING:
			return 0.1f;
			break;
		case Season::SUMMER:
			return 0.0f;
			break;
		case Season::AUTUMN:
			return 0.1f;
			break;
		case Season::WINTER:
			return 0.5f;
			break;
	}
}

std::string Season::ToString(Seasons s) const
{
	switch(s)
	{
	case Season::SPRING:
		return std::string("Spring");
		break;
	case Season::SUMMER:
		return std::string("Summer");
		break;
	case Season::AUTUMN:
		return std::string("Autumn");
		break;
	case Season::WINTER:
		return std::string("Winter");
		break;
	}
}
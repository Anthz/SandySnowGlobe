#include "DXUtil.h"

namespace Validation
{
	//simple error checking replacement for DXERR (only found in old SDK)
	bool ErrCheck(HRESULT result, char const *file, const int &line, const std::string &desc)
	{
		std::string msg;
		std::string fileName;
		fileName.append(file);
		fileName = fileName.substr(fileName.find_last_of("/\\") + 1);

		if(FAILED(result))
		{
			msg = "Failed: " + fileName + " Ln " + std::to_string(line) + " " + desc;
			Logger::Log(msg);
			OutputDebugString((msg + "\n").c_str());
			return false;
		}
		else
		{
			msg = "Success: " + fileName + " Ln " + std::to_string(line) + " " + desc;
			Logger::Log(msg);
			OutputDebugString((msg + "\n").c_str());
			return true;
		}
	}
}

namespace Logger
{
	std::string path = "";
	std::string logName = "";
	std::string filePath = "";
	std::string date = "";
	std::string currentTime = "";
	std::ofstream fileStream;
	bool initialised = false;

	void InitLogFile(const std::string &folderPath)
	{
		time_t now = time(0);
		tm localTime;
		localtime_s(&localTime, &now);
		date = std::to_string(localTime.tm_mday) + "-" + std::to_string(localTime.tm_mon + 1) + "-" + std::to_string(localTime.tm_year + 1900);
		currentTime = std::to_string(localTime.tm_hour) + ":" + std::to_string(localTime.tm_min) + ":" + std::to_string(localTime.tm_sec);

		path = folderPath;
		logName = "Log - " + date + ".txt";	//remove /n for filename
		filePath = path + "\\" + logName;

		if(CreateDirectory(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			fileStream.open(filePath, std::ofstream::in);	//check if it exists
			if(fileStream.is_open())
			{
				fileStream.close();
				fileStream.open(filePath, std::ofstream::out | std::ofstream::app);	//if exists, open for append
				fileStream << "\n\n";
			}
			else
			{
				fileStream.close();
				fileStream.open(filePath);	//if it doesn't, create file with read/write permissions
			}

			if(fileStream.is_open())
			{
				fileStream << "[" << currentTime << "]" << " LOG START" << "\n";
				fileStream.close();
			}

			initialised = true;
		}
		else
		{
			OutputDebugString("Failed to find/create folder.");
		}
	}

	/// <summary>
	/// Prefixes log message with current time + formatting
	/// </summary>
	/// <param name="msg">Message to be logged</param>
	void Log(const std::string &msg)
	{
		if(initialised)
		{
			fileStream.open(filePath, std::ofstream::out | std::ofstream::app);
			if(fileStream.is_open())
			{
				fileStream << "[" << currentTime << "]" << " " << msg << "\n";
				fileStream.close();
			}
		}
		else
		{
			OutputDebugString("Log not initialised.");
		}
	}
}
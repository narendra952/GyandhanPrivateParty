#pragma once
#include <fstream>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#define DEGREE_TO_RADIANS 0.0174532925199433

struct User_Information
{
	std::string m_Name;
	double m_Latitude;
	double m_Longitude;
};

class PartyAtGyandhan
{
private:
	const unsigned int MAXIMUM_RADIUS_FROM_GYANDHAN_OFFICE = 100; // 100 Km
	const double GYANDHAN_LATITUDE = 28.521134;
	const double GYANDHAN_LONGITUDE = 77.206567;
	const double EARTH_RADIUS = 6371; //6371 km
public:
	static void GetUsersNearGyandhanOffice(const std::string& i_UserInfoFile, std::string& o_UsersNearGyandhanStoredInAFile);
private:
	static void GetUserDataFromCSVFile(const std::string& i_UserInfoFile, std::map<unsigned int, std::vector<User_Information>>& o_UserDatabase);
	static bool IsUserNearGyandhanOffice(User_Information& i_UserInfo);
	static void StoreUserDetailsNearGyandhanOffice(std::map<unsigned int, std::vector<User_Information>>& i_usersNearGyandhan, std::string& o_UsersNearGyandhanStoredInAFile);
};

inline void PartyAtGyandhan::GetUserDataFromCSVFile(const std::string& i_UserInfoFile, std::map<unsigned int, std::vector<User_Information>>& o_UserDatabase)
{
	std::ifstream user_Information_File_Stream(i_UserInfoFile);

	std::string tempString;
	std::getline(user_Information_File_Stream, tempString);// skip first line(ID,name,latitude, longitude)

	User_Information currentUser;
	std::string user_Info_line;
	while (std::getline(user_Information_File_Stream, user_Info_line))// Example: user_ID = "1452,gyandhan,-54.26,48.15";	
	{
		// user_ID = "1452";	
		std::string user_ID = user_Info_line.substr(0, user_Info_line.find_first_of(','));

		// "gyandhan,-54.26,48.15";	
		user_Info_line = user_Info_line.substr(user_Info_line.find_first_of(',') + 1, std::string::npos);

		//  "gyandhan";	
		std::string user_Name = user_Info_line.substr(0, user_Info_line.find_first_of(','));
		currentUser.m_Name = user_Name;

		// "-54.26,48.15"	
		user_Info_line = user_Info_line.substr(user_Info_line.find_first_of(',') + 1, std::string::npos);

		// "-54.26"
		std::string latitude = user_Info_line.substr(0, user_Info_line.find_first_of(','));
		currentUser.m_Latitude = std::stod(latitude);

		// 48.15"
		std::string longitude = user_Info_line.substr(user_Info_line.find_first_of(',') + 1, std::string::npos);
		currentUser.m_Longitude = std::stod(longitude);

		o_UserDatabase[std::stoi(user_ID)].push_back(currentUser);
	}
}

inline bool PartyAtGyandhan::IsUserNearGyandhanOffice(User_Information& i_UserInfo)
{
	PartyAtGyandhan partyAtGyandhan;
	double longitudeDifferenceBetweenUserAndGyandhan = DEGREE_TO_RADIANS* (i_UserInfo.m_Longitude - partyAtGyandhan.GYANDHAN_LONGITUDE);
	double gyandhanLatitudeInRadians = partyAtGyandhan.GYANDHAN_LATITUDE*DEGREE_TO_RADIANS;
	double gyandhanLongitudeInRadians = partyAtGyandhan.GYANDHAN_LONGITUDE*DEGREE_TO_RADIANS;
	double userLatitudeInRadians = i_UserInfo.m_Latitude*DEGREE_TO_RADIANS;
	double userLongitudeInRadians = i_UserInfo.m_Longitude*DEGREE_TO_RADIANS;

	double distanceBetweenUserAndGyandhanOffice = partyAtGyandhan.EARTH_RADIUS * acos((sin(gyandhanLatitudeInRadians)*sin(userLatitudeInRadians)) + (cos(gyandhanLongitudeInRadians)*cos(userLongitudeInRadians)*cos(longitudeDifferenceBetweenUserAndGyandhan)));

	if (distanceBetweenUserAndGyandhanOffice > partyAtGyandhan.MAXIMUM_RADIUS_FROM_GYANDHAN_OFFICE)
		return false;
	else
		return true;
}

inline void PartyAtGyandhan::GetUsersNearGyandhanOffice(const std::string& i_UserInfoFile, std::string & o_UsersNearGyandhanStoredInAFile)
{
	std::map<unsigned int, std::vector<User_Information>> user_Database;// Repeating ID is mapped with user information
	PartyAtGyandhan::GetUserDataFromCSVFile(i_UserInfoFile, user_Database);

	//Delete users far away in-place with out creating extra memory
	for (auto usersWithSameIDIterator = user_Database.begin(); usersWithSameIDIterator != user_Database.end();)
	{
		//std::vector<User_Information> usersWithSameID = usersWithSameIDIterator->second;
		for (auto userIndex = usersWithSameIDIterator->second.begin(); userIndex != usersWithSameIDIterator->second.end();)
		{
			if (!IsUserNearGyandhanOffice(*userIndex))
			{
				userIndex = usersWithSameIDIterator->second.erase(userIndex);
			}
			else
			{
				++userIndex;
			}
		}
		if (usersWithSameIDIterator->second.size() == 0)
			user_Database.erase(usersWithSameIDIterator++);
		else
			++usersWithSameIDIterator;
	}

	//in map keys are always sorted, so pushing ID and name from map
	StoreUserDetailsNearGyandhanOffice(user_Database, o_UsersNearGyandhanStoredInAFile);
}

inline void PartyAtGyandhan::StoreUserDetailsNearGyandhanOffice(std::map<unsigned int, std::vector<User_Information>>& i_usersNearGyandhan, std::string & o_UsersNearGyandhanStoredInAFile)
{
	std::ofstream outputFile(o_UsersNearGyandhanStoredInAFile);
	if (outputFile.is_open() == false)
		return;

	for (const auto& usersWithSameID : i_usersNearGyandhan)
	{
		for (const auto& user : usersWithSameID.second)
		{
			outputFile << usersWithSameID.first << "," << user.m_Name << std::endl; // 125, narendra (ID, NAME)
		}
	}
	outputFile.close();
}


// UsersNearGyandhanOffice.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GyandhanParty.h"

const std::string USER_INFORMATION_FILE = "User_Data.csv"; // user data containing IDs, names, latitudes, longitudes

int main()
{
	std::string usersNearGyandhan = "UsersListAttendingParty.csv";
	PartyAtGyandhan::GetUsersNearGyandhanOffice(USER_INFORMATION_FILE, usersNearGyandhan);

	return 0;
}


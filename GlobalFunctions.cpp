#include "stdafx.h"
#include "GlobalFunctions.h"
#include "Exam_HelperStructs.h"

chrono::time_point<std::chrono::system_clock> Now( )
{
	return std::chrono::system_clock::now( );
}

float ElapsedBetween( const time_point& from, const time_point& to )
{
	return float( std::chrono::duration_cast<std::chrono::seconds>( to - from ).count( ) );
}

float ElapsedSince( const time_point& timeOfDiscovery )
{
	return ElapsedBetween( timeOfDiscovery, Now( ) );
}

bool operator==( const ItemInfo& lhs, const ItemInfo& rhs )
{
	return lhs.Location == rhs.Location && lhs.Type == rhs.Type;
}

COORD GetConsoleCursorPosition( )
{
	//std::cout.flush( );
	HANDLE hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if( GetConsoleScreenBufferInfo( hConsoleOutput, &cbsi ) )
	{
		return cbsi.dwCursorPosition;
	}
	// The function failed. Call GetLastError() for details.
	COORD invalid = { 0, 0 };
	return invalid;
}

void GoToConsoleCursorPosition( COORD pos )
{
	HANDLE output = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleCursorPosition( output, pos );
	std::cout << "                                                                                   \r";
	std::cout.flush( );
}

#pragma once
#include <chrono>

struct ItemInfo;
using time_point = std::chrono::time_point<std::chrono::system_clock>;

time_point Now( );
float ElapsedBetween( const time_point& from, const time_point& to );
float ElapsedSince( const time_point& timeOfDiscovery );

bool operator==( const ItemInfo& lhs, const ItemInfo& rhs );

// Very hacky code, but I want to get rid of unwanted prints
COORD GetConsoleCursorPosition( );
void GoToConsoleCursorPosition( COORD pos );

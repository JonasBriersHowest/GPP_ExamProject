#pragma once


struct WorldState
{
	UINT health;
	UINT energy;
	UINT housesAvailable;
	UINT pistolsAvailable;
	UINT foodsAvailable;
	UINT medkitsAvailable;
	bool isInHouse;
	uint8_t pistolsInInventory;
	uint8_t foodsInInventory;
	uint8_t medkitsInInventory;
	uint8_t enemiesInFov;
	uint8_t killCount;
	bool aimedAtEnemy;
	bool wasBitten;
	bool wantsToSpin;
	bool isInDanger;
	bool purgeZoneInFov;
};
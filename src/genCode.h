#pragma once
#include <cstdint>
#include "geom.h"

class Player
{
public:
	void* vtable;
	Vector3 headpos; //0x0004
	Vector3 velocity; //0x0010
	Vector3 N0000014D; //0x001C
	Vector3 pos; //0x0028
	float yaw; //0x0034
	float pitch; //0x0038
	float roll; //0x003C
	char pad_0040[172]; //0x0040
	int32_t health; //0x00EC
	int32_t armor; //0x00F0
	char pad_00F4[20]; //0x00F4
	int32_t pistolreserve; //0x0108
	char pad_010C[8]; //0x010C
	int32_t riflereserve; //0x0114
	char pad_0118[20]; //0x0118
	int32_t pistolammo; //0x012C
	char pad_0130[8]; //0x0130
	int32_t rifleammo; //0x0138
	char pad_013C[8]; //0x013C
	int32_t grenadecount; //0x0144
	char pad_0148[188]; //0x0148
	bool attacking; //0x0204
	char name[16]; //0x0205
	char pad_0215[247]; //0x0215
	uint8_t team; //0x030C
	char pad_030D[3]; //0x030D
}; //Size: 0x0310
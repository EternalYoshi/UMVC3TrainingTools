//By Eternal Yoshi. Thanks to ermaccer for the new hook & HKHaan and Sheep for collecting the pointers and data needed to make this possible.
#include "Menu.h"
#include "Settings.h"

#include "../gui/notifications.h"
#include "../gui/imgui/imgui.h"

#include "../umvc3/Camera.h"

#define undefined4 int

float p1Pos = -125.0f;
float p2Pos = 125.0f;
bool restarted = false;
int restartTimer = 0;

UMVC3Menu* TheMenu = new UMVC3Menu();

int runaheadcount = 0;
bool pAddrSet = false;

uintptr_t mysterytable;
uintptr_t block2;
uintptr_t ptable;
int P1Character1ID;
int P1Character2ID;
int P1Character3ID;
int P2Character1ID;
int P2Character2ID;
int P2Character3ID;
uintptr_t P1Character1Data;
uintptr_t P1Character2Data;
uintptr_t P1Character3Data;
uintptr_t P2Character1Data;
uintptr_t P2Character2Data;
uintptr_t P2Character3Data;
int MODOKLOU = 1;
int FrankLevel = 1;
float PrestigePoints = 0.0;
int WrightEvidenceA = -1;
int WrightEvidenceB = -1;
int WrightEvidenceC = -1;
int SaveStateSlot = 0;
int DormRed = 0;
int DormBlue = 0;
bool DormSpellSet = false;
bool EndlessInstalls = false;
int DeadpoolTeleportCount = 0;
bool FreezeDeadpoolTPCounter = false;
bool P1Character1Dead = false;
bool P1Character2Dead = false;
bool P1Character3Dead = false;
bool P2Character1Dead = false;
bool P2Character2Dead = false;
bool P2Character3Dead = false;
bool EndlessXFactor = false;
bool SpecialFeature = false;
bool HitboxDisplay = false;
bool SetIndividualHP = false;
bool AlsoSetRedHealth = false;
float P1Char1Health = 1.0;
float P1Char1RedHealth = 1.0;
float P1Char2Health = 1.0;
float P1Char2RedHealth = 1.0;
float P1Char3Health = 1.0;
float P1Char3RedHealth = 1.0;
float P2Char1Health = 1.0;
float P2Char1RedHealth = 1.0;
float P2Char2Health = 1.0;
float P2Char2RedHealth = 1.0;
float P2Char3Health = 1.0;
float P2Char3RedHealth = 1.0;
bool P1Char1Slow = false;
bool P1Char2Slow = false;
bool P1Char3Slow = false;
bool P2Char1Slow = false;
bool P2Char2Slow = false;
bool P2Char3Slow = false;

int GameMode;

const char* EvidenceTypes[] = { "Nothing","Photo","Watch", "Vase","Folder","Phone","Knife","Sunglasses","Beer","Bonsai", "Doll","Figurine","Plunger","Chicken" };
const char* EvidenceTypesTwo[] = { "Nothing","Photo","Watch", "Vase","Folder","Phone","Knife","Sunglasses","Beer","Bonsai", "Doll","Figurine","Plunger","Chicken" };
const char* EvidenceTypesThree[] = { "Nothing","Photo","Watch", "Vase","Folder","Phone","Knife","Sunglasses","Beer","Bonsai", "Doll","Figurine","Plunger","Chicken" };
static const char* selected_item = "Nothing";
static const char* selected_itemTwo = "Nothing";
static const char* selected_itemThree = "Nothing";

struct MarvelVector {
	float x;
	float y;
	float xVelocity;
};
enum EGroundedState :uint8_t {
	NoGround = 0,
	Grounded = 0xF4,
	InAir = 0xF2,
};

enum ETagState :uint8_t {
	NoTag = 0,
	Active = 0x0D,
	Inactive = 0x09,
	Active2 = 0x68,
	TagginOut = 0xB4,
	TagginIn = 0x6c
};

struct Fighter { // TODO map this out
	char offset[0x14];
	ETagState tagState;
	char offset2[0x3B];
	MarvelVector vector;
	char offset3[0x10];
	EGroundedState  groundedState;
	char offset4[0xc7];
	uint16_t specialState;//specialstate?
	char offset5[0x528];
	uint32_t anim; // anim??
};

struct ScriptableFighter {
	Fighter* fighter = 0;
	intptr_t fighterController = 0; // TODO find better name for this
	char* name = nullptr;
	uintptr_t tickPtr = 0x14004bd30;
	void Tick() {
		((void(__fastcall*)(longlong*))_addr(tickPtr))((longlong*)fighter);
	}
	const char* GetGroundedState() {
		switch (fighter->groundedState)
		{
		case Grounded:
			return "Grounded";
		case InAir:
			return "InAir";
		default:
			return "Unkown";
			break;
		}
	}
	const char* GetTagState() {
		switch (fighter->tagState)
		{
		case Active:
		case Active2:
			return "Active";
		case Inactive:
			return "Inactive";
		case TagginOut:
			return "TagginOut";
		case TagginIn:
			return "TagginIn";
		default:
			return "Unkown";
			break;
		}
	}
};

struct Recording {

	int FrameCount;
	char* InputBinary[3844];

};

enum WrightEvidence
{
	Nothing = -1,
	Photo = 0,
	Watch = 1,
	Vase = 2,
	Folder = 3,
	Phone = 4,
	Knife = 5,
	Sunglasses = 6,
	Beer = 7,
	Bonsai = 8,
	Doll = 9,
	Figurine = 10,
	Plunger = 11,
	Chicken = 12,
};

ScriptableFighter scriptableFighters[6] = { 0 };


//Adjusts Frank's Level.
void ChangeFrankLevel(int FrankLevel)
{
	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	//Sets Prestige Points based on level chosen.

	switch (FrankLevel)
	{
	case 1:
		PrestigePoints = 0.0;
		break;
	case 2:
		PrestigePoints = 5.0;
		break;
	case 3:
		PrestigePoints = 20.0;
		break;
	case 4:
		PrestigePoints = 50.0;
		break;
	case 5:
		PrestigePoints = 100.0;
		break;
	default:
		PrestigePoints = 0.0;
		break;

	}

	if (P1Character1ID == 9)
	{

		uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
		*((int*)(P1Character1Data + 0x69C4)) = FrankLevel;
		eLog::Message(__FUNCTION__, "P1Character1 Frank's Level: " + *((int*)(P1Character1Data + 0x69C4)));
		//eLog::Message(__FUNCTION__, "P1Character1 Frank's PrestigePoints: " + *((float*)(P1Character1Data + 0x69DC)));
		//std::cout << "P1Character1 Frank's Level: " << std::hex << *((int*)(P1Character1Data + 0x69C4)) << std::endl;
		//std::cout << "P1Character1 Frank's PrestigePoints: " << std::hex << *((float*)(P1Character1Data + 0x69DC)) << std::endl;
		*((float*)(P1Character1Data + 0x69DC)) = PrestigePoints;

	}

	if (P1Character2ID == 9)
	{
		uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
		*((int*)(P1Character2Data + 0x69C4)) = FrankLevel;
		*((float*)(P1Character2Data + 0x69DC)) = PrestigePoints;
	}

	if (P1Character3ID == 9)
	{
		uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
		*((int*)(P1Character3Data + 0x69C4)) = FrankLevel;
		*((float*)(P1Character3Data + 0x69DC)) = PrestigePoints;
	}

	if (P2Character1ID == 9)
	{
		uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
		*((int*)(P2Character1Data + 0x69C4)) = FrankLevel;
		*((float*)(P2Character1Data + 0x69DC)) = PrestigePoints;
	}

	if (P2Character2ID == 9)
	{
		uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
		*((int*)(P2Character2Data + 0x69C4)) = FrankLevel;
		*((float*)(P2Character2Data + 0x69DC)) = PrestigePoints;
	}

	if (P2Character3ID == 9)
	{
		uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
		*((int*)(P2Character3Data + 0x69C4)) = FrankLevel;
		*((float*)(P2Character3Data + 0x69DC)) = PrestigePoints;
	}
}

//Sets Evidence based on selection.
void ChangeWrightEvidence()
{
	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	//Checks Character ID.
	if (P1Character1ID == 23)
	{
		uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
		*((int*)(P1Character1Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P1Character1Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P1Character1Data + 0x6920)) = WrightEvidenceC;
	}
	if (P1Character2ID == 23)
	{
		uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
		*((int*)(P1Character2Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P1Character2Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P1Character2Data + 0x6920)) = WrightEvidenceC;
	}
	if (P1Character3ID == 23)
	{
		uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
		*((int*)(P1Character3Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P1Character3Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P1Character3Data + 0x6920)) = WrightEvidenceC;
	}
	if (P2Character1ID == 23)
	{
		uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
		*((int*)(P2Character1Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P2Character1Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P2Character1Data + 0x6920)) = WrightEvidenceC;
	}
	if (P2Character2ID == 23)
	{
		uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
		*((int*)(P2Character2Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P2Character2Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P2Character2Data + 0x6920)) = WrightEvidenceC;
	}
	if (P2Character3ID == 23)
	{
		uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
		*((int*)(P2Character3Data + 0x6918)) = WrightEvidenceA;
		*((int*)(P2Character3Data + 0x691C)) = WrightEvidenceB;
		*((int*)(P2Character3Data + 0x6920)) = WrightEvidenceC;
	}

}

//Adjusts MODOK's level of Understanding.
void ChangeMODOKUnderstanding(int MODOKLOU)
{
	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	if (P1Character1ID == 38)
	{
		uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
		*((int*)(P1Character1Data + 0x68F0)) = MODOKLOU;
		*((int*)(P1Character1Data + 0x68F4)) = MODOKLOU;
	}

	if (P1Character2ID == 38)
	{
		uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
		*((int*)(P1Character2Data + 0x68F0)) = MODOKLOU;
		*((int*)(P1Character2Data + 0x68F4)) = MODOKLOU;
	}

	if (P1Character3ID == 38)
	{
		uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
		*((int*)(P1Character3Data + 0x68F0)) = MODOKLOU;
		*((int*)(P1Character3Data + 0x68F4)) = MODOKLOU;
	}

	if (P2Character1ID == 38)
	{
		uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
		*((int*)(P2Character1Data + 0x68F0)) = MODOKLOU;
		*((int*)(P2Character1Data + 0x68F4)) = MODOKLOU;
	}

	if (P2Character2ID == 38)
	{
		uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
		*((int*)(P2Character2Data + 0x68F0)) = MODOKLOU;
		*((int*)(P2Character2Data + 0x68F4)) = MODOKLOU;
	}

	if (P2Character3ID == 38)
	{
		uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
		*((int*)(P2Character3Data + 0x68F0)) = MODOKLOU;
		*((int*)(P2Character3Data + 0x68F4)) = MODOKLOU;
	}
}

int EvidenceNameToID(const char* name)
{

	if (name == "Nothing")
	{
		return -1;
	}
	else if (name == "Photo")
	{
		return 0;
	}
	else if (name == "Watch")
	{
		return 1;
	}
	else if (name == "Vase")
	{
		return 2;
	}
	else if (name == "Folder")
	{
		return 3;
	}
	else if (name == "Phone")
	{
		return 4;
	}
	else if (name == "Knife")
	{
		return 5;
	}
	else if (name == "Sunglasses")
	{
		return 6;
	}
	else if (name == "Beer")
	{
		return 7;
	}
	else if (name == "Bonsai")
	{
		return 8;
	}
	else if (name == "Doll")
	{
		return 9;
	}
	else if (name == "Figurine")
	{
		return 10;
	}
	else if (name == "Plunger")
	{
		return 11;
	}
	else if (name == "Chicken")
	{
		return 12;
	}

	return -1;

}

//Sets Dormammu Spell Charges.
void SetDormSpellLevels()
{

	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	if (*((int*)(mysterytable + 0xAA0)) != 0)
	{

		//Sets Both Spell Values.
		if (P1Character1ID == 39)
		{
			uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
			*((int*)(P1Character1Data + 0x68F0)) = DormRed;
			*((int*)(P1Character1Data + 0x68F4)) = DormBlue;
		}

		if (P1Character2ID == 39)
		{
			uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
			*((int*)(P1Character2Data + 0x68F0)) = DormRed;
			*((int*)(P1Character2Data + 0x68F4)) = DormBlue;
		}

		if (P1Character3ID == 39)
		{
			uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
			*((int*)(P1Character3Data + 0x68F0)) = DormRed;
			*((int*)(P1Character3Data + 0x68F4)) = DormBlue;
		}

		if (P2Character1ID == 39)
		{
			uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
			*((int*)(P2Character1Data + 0x68F0)) = DormRed;
			*((int*)(P2Character1Data + 0x68F4)) = DormBlue;
		}

		if (P2Character2ID == 39)
		{
			uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
			*((int*)(P2Character2Data + 0x68F0)) = DormRed;
			*((int*)(P2Character2Data + 0x68F4)) = DormBlue;
		}

		if (P2Character3ID == 39)
		{
			uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
			*((int*)(P2Character3Data + 0x68F0)) = DormRed;
			*((int*)(P2Character3Data + 0x68F4)) = DormBlue;
		}

	}

}

//Sets Infnite Install Time for Several Characters.
void EndlessInstallBoolUpdate()
{
	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	if (*((int*)(mysterytable + 0xAA0)) != 0)
	{

		//Checks for these characters as these are the ones compatible with this method.
		if (P1Character1ID == 1 || P1Character1ID == 7 || P1Character1ID == 11 || P1Character1ID == 13 || P1Character1ID == 15 || P1Character1ID == 16
			|| P1Character1ID == 17 || P1Character1ID == 20 || P1Character1ID == 21 || P1Character1ID == 22 || P1Character1ID == 23 || P1Character1ID == 24 || P1Character1ID == 28
			|| P1Character1ID == 37 || P1Character1ID == 44)
		{

			uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
			if (*((char*)(P1Character1Data + 0x15F8)) == 17 || *((char*)(P1Character1Data + 0x15F8)) == 16)
			{
				*((float*)(P1Character1Data + 0x1600)) = 216000;
			}
		}

		//Checks for these characters as these are the ones compatible with this method.
		if (P1Character2ID == 1 || P1Character2ID == 7 || P1Character2ID == 11 || P1Character2ID == 13 || P1Character2ID == 15 || P1Character2ID == 16
			|| P1Character2ID == 17 || P1Character2ID == 20 || P1Character2ID == 21 || P1Character2ID == 22 || P1Character2ID == 23 || P1Character2ID == 24 || P1Character2ID == 28
			|| P1Character2ID == 37 || P1Character2ID == 44)
		{

			uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
			if (*((char*)(P1Character2Data + 0x15F8)) == 17 || *((char*)(P1Character2Data + 0x15F8)) == 16)
			{
				*((float*)(P1Character2Data + 0x1600)) = 216000;
			}
		}

		//Checks for these characters as these are the ones compatible with this method.
		if (P1Character3ID == 1 || P1Character3ID == 7 || P1Character3ID == 11 || P1Character3ID == 13 || P1Character3ID == 15 || P1Character3ID == 16
			|| P1Character3ID == 17 || P1Character3ID == 20 || P1Character3ID == 21 || P1Character3ID == 22 || P1Character3ID == 23 || P1Character3ID == 24 || P1Character3ID == 28
			|| P1Character3ID == 37 || P1Character3ID == 44)
		{

			uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
			if (*((char*)(P1Character3Data + 0x15F8)) == 17 || *((char*)(P1Character3Data + 0x15F8)) == 16)
			{
				*((float*)(P1Character3Data + 0x1600)) = 216000;
			}
		}

		//Checks for these characters as these are the ones compatible with this method.
		if (P2Character1ID == 1 || P2Character1ID == 7 || P2Character1ID == 11 || P2Character1ID == 13 || P2Character1ID == 15 || P2Character1ID == 16
			|| P2Character1ID == 17 || P2Character1ID == 20 || P2Character1ID == 21 || P2Character1ID == 22 || P2Character1ID == 23 || P2Character1ID == 24 || P2Character1ID == 28
			|| P2Character1ID == 37 || P2Character1ID == 44)
		{

			uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
			if (*((char*)(P2Character1Data + 0x15F8)) == 17 || *((char*)(P2Character1Data + 0x15F8)) == 16)
			{
				*((float*)(P2Character1Data + 0x1600)) = 216000;
			}
		}

		//Checks for these characters as these are the ones compatible with this method.
		if (P2Character2ID == 1 || P2Character2ID == 7 || P2Character2ID == 11 || P2Character2ID == 13 || P2Character2ID == 15 || P2Character2ID == 16
			|| P2Character2ID == 17 || P2Character2ID == 20 || P2Character2ID == 21 || P2Character2ID == 22 || P2Character2ID == 23 || P2Character2ID == 24 || P2Character2ID == 28
			|| P2Character2ID == 37 || P2Character2ID == 44)
		{

			uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
			if (*((char*)(P2Character2Data + 0x15F8)) == 17 || *((char*)(P2Character2Data + 0x15F8)) == 16)
			{
				*((float*)(P2Character2Data + 0x1600)) = 216000;
			}
		}

		//Checks for these characters as these are the ones compatible with this method.
		if (P2Character3ID == 1 || P2Character3ID == 7 || P2Character3ID == 11 || P2Character3ID == 13 || P2Character3ID == 15 || P2Character3ID == 16
			|| P2Character3ID == 17 || P2Character3ID == 20 || P2Character3ID == 21 || P2Character3ID == 22 || P2Character3ID == 23 || P2Character3ID == 24 || P2Character3ID == 28
			|| P2Character3ID == 37 || P2Character3ID == 44)
		{

			uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
			if (*((char*)(P2Character3Data + 0x15F8)) == 17 || *((char*)(P2Character3Data + 0x15F8)) == 16)
			{
				*((float*)(P2Character3Data + 0x1600)) = 216000;
			}
		}
	}
}

void SetDeadpoolTeleport()
{

	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	int P1Character1ID = *(uintptr_t*)_addr((ptable + 0x44));
	int P1Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58));
	int P1Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58));
	int P2Character1ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58));
	int P2Character2ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58));
	int P2Character3ID = *(uintptr_t*)_addr((ptable + 0x44 + 0x58 + 0x58 + 0x58 + 0x58 + 0x58));

	if (*((int*)(mysterytable + 0xAA0)) != 0)
	{
		if (P1Character1ID == 40)
		{
			uintptr_t P1Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0);
			*((int*)(P1Character1Data + 0x68F0)) = DeadpoolTeleportCount;
		}

		if (P1Character2ID == 40)
		{
			uintptr_t P1Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438));
			*((int*)(P1Character2Data + 0x68F0)) = DeadpoolTeleportCount;
		}

		if (P1Character3ID == 40)
		{
			uintptr_t P1Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 2));
			*((int*)(P1Character3Data + 0x68F0)) = DeadpoolTeleportCount;
		}

		if (P2Character1ID == 40)
		{
			uintptr_t P2Character1Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 3));
			*((int*)(P2Character1Data + 0x68F0)) = DeadpoolTeleportCount;
		}

		if (P2Character2ID == 40)
		{
			uintptr_t P2Character2Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 4));
			*((int*)(P2Character2Data + 0x68F0)) = DeadpoolTeleportCount;
		}

		if (P2Character3ID == 40)
		{
			uintptr_t P2Character3Data = *(uintptr_t*)_addr(mysterytable + 0xAA0 + (0x438 * 5));
			*((int*)(P2Character3Data + 0x68F0)) = DeadpoolTeleportCount;
		}
	}

}

/*
//Saves Raw Replay Data
void SaveReplayP1()
{

	uintptr_t block3 = *(uintptr_t*)_addr(0x140D510A0);
	uintptr_t P1RecordingData = *(uintptr_t*)_addr(block3 + 0x90);
	//int P1RecordingFrameTotal = *(uintptr_t*)_addr(P1RecordingData + 0x40);
	uintptr_t P1RecordingRawInput = *(uintptr_t*)_addr(P1RecordingData + 0x44);

	Recording P1Recording = {
	*(uintptr_t*)_addr(P1RecordingData + 0x40),

	};
	//*P1Recording.FrameCount = *(uintptr_t*)_addr(P1RecordingData + 0x40);



}
*/
void DeathBoolUpdate()
{

}

//Attempts to set characters at the specified health values.
void SetIndividualCharacterHealth()
{

	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);

	//Character Actor Data Pointers.
	uintptr_t P1C1 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 0));
	uintptr_t P1C2 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 1));
	uintptr_t P1C3 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 2));
	uintptr_t P2C1 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 3));
	uintptr_t P2C2 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 4));
	uintptr_t P2C3 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 5));

	//Gets the Max HP Value for the active characters. They are ints instead of floats.
	int P1C1MaxHP = *(uintptr_t*)_addr((P1C1 + 0x154C));
	int P1C2MaxHP = *(uintptr_t*)_addr((P1C2 + 0x154C));
	int P1C3MaxHP = *(uintptr_t*)_addr((P1C3 + 0x154C));
	int P2C1MaxHP = *(uintptr_t*)_addr((P2C1 + 0x154C));
	int P2C2MaxHP = *(uintptr_t*)_addr((P2C2 + 0x154C));
	int P2C3MaxHP = *(uintptr_t*)_addr((P2C3 + 0x154C));

	if (SetIndividualHP == true)
	{

		//P1 Character 1.
		*(float*)_addr(P1C1 + 0x1550) = P1C1MaxHP * P1Char1Health;

		//P1 Character 2.
		*(float*)_addr(P1C2 + 0x1550) = P1C2MaxHP * P1Char3Health;

		//P1 Character 3.
		*(float*)_addr(P1C3 + 0x1550) = P1C3MaxHP * P1Char3Health;

		//P2 Character 1.
		*(float*)_addr(P2C1 + 0x1550) = P2C1MaxHP * P2Char1Health;

		//P2 Character 2.
		*(float*)_addr(P2C2 + 0x1550) = P2C2MaxHP * P2Char2Health;

		//P2 Character 3.
		*(float*)_addr(P2C3 + 0x1550) = P2C3MaxHP * P2Char3Health;

		//Applies the requested values to the characters' red health as well.
		if(AlsoSetRedHealth == true)
		{
			//P1 Character 1.
			*(float*)_addr(P1C1 + 0x1558) = P1C1MaxHP * P1Char1Health;

			//P1 Character 2.
			*(float*)_addr(P1C2 + 0x1558) = P1C2MaxHP * P1Char3Health;

			//P1 Character 3.
			*(float*)_addr(P1C3 + 0x1558) = P1C3MaxHP * P1Char3Health;

			//P2 Character 1.
			*(float*)_addr(P2C1 + 0x1558) = P2C1MaxHP * P2Char1Health;

			//P2 Character 2.
			*(float*)_addr(P2C2 + 0x1558) = P2C2MaxHP * P2Char2Health;

			//P2 Character 3.
			*(float*)_addr(P2C3 + 0x1558) = P2C3MaxHP * P2Char3Health;
		}

	}



}

void SetSlowStatus()
{
	//Gets the needed values in memory.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);

	//Character Actor Data Pointers.
	uintptr_t P1C1 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 0));
	uintptr_t P1C2 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 1));
	uintptr_t P1C3 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 2));
	uintptr_t P2C1 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 3));
	uintptr_t P2C2 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 4));
	uintptr_t P2C3 = *(uintptr_t*)_addr(mysterytable + 0xaa0 + (0x438 * 5));










}

//Carries over changes To restart.
void RestartWithChanges()
{
	ChangeFrankLevel(FrankLevel);
	ChangeWrightEvidence();
	ChangeMODOKUnderstanding(MODOKLOU);
	SetDormSpellLevels();
	EndlessInstallBoolUpdate();
	SetDeadpoolTeleport();
}

void EndlessXFactorUpdate()
{

	//Gets the needed values in memory. First these pointers.
	uintptr_t mysterytable = *(uintptr_t*)_addr(0x140d533e0);
	uintptr_t block2 = *(uintptr_t*)_addr(0x140d47e68);
	uintptr_t ptable = *(uintptr_t*)_addr(0x140d50e58);
	uintptr_t team1ptr = _addr((block2 + 0x350));
	uintptr_t team2ptr = _addr((block2 + 0x610));

	uintptr_t P1C1 = *(uintptr_t*)_addr(mysterytable + 0xAA0);
	uintptr_t P1C2 = *(uintptr_t*)_addr((mysterytable + 0xAA0) + (0x438 * 1));
	uintptr_t P1C3 = *(uintptr_t*)_addr((mysterytable + 0xAA0) + (0x438 * 2));
	uintptr_t P2C1 = *(uintptr_t*)_addr((mysterytable + 0xAA0) + (0x438 * 3));
	uintptr_t P2C2 = *(uintptr_t*)_addr((mysterytable + 0xAA0) + (0x438 * 4));
	uintptr_t P2C3 = *(uintptr_t*)_addr((mysterytable + 0xAA0) + (0x438 * 5));

	//Player 1.
	//uintptr_t P1C1Hurtboxes = (P1C1 + 0x4E10);
	//std::list<Collision> P1HurtboxList;
	//int HopPoint = P1C1Hurtboxes + 0x10;
	//int Thing = 5379632656;

	//Team1.
	*(float*)_addr(team1ptr + 0xC0) = 7000.0;

	//Team2.
	*(float*)_addr(team2ptr + 0xC0) = 7000.0;


}

void TurnOnHitboxDisplay()
{

	//First we get the hurtbox data. To be continued....

}

bool CheckTheMode()
{
	//Checks for Training Mode. If not in Training Mode, it returns false.
	uintptr_t strangetable = *(uintptr_t*)_addr(0x140d50e58);
	GameMode = *((int*)(strangetable + 0x34C));
	//printf("%d\n", GameMode);
	if (GameMode == 5)
	{
		return true;
	}
	else
	{
		return false;
	}
}



static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

UMVC3Menu::UMVC3Menu()
{
}

void UMVC3Menu::OnActivate()
{
	m_bIsActive ^= 1;
}

void UMVC3Menu::OnToggleFreeCamera()
{
	if (m_bIsActive)
		return;

	m_bFreeCam ^= 1;
}

void UMVC3Menu::OnToggleHUD()
{
	if (m_bIsActive)
		return;

	m_bDisableHUD ^= 1;
}

void GetTheImportantAddresses()
{

	//auto sigger = sigscan::get();

	if (pAddrSet == false)
	{
		pAddrSet = true;
		auto mysterytable = *(uintptr_t*)_addr(0x140d533e0);
		auto block2 = *(uintptr_t*)_addr(0x140d47e68);
		for (size_t i = 0; i < 6; i++)
		{
			scriptableFighters[i].fighter = *(Fighter**)(mysterytable + 0xaa0 + (0x438 * i));
		}
	}
	else {
		for (size_t i = 0; i < 6; i++)
		{
			if (scriptableFighters[i].name == nullptr) {
				ImGui::Text("p%d:%llx", i, scriptableFighters[i].fighter);
			}
			else {
				ImGui::Text("p%d:%llx %llx %s", i, scriptableFighters[i].fighter, scriptableFighters[i].tickPtr, scriptableFighters[i].name);
			}
			if (scriptableFighters[i].fighter->tagState == ETagState::Active || scriptableFighters[i].fighter->tagState == ETagState::Active2)
			{
				ImGui::Text("  X:%d", (int)scriptableFighters[i].fighter->vector.x);
				ImGui::SameLine();
				ImGui::Text("- Y:%d", (int)scriptableFighters[i].fighter->vector.y);
				ImGui::SameLine();
				ImGui::Text("- Vel:%d", (int)scriptableFighters[i].fighter->vector.xVelocity);

				ImGui::Text("  Anim:%x", scriptableFighters[i].fighter->anim);
				ImGui::Text("  Special:%x", scriptableFighters[i].fighter->specialState);

				ImGui::Text("  Grounded:%s", scriptableFighters[i].GetGroundedState());
				ImGui::Text("  TagState:%s", scriptableFighters[i].GetTagState());
			}
		}


		auto block2 = *(uintptr_t*)_addr(0x140d47e68);
		auto team1 = block2 + 0x350;
		auto team2 = block2 + 0x610;
		ImGui::Text("Team1:%llx ActiveFighter %x", team1, team1 + 0x48);
		ImGui::Text("Team2:%llx ActiveFighter %x", team2, team2 + 0x48);
	}

}

void UMVC3Menu::Draw()
{
	if (!m_bIsActive)
		return;

	auto block2 = *(uintptr_t*)_addr(0x140d47e68);
	auto val = *(unsigned char*)(block2 + 0x118);
	if (val != 0)
	{
		restarted = true;
		restartTimer = 0;
	}
	else if (restarted)
	{
		GetTheImportantAddresses();
		//hitboxes.clear();
		restartTimer += 1;
		if (restartTimer > 5)//0.166667 sec wait
		{
			RestartWithChanges();

			{
				auto ptr = ((uintptr_t)scriptableFighters[0].fighter) + 0x50;
				*((float*)ptr) = p1Pos;
			}
			{
				auto ptr = ((uintptr_t)scriptableFighters[3].fighter) + 0x50;
				*((float*)ptr) = p2Pos;
			}
			restarted = false;
			SetIndividualCharacterHealth();

		}


	}

	ImGui::GetIO().MouseDrawCursor = true;

	ImGui::Begin("UMVC3Hook by ermaccer", &m_bIsActive, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNav);

	//Start of my port of the Training Mode Tools mod I made in Spring. Still more to do.
	ImGui::Text("Training Tools");
	ImGui::Separator();
	if (ImGui::BeginTabBar("##tabs"))
	{

		if (DormSpellSet == true)
		{
			if (CheckTheMode() == true)
			{
				SetDormSpellLevels();
			}
		}

		if (EndlessInstalls == true)
		{
			if (CheckTheMode() == true)
			{
				EndlessInstallBoolUpdate();
			}
		}

		if (FreezeDeadpoolTPCounter == true)
		{
			if (CheckTheMode() == true)
			{
				SetDeadpoolTeleport();
			}
		}

		if (EndlessXFactor == true)
		{
			if (CheckTheMode() == true)
			{
				EndlessXFactorUpdate();
			}
		}

		//General training settings.
		if (ImGui::BeginTabItem("Extra Settings"))
		{
			ImGui::Text("Remember! These Parameters will only take\neffect when this window is open.");

			ImGui::Separator();

			if (ImGui::Button("Restart Training Mode"))
			{
				if (CheckTheMode() == true)
				{
					*(unsigned char*)(block2 + 0x118) = 3;
				}
			}

			ImGui::Text("Player 1 Position");
			if (ImGui::SliderFloat("P1 X Position", &p1Pos, -750.0f, 750.0f))
			{

			}

			ImGui::Text("Player 2 Position");
			if (ImGui::SliderFloat("P2 X Position", &p2Pos, -750.0f, 750.0f))
			{

			}

			ImGui::Text("Position Presets");
			if (ImGui::Button("Default"))
			{

				p1Pos = -125.0f;
				p2Pos = 125.0f;


			}
			ImGui::SameLine();
			if (ImGui::Button("Default P2 Side"))
			{

				p1Pos = 125.0f;
				p2Pos = -125.0f;


			}
			if (ImGui::Button("Left Corner"))
			{

				p1Pos = -750.0f;
				p2Pos = -800.0f;


			}
			ImGui::SameLine();
			if (ImGui::Button("Left Corner P2 Side"))
			{

				p1Pos = -800.0f;
				p2Pos = -750.0f;


			}

			if (ImGui::Button("Right Corner"))
			{

				p1Pos = 750.0f;
				p2Pos = 800.0f;


			}
			ImGui::SameLine();
			if (ImGui::Button("Right Corner P2 Side"))
			{

				p1Pos = 800.0f;
				p2Pos = 750.0f;


			}

			ImGui::Separator();

			if (ImGui::Checkbox("Endless X-Factor", &EndlessXFactor))
			{

			}

			ImGui::Separator();

			ImGui::Text("More features coming soon!");

			if (SaveStateSlot)
			{
				if (ImGui::SliderInt("Save State Slot", &SaveStateSlot, 1, 10))
				{

				}

				if (ImGui::Button("Save"))
				{

				}

				if (ImGui::Button("Load"))
				{

				}

			}


			ImGui::Separator();
			ImGui::Text("Incomplete Stuff");
			if (ImGui::Button("Turn On Hitboxes"))
			{
				if (CheckTheMode() == true)
				{
					/*
					//hitboxes.clear();
					//startedHitboxViewer = false;
					for (uint8_t* i = (uint8_t*)0x14200000; i < (uint8_t*)0x14900000; i++)
					{
						auto t = *(uintptr_t*)i;
						if (t == 0x140a6b000) {
							printf("hitboxes %llx\n", i);
							hitboxes.push_back((uintptr_t)i);
						}
						pAddrSet = true;
						auto mysterytable = *(uintptr_t*)_addr(0x140d533e0);
						auto block2 = *(uintptr_t*)_addr(0x140d47e68);
						for (size_t i = 0; i < 6; i++)
						{
							scriptableFighters[i].fighter = *(Fighter**)(mysterytable + 0xaa0 + (0x438 * i));
						}
					}
					*/
				}
			}
			if (ImGui::Button("Turn Off Hitboxes"))
			{
				/*
				if (CheckTheMode() == true)
				{
					//hitboxes.clear();
					//startedHitboxViewer = false;
					pAddrSet = false;
					for (size_t i = 0; i < 6; i++)
					{
						scriptableFighters[i].fighter = nullptr;
					}
				}
				*/
			}

			ImGui::Separator();

			ImGui::Text("V0.2 By Eternal Yoshi & HkHaan96");

			ImGui::EndTabItem();

		}

		//ImGui::EndTabItem();

		//Player and Status Related  Settings such as Health.
		if (ImGui::BeginTabItem("Status Options"))
		{
			ImGui::Text("Remember! These Parameters will only take\neffect when this window is open.");

			ImGui::Separator();

			//Toggle for using individual character health.
			if (ImGui::Checkbox("Set Individual Character Health(Applies during restarts).", &SetIndividualHP))
			{

			}

			//Toggle for Applying Health Settings to Red Health.
			if (ImGui::Checkbox("Also Apply To Red Health", &AlsoSetRedHealth))
			{

			}

			if (P1Char1Health)
			{
				ImGui::Text("Player 1 Character 1 Health");
				if (ImGui::SliderFloat("P1C1 HP %", &P1Char1Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			if (P1Char2Health)
			{
				ImGui::Text("Player 1 Character 2 Health");
				if (ImGui::SliderFloat("P1C2 HP %", &P1Char2Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			if (P1Char3Health)
			{
				ImGui::Text("Player 1 Character 3 Health");
				if (ImGui::SliderFloat("P1C3 HP %", &P1Char3Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			if (P2Char1Health)
			{
				ImGui::Text("Player 2 Character 1 Health");
				if (ImGui::SliderFloat("P2C1 HP %", &P2Char1Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			if (P2Char2Health)
			{
				ImGui::Text("Player 2 Character 2 Health");
				if (ImGui::SliderFloat("P2C2 HP %", &P2Char2Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			if (P2Char3Health)
			{
				ImGui::Text("Player 2 Character 3 Health");
				if (ImGui::SliderFloat("P2C3 HP %", &P2Char3Health, 0.01f, 1.0f))
				{
					if (CheckTheMode() == true)
					{

					}
				}
			}

			ImGui::Separator();

			ImGui::Text("V0.2 By Eternal Yoshi, thanks to HkHaan96 for help");

			ImGui::EndTabItem();

		}

		//ImGui::EndTabItem();

		//Character specific parameters like Frank West levels.
		if (ImGui::BeginTabItem("Character Settings"))
		{

			ImGui::Text("Remember! These Parameters will only take\neffect when this window is open.");

			ImGui::Separator();

			if (FrankLevel)
			{
				ImGui::Text("Frank West's Level");
				if (ImGui::SliderInt("Lv", &FrankLevel, 1, 5))
				{
					if (CheckTheMode() == true)
					{
						ChangeFrankLevel(FrankLevel);
					}
				}
			}

			ImGui::Separator();

			ImGui::Text("Mr. Wright's Evidence");
			if (WrightEvidenceA || WrightEvidenceB || WrightEvidenceC)
			{
				ImGui::Text("Evidence Slot 1");

				if (ImGui::BeginCombo("##WrightEvidencecombo", selected_item))
				{
					if (CheckTheMode() == true)
					{
						for (int n = 0; n < IM_ARRAYSIZE(EvidenceTypes); n++)
						{
							bool is_selected = (selected_item == EvidenceTypes[n]);
							if (ImGui::Selectable(EvidenceTypes[n], is_selected))
							{
								selected_item = EvidenceTypes[n];
								WrightEvidenceA = EvidenceNameToID(selected_item);
								ChangeWrightEvidence();

							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::Text("Evidence Slot 2");

				if (ImGui::BeginCombo("##WrightEvidencecombo2", selected_itemTwo))
				{
					if (CheckTheMode() == true)
					{
						for (int n = 0; n < IM_ARRAYSIZE(EvidenceTypesTwo); n++)
						{
							bool is_selected = (selected_itemTwo == EvidenceTypesTwo[n]);
							if (ImGui::Selectable(EvidenceTypesTwo[n], is_selected))
							{
								selected_itemTwo = EvidenceTypesTwo[n];
								WrightEvidenceB = EvidenceNameToID(selected_itemTwo);
								ChangeWrightEvidence();

							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::Text("Evidence Slot 3");

				if (ImGui::BeginCombo("##WrightEvidencecombo3", selected_itemThree))
				{
					if (CheckTheMode() == true)
					{
						for (int n = 0; n < IM_ARRAYSIZE(EvidenceTypesThree); n++)
						{
							bool is_selected = (selected_itemThree == EvidenceTypesThree[n]);
							if (ImGui::Selectable(EvidenceTypesThree[n], is_selected))
							{
								selected_itemThree = EvidenceTypesThree[n];
								WrightEvidenceC = EvidenceNameToID(selected_itemThree);
								ChangeWrightEvidence();

							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

			}

			ImGui::Separator();

			if (MODOKLOU)
			{
				ImGui::Text("MODOK Level of Understanding");
				if (ImGui::SliderInt("LOU", &MODOKLOU, 1, 10))
				{
					if (CheckTheMode() == true)
					{
						ChangeMODOKUnderstanding(MODOKLOU);
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("Dormammu's Spell Charges");

			ImGui::Text("Power Of The Destructor/Red");
			if (ImGui::SliderInt("DRed", &DormRed, 0, 3))
			{
				if (CheckTheMode() == true)
				{
					DormSpellSet = true;
					SetDormSpellLevels();
				}
			}

			ImGui::Text("Power Of The Creator/Blue");
			if (ImGui::SliderInt("DBlue", &DormBlue, 0, 3))
			{
				if (CheckTheMode() == true)
				{
					DormSpellSet = true;
					SetDormSpellLevels();
				}
			}

			ImGui::Separator();
			ImGui::Text("Endless Install Toggle");
			if (ImGui::Checkbox("EndlessInstallToggle", &EndlessInstalls))
			{
				if (CheckTheMode() == true)
				{
					EndlessInstallBoolUpdate();
				}
			}

			ImGui::Separator();

			ImGui::Text("Deadpool Teleport");
			if (ImGui::SliderInt("TeleportCount", &DeadpoolTeleportCount, 0, 2))
			{
				if (CheckTheMode() == true)
				{
					SetDeadpoolTeleport();
				}
			}

			if (ImGui::Checkbox("LockTeleportCount", &FreezeDeadpoolTPCounter))
			{
				if (CheckTheMode() == true)
				{
				}
			}

			/*
			ImGui::Text("Player 1 Character 1 Death Toggle");
			if (ImGui::Checkbox("IsP1C1Dead", &P1Character1Dead))
			{

			}

			ImGui::Text("Player 1 Character 2 Death Toggle");
			if (ImGui::Checkbox("IsP1C2Dead", &P1Character2Dead))
			{

			}

			ImGui::Text("Player 1 Character 3 Death Toggle");

			if (ImGui::Checkbox("IsP1C3Dead", &P1Character3Dead))
			{

			}

			ImGui::Text("Player 2 Character 1 Death Toggle");

			if (ImGui::Checkbox("IsP2C31Dead", &P2Character1Dead))
			{

			}

			ImGui::Text("Player 2 Character 2 Death Toggle");
			if (ImGui::Checkbox("IsP2C2Dead", &P2Character2Dead))
			{

			}

			ImGui::Text("Player 2 Character 3 Death Toggle");
			if (ImGui::Checkbox("IsP2C3Dead", &P2Character3Dead))
			{

			}
			*/


			ImGui::Separator();

			ImGui::Text("Dark Phoenix/Turnabout Mode Toggle\n(Coming whenever I can figure it out!)");

			ImGui::Separator();

			ImGui::Text("V0.2 By Eternal Yoshi, thanks to HkHaan96 for help");

			ImGui::EndTabItem();

		}

		ImGui::EndTabBar();
	}
	ImGui::End();

	//Original Code Here.
	/*
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			m_bSubmenuActive[SM_Settings] = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::BeginMenu("About"))
			{
				ImGui::MenuItem("Version: " UMVC3HOOK_VERSION);
				ImGui::MenuItem("Date: " __DATE__);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

	}
	ImGui::EndMenuBar();

	if (ImGui::BeginTabBar("##tabs"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			DrawCameraTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Misc."))
		{
			DrawMiscTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	*/

	if (m_bSubmenuActive[SM_Settings])
		DrawSettings();

}

void UMVC3Menu::Process()
{

	Memory::VP::Patch<char>(_addr(0x140532D80), m_bFreeCam ? 0xC3 : 0x48);

	Memory::VP::Patch<char>(_addr(0x140326A70), (m_bDisableHUD || m_bDisableHUDTotal) ? 0xC3 : 0x48);
	Memory::VP::Patch<char>(_addr(0x140324240), (m_bDisableHUD || m_bDisableHUDTotal) ? 0xC3 : 0x48);
	Memory::VP::Patch<char>(_addr(0x1404537B0), m_bDisableHUDTotal ? 0xC3 : 0x48);

	Memory::VP::Patch<char>(_addr(0x14077EA40), m_bDisableEffects ? 0xC3 : 0x48);
	Memory::VP::Patch<char>(_addr(0x14077D330), m_bDisableEffects ? 0xC3 : 0x48);

	UpdateControls();
	UpdateFreecam();
}

void UMVC3Menu::UpdateControls()
{

}

void UMVC3Menu::UpdateFreecam()
{

	UMVC3MainCamera* cam = GetMainCamera();

	if (!cam)
		return;

	Vector fwd = cam->mat.forward;
	Vector up = cam->mat.up;
	Vector right = cam->mat.right;

	static XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	static XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	static XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	float delta = GUIImplementationDX9::GetDeltaTime();

	if (m_bFreeCam)
	{
		// forward

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyXPlus))
			TheMenu->camMat.pos.X += TheMenu->m_fFreeCameraSpeed * delta * 1;


		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyXMinus))
			TheMenu->camMat.pos.X += TheMenu->m_fFreeCameraSpeed * delta * -1;

		// right

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyYPlus))
			TheMenu->camMat.pos.Y += TheMenu->m_fFreeCameraSpeed * delta * 1;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyYMinus))
			TheMenu->camMat.pos.Y += TheMenu->m_fFreeCameraSpeed * delta * -1;

		// up

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyZPlus))
			TheMenu->camMat.pos.Z += TheMenu->m_fFreeCameraSpeed * delta * 1;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyZMinus))
			TheMenu->camMat.pos.Z += TheMenu->m_fFreeCameraSpeed * delta * -1;


		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyPitchPlus))
			camRot.X += TheMenu->m_fFreeCameraRotationSpeed * delta;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyPitchMinus))
			camRot.X -= TheMenu->m_fFreeCameraRotationSpeed * delta;

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyYawPlus))
			camRot.Y += TheMenu->m_fFreeCameraRotationSpeed * delta;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyYawMinus))
			camRot.Y -= TheMenu->m_fFreeCameraRotationSpeed * delta;

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyRollPlus))
			camRot.Z += TheMenu->m_fFreeCameraRotationSpeed * delta;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyRollMinus))
			camRot.Z -= TheMenu->m_fFreeCameraRotationSpeed * delta;
	}

	if (m_bFreeCam || m_bCustomCameraFOV)
	{
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFOVMinus))
			camFov -= 2.5f * delta;
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFOVPlus))
			camFov += 2.5f * delta;
	}


	if (m_bFreeCam)
	{
		XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(camRot.X, camRot.Y, camRot.Z);

		XMVECTOR camLookAt = XMVector3TransformCoord(defaultForward, rotMat);
		XMVECTOR camUp = XMVector3TransformCoord(defaultUp, rotMat);
		XMVECTOR camRight = XMVector3TransformCoord(defaultRight, rotMat);
		XMVector3Normalize(camLookAt);
		XMVector3Normalize(camUp);
		XMVector3Normalize(camRight);
		XMFLOAT3 cL, cU, cR;
		XMStoreFloat3(&cL, camLookAt);
		XMStoreFloat3(&cU, camUp);
		XMStoreFloat3(&cR, camRight);

		camMat.forward = { cL.x, cL.y, cL.z };
		camMat.up = { cU.x, cU.y, cU.z };
		camMat.right = { cR.x, cR.y, cR.z };
	}
	if (m_bFreeCam)
	{
		cam->mat.pos = cam->mat2.pos = camMat.pos;
		cam->mat.right = cam->mat2.right = camMat.right;
		cam->mat.forward = cam->mat2.forward = camMat.forward;
		cam->mat.up = cam->mat2.up = camMat.up;

	}
	else
	{
		camMat.pos = cam->mat.pos;
	}

	if (TheCamera)
	{
		if (m_bCustomCameraFOV)
			TheCamera->mFOV = camFov;
		else
			camFov = TheCamera->mFOV;
	}
}

void UMVC3Menu::DrawCameraTab()
{
	UMVC3MainCamera* cam = GetMainCamera();

	if (!cam)
		return;
#ifdef _DEBUG
	ImGui::Text("%p", cam);


	ImGui::InputFloat4("Pr", &cam->mat.M[0][0]);
	ImGui::InputFloat4("Pu", &cam->mat.M[1][0]);
	ImGui::InputFloat4("Pf", &cam->mat.M[2][0]);
	ImGui::InputFloat4("Pp", &cam->mat.M[3][0]);
	ImGui::Separator();
	ImGui::Separator();
	ImGui::InputFloat4("Sr", &cam->mat2.M[0][0]);
	ImGui::InputFloat4("Su", &cam->mat2.M[1][0]);
	ImGui::InputFloat4("Sf", &cam->mat2.M[2][0]);
	ImGui::InputFloat4("Sp", &cam->mat2.M[3][0]);


	ImGui::Separator();
#endif

	ImGui::InputFloat3("X | Y | Z", &camMat.pos.X);
	ImGui::InputFloat3("Pitch | Yaw | Roll", &camRot.X);


	ImGui::Separator();
	ImGui::Checkbox("Set FOV", &m_bCustomCameraFOV);
	ImGui::SameLine();
	ShowHelpMarker("FOV won't update while free camera is active!");
	ImGui::InputFloat("FOV", &camFov);


	ImGui::Separator();
	ImGui::Checkbox("Enable Free Camera", &m_bFreeCam);
	ImGui::SameLine(); ShowHelpMarker("Allows to move camera with keyboard. Keys can be changed in the Settings menu or in the .ini file. Hotkey can be assigned in Settings menu.");

	if (m_bFreeCam)
	{
		ImGui::InputFloat("Free Camera Speed", &m_fFreeCameraSpeed);
		ImGui::InputFloat("Free Camera Rotation Speed", &m_fFreeCameraRotationSpeed);

	}

}

void UMVC3Menu::DrawMiscTab()
{
	ImGui::Checkbox("Disable Game HUD", &m_bDisableHUD);
	ImGui::SameLine();
	ShowHelpMarker(eKeyboardMan::KeyToString(SettingsMgr->iToggleHUDKey));
	ImGui::Checkbox("Disable All HUD", &m_bDisableHUDTotal);
	ImGui::Checkbox("Disable Effects", &m_bDisableEffects);
}

void UMVC3Menu::DrawSettings()
{
	ImGui::SetNextWindowPos({ ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f }, ImGuiCond_Once, { 0.5f, 0.5f });
	ImGui::SetNextWindowSize({ 35 * ImGui::GetFontSize(), 35 * ImGui::GetFontSize() }, ImGuiCond_Once);
	ImGui::Begin("Settings", &m_bSubmenuActive[SM_Settings]);

	static int settingID = 0;
	static const char* settingNames[] = {
		"Menu",
		"INI",
		"Keys",
	};

	enum eSettings {
		MENU,
		INI,
		KEYS,
	};

	ImGui::BeginChild("##settings", { 12 * ImGui::GetFontSize(), 0 }, true);

	for (int n = 0; n < IM_ARRAYSIZE(settingNames); n++)
	{
		bool is_selected = (settingID == n);
		if (ImGui::Selectable(settingNames[n], is_selected))
			settingID = n;
		if (is_selected)
			ImGui::SetItemDefaultFocus();
	}

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("##content", { 0, -ImGui::GetFrameHeightWithSpacing() });

	switch (settingID)
	{
	case MENU:
		ImGui::TextWrapped("All user settings are saved to umvc3hook_user.ini.");
		ImGui::Text("Menu Scale");
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::InputFloat("##", &SettingsMgr->fMenuScale);
		ImGui::PopItemWidth();
		break;
	case INI:
		ImGui::TextWrapped("These settings control UMVC3Hook.ini options. Any changes require game restart to take effect.");
		ImGui::Checkbox("Debug Console", &SettingsMgr->bEnableConsoleWindow);
		break;
	case KEYS:
		if (m_bPressingKey)
			ImGui::TextColored(ImVec4(0.f, 1.f, 0.3f, 1.f), "Press a key!");

		if (ImGui::Button("Reset Keys", { -FLT_MIN, 0 }))
		{
			SettingsMgr->ResetKeys();
			Notifications->SetNotificationTime(5500);
			Notifications->PushNotification("Keys reset! Remember to save.");

		}
		ImGui::Separator();
		ImGui::LabelText("##", "Core");
		ImGui::Separator();
		KeyBind(&SettingsMgr->iHookMenuOpenKey, "Open/Close Menu", "menu");
		ImGui::Separator();
		ImGui::LabelText("##", "Camera");
		ImGui::Separator();

		KeyBind(&SettingsMgr->iFreeCameraKeyFOVPlus, "FOV+", "fov_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyFOVMinus, "FOV-", "fov_minus");

		KeyBind(&SettingsMgr->iFreeCameraKeyYawPlus, "Yaw+", "ya_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyYawMinus, "Yaw-", "ya_minus");
		KeyBind(&SettingsMgr->iFreeCameraKeyPitchPlus, "Pitch+", "pi_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyPitchMinus, "Pitch-", "pi_minus");
		KeyBind(&SettingsMgr->iFreeCameraKeyRollPlus, "Roll+", "r_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyRollMinus, "Roll-", "r_minus");

		KeyBind(&SettingsMgr->iFreeCameraKeyXPlus, "X+", "x_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyXMinus, "X-", "x_minus");
		KeyBind(&SettingsMgr->iFreeCameraKeyYPlus, "Y+", "y_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyYMinus, "Y-", "y_minus");
		KeyBind(&SettingsMgr->iFreeCameraKeyZPlus, "Z+", "z_plus");
		KeyBind(&SettingsMgr->iFreeCameraKeyZMinus, "Z-", "z_minus");


		ImGui::Separator();
		ImGui::LabelText("##", "Misc");
		ImGui::Separator();
		KeyBind(&SettingsMgr->iToggleFreeCameraKey, "Toggle Free Camera", "fcam");
		KeyBind(&SettingsMgr->iToggleHUDKey, "Toggle HUD", "thud");
		ImGui::Separator();

		if (m_bPressingKey)
		{
			eVKKeyCode result = eKeyboardMan::GetLastKey();

			if (result >= VK_BACKSPACE && result < VK_KEY_NONE)
			{
				*m_pCurrentVarToChange = result;
				m_bPressingKey = false;
			}

		}
		break;
	default:
		break;
	}

	if (ImGui::Button("Save", { -FLT_MIN, 0 }))
	{
		Notifications->SetNotificationTime(5500);
		Notifications->PushNotification("Settings saved to UMVC3Hook.ini and umvc3hook_user.ini!");
		GUIImplementationDX9::RequestFontReload();
		SettingsMgr->SaveSettings();
	}

	ImGui::EndChild();

	ImGui::End();
}

void UMVC3Menu::DrawKeyBind(char* name, int* var)
{
	ImGui::SameLine();

	static char butName[256] = {};
	sprintf(butName, "%s##key%s", eKeyboardMan::KeyToString(*var), name);
	if (ImGui::Button(butName))
	{
		m_bPressingKey = true;
		m_pCurrentVarToChange = var;
	}
}

void UMVC3Menu::KeyBind(int* var, char* bindName, char* name)
{
	ImGui::LabelText("##", bindName);
	DrawKeyBind(name, var);
}

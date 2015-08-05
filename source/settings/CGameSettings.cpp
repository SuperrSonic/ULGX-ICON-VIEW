/****************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CSettings.h"
#include "CGameSettings.h"
#include "FileOperations/fileops.h"
#include "svnrev.h"

#define VALID_CONFIG_REV	1031

CGameSettings GameSettings;

CGameSettings::CGameSettings()
{
	SetDefault(DefaultConfig);
}

CGameSettings::~CGameSettings()
{
}

GameCFG * CGameSettings::GetGameCFG(const char * id)
{
	if(!id)
	{
		DefaultConfig.id[0] = '\0';
		return &DefaultConfig;
	}

	for(u32 i = 0; i < GameList.size(); ++i)
	{
		if(strncasecmp(id, GameList[i].id, 6) == 0)
			return &GameList[i];
	}

	memcpy(DefaultConfig.id, id, 6);

	return &DefaultConfig;
}

bool CGameSettings::AddGame(const GameCFG & NewGame)
{
	for(u32 i = 0; i < GameList.size(); ++i)
	{
		if(strncasecmp(NewGame.id, GameList[i].id, 6) == 0)
		{
			GameList[i] = NewGame;
			return true;
		}
	}

	GameList.push_back(NewGame);

	return true;
}

bool CGameSettings::RemoveAll()
{
	GameList.clear();
	std::vector<GameCFG>().swap(GameList);

	return Save();
}

bool CGameSettings::Remove(const char * id)
{
	if(!id)
		return false;

	for(u32 i = 0; i < GameList.size(); ++i)
	{
		if(strncasecmp(id, GameList[i].id, 6) == 0)
		{
			GameList.erase(GameList.begin()+i);
			break;
		}
	}

	return true;
}

bool CGameSettings::Load(const char * path)
{
	char filepath[300];
	snprintf(filepath, sizeof(filepath), "%sGXGameSettings.cfg", path);

	ConfigPath = filepath;

	FILE *file = fopen(filepath, "r");
	if (!file) return false;

	if(!ValidVersion(file))
	{
		fclose(file);
		return false;
	}

	const int lineSize = 20*1024;

	char *line = new (std::nothrow) char[lineSize];
	if(!line) {
		fclose(file);
		return false;
	}

	while (fgets(line, lineSize, file))
	{
		if (line[0] == '#')
			continue;

		this->ParseLine(line);
	}

	delete [] line;

	fclose(file);

	return true;
}

bool CGameSettings::ValidVersion(FILE * file)
{
	if(!file) return false;

	char line[255];
	int revision = 0;

	while (fgets(line, sizeof(line), file))
	{
		const char * ptr = strcasestr(line, "USB Loader GX R");
		if(ptr)
		{
			ptr += strlen("USB Loader GX R");
			revision = atoi(ptr);
			break;
		}
	}

	rewind(file);

	return revision >= VALID_CONFIG_REV;
}

bool CGameSettings::Save()
{
	char filepath[300];
	strlcpy(filepath, ConfigPath.c_str(), sizeof(filepath));

	char * ptr = strrchr(filepath, '/');
	if(ptr)
		ptr[0] = 0;

	CreateSubfolder(filepath);

	FILE * f = fopen(ConfigPath.c_str(), "w");
	if (!f) return false;

	fprintf(f, "# USB Loader GX R%s - Individual game settings file\n", GetRev());
	fprintf(f, "# note: this file is automatically generated\n");
	fprintf(f, "# Num Games: %d\n", GameList.size());
	for (u32 i = 0; i < GameList.size(); ++i)
	{
		fprintf(f, "game:%s = ", GameList[i].id);
		fprintf(f, "video:%d; ", GameList[i].video);
		fprintf(f, "videoPatchDol:%d; ", GameList[i].videoPatchDol);
		fprintf(f, "aspectratio:%d; ", GameList[i].aspectratio);
		fprintf(f, "language:%d; ", GameList[i].language);
		fprintf(f, "ocarina:%d; ", GameList[i].ocarina);
		fprintf(f, "vipatch:%d; ", GameList[i].vipatch);
		fprintf(f, "ios:%d; ", GameList[i].ios);
		fprintf(f, "parentalcontrol:%d; ", GameList[i].parentalcontrol);
		fprintf(f, "iosreloadblock:%d; ", GameList[i].iosreloadblock);
		fprintf(f, "patchcountrystrings:%d; ", GameList[i].patchcountrystrings);
		fprintf(f, "loadalternatedol:%d; ", GameList[i].loadalternatedol);
		fprintf(f, "alternatedolstart:%d; ", GameList[i].alternatedolstart);
		fprintf(f, "alternatedolname:%s; ", GameList[i].alternatedolname.c_str());
		fprintf(f, "returnTo:%d; ", GameList[i].returnTo);
		fprintf(f, "sneekVideoPatch:%d; ", GameList[i].sneekVideoPatch);
		fprintf(f, "NandEmuMode:%d; ", GameList[i].NandEmuMode);
		fprintf(f, "NandEmuPath:%s; ", GameList[i].NandEmuPath.c_str());
		fprintf(f, "Hooktype:%d; ", GameList[i].Hooktype);
		fprintf(f, "WiirdDebugger:%d; ", GameList[i].WiirdDebugger);
		fprintf(f, "GameCubeMode:%d; ", GameList[i].GameCubeMode);
		fprintf(f, "DMLVideo:%d; ", GameList[i].DMLVideo);
		fprintf(f, "DMLProgPatch:%d; ", GameList[i].DMLProgPatch);
		fprintf(f, "DMLNMM:%d; ", GameList[i].DMLNMM);
		fprintf(f, "DMLActivityLED:%d; ", GameList[i].DMLActivityLED);
		fprintf(f, "DMLPADHOOK:%d; ", GameList[i].DMLPADHOOK);
		fprintf(f, "DMLNoDisc2:%d; ", GameList[i].DMLNoDisc2);
		fprintf(f, "DMLWidescreen:%d; ", GameList[i].DMLWidescreen);
		fprintf(f, "DMLScreenshot:%d; ", GameList[i].DMLScreenshot);
		fprintf(f, "DMLJPNPatch:%d; ", GameList[i].DMLJPNPatch);
		fprintf(f, "DMLDebug:%d; ", GameList[i].DMLDebug);
		fprintf(f, "NINDeflicker:%d; ", GameList[i].NINDeflicker);
		fprintf(f, "NINMCEmulation:%d; ", GameList[i].NINMCEmulation);
		fprintf(f, "NINMCSize:%d; ", GameList[i].NINMCSize);
		fprintf(f, "NINUSBHID:%d; ", GameList[i].NINUSBHID);
		fprintf(f, "NINMaxPads:%d; ", GameList[i].NINMaxPads);
		fprintf(f, "NINNativeSI:%d; ", GameList[i].NINNativeSI);
		fprintf(f, "NINWiiUWide:%d; ", GameList[i].NINWiiUWide);
		fprintf(f, "NINOSReport:%d; ", GameList[i].NINOSReport);
		fprintf(f, "NINLED:%d; ", GameList[i].NINLED);
		fprintf(f, "NINLog:%d; ", GameList[i].NINLog);
		fprintf(f, "NINLoaderPath:%s; ", GameList[i].NINLoaderPath.c_str());
		fprintf(f, "DEVOMCEmulation:%d; ", GameList[i].DEVOMCEmulation);
		fprintf(f, "DEVOWidescreen:%d; ", GameList[i].DEVOWidescreen);
		fprintf(f, "DEVOActivityLED:%d; ", GameList[i].DEVOActivityLED);
		fprintf(f, "DEVOFZeroAX:%d; ", GameList[i].DEVOFZeroAX);
		fprintf(f, "DEVOTimerFix:%d; ", GameList[i].DEVOTimerFix);
		fprintf(f, "DEVODButtons:%d; ", GameList[i].DEVODButtons);
		fprintf(f, "DEVOCropOverscan:%d; ", GameList[i].DEVOCropOverscan);
		fprintf(f, "DEVODiscDelay:%d; ", GameList[i].DEVODiscDelay);
		fprintf(f, "PrivateServer:%d; ", GameList[i].PrivateServer);
		fprintf(f, "Locked:%d;\n", GameList[i].Locked);
	}
	fprintf(f, "# END\n");
	fclose(f);

	return true;
}

bool CGameSettings::SetSetting(GameCFG & game, const char *name, const char *value)
{
	if (strcmp(name, "video") == 0)
	{
		game.video = atoi(value);
		return true;
	}
	if (strcmp(name, "videoPatchDol") == 0)
	{
		game.videoPatchDol = atoi(value);
		return true;
	}
	else if(strcmp(name, "aspectratio") == 0)
	{
		game.aspectratio = atoi(value);
		return true;
	}
	else if(strcmp(name, "language") == 0)
	{
		game.language = atoi(value);
		return true;
	}
	else if(strcmp(name, "ocarina") == 0)
	{
		game.ocarina = atoi(value);
		return true;
	}
	else if(strcmp(name, "vipatch") == 0)
	{
		game.vipatch = atoi(value);
		return true;
	}
	else if(strcmp(name, "ios") == 0)
	{
		game.ios = atoi(value);
		return true;
	}
	else if(strcmp(name, "parentalcontrol") == 0)
	{
		game.parentalcontrol = atoi(value);
		return true;
	}
	else if(strcmp(name, "iosreloadblock") == 0)
	{
		game.iosreloadblock = atoi(value);
		return true;
	}
	else if(strcmp(name, "loadalternatedol") == 0)
	{
		game.loadalternatedol = atoi(value);
		return true;
	}
	else if(strcmp(name, "alternatedolstart") == 0)
	{
		game.alternatedolstart = atoi(value);
		return true;
	}
	else if(strcmp(name, "patchcountrystrings") == 0)
	{
		game.patchcountrystrings = atoi(value);
		return true;
	}
	else if(strcmp(name, "alternatedolname") == 0)
	{
		game.alternatedolname = value;
		return true;
	}
	else if(strcmp(name, "returnTo") == 0)
	{
		game.returnTo = atoi(value);
		return true;
	}
	else if(strcmp(name, "sneekVideoPatch") == 0)
	{
		game.sneekVideoPatch = atoi(value);
		return true;
	}
	else if(strcmp(name, "NandEmuMode") == 0)
	{
		game.NandEmuMode = atoi(value);
		return true;
	}
	else if(strcmp(name, "NandEmuPath") == 0)
	{
		game.NandEmuPath = value;
		return true;
	}
	else if(strcmp(name, "Hooktype") == 0)
	{
		game.Hooktype = atoi(value);
		return true;
	}
	else if(strcmp(name, "WiirdDebugger") == 0)
	{
		game.WiirdDebugger = atoi(value);
		return true;
	}
	else if(strcmp(name, "Locked") == 0)
	{
		game.Locked = atoi(value);
		return true;
	}
	else if(strcmp(name, "GameCubeMode") == 0)
	{
		game.GameCubeMode = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLVideo") == 0)
	{
		game.DMLVideo = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLProgPatch") == 0)
	{
		game.DMLProgPatch = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLNMM") == 0)
	{
		game.DMLNMM = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLActivityLED") == 0)
	{
		game.DMLActivityLED = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLPADHOOK") == 0)
	{
		game.DMLPADHOOK = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLNoDisc2") == 0)
	{
		game.DMLNoDisc2 = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLWidescreen") == 0)
	{
		game.DMLWidescreen = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLScreenshot") == 0)
	{
		game.DMLScreenshot = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLJPNPatch") == 0)
	{
		game.DMLJPNPatch = atoi(value);
		return true;
	}
	else if(strcmp(name, "DMLDebug") == 0)
	{
		game.DMLDebug = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINDeflicker") == 0)
	{
		game.NINDeflicker = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINMCEmulation") == 0)
	{
		game.NINMCEmulation = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINMCSize") == 0)
	{
		game.NINMCSize = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINUSBHID") == 0)
	{
		game.NINUSBHID = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINMaxPads") == 0)
	{
		game.NINMaxPads = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINNativeSI") == 0)
	{
		game.NINNativeSI = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINWiiUWide") == 0)
	{
		game.NINWiiUWide = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINOSReport") == 0)
	{
		game.NINOSReport = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINLED") == 0)
	{
		game.NINLED = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINLog") == 0)
	{
		game.NINLog = atoi(value);
		return true;
	}
	else if(strcmp(name, "NINLoaderPath") == 0)
	{
		game.NINLoaderPath = value;
		return true;
	}
	else if(strcmp(name, "DEVOMCEmulation") == 0)
	{
		game.DEVOMCEmulation = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVOWidescreen") == 0)
	{
		game.DEVOWidescreen = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVOActivityLED") == 0)
	{
		game.DEVOActivityLED = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVOFZeroAX") == 0)
	{
		game.DEVOFZeroAX = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVOTimerFix") == 0)
	{
		game.DEVOTimerFix = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVODButtons") == 0)
	{
		game.DEVODButtons = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVOCropOverscan") == 0)
	{
		game.DEVOCropOverscan = atoi(value);
		return true;
	}
	else if(strcmp(name, "DEVODiscDelay") == 0)
	{
		game.DEVODiscDelay = atoi(value);
		return true;
	}
	else if(strcmp(name, "PrivateServer") == 0)
	{
		game.PrivateServer = atoi(value);
		return true;
	}

	return false;
}

bool CGameSettings::ReadGameID(const char * src, char * GameID, int size)
{
	if(strncasecmp(src, "game:", 5) != 0)
		return false;

	char * ptr = strchr(src, ':');
	if(!ptr)
		return false;

	ptr++;

	int i = 0;

	for(i = 0; i < size; i++, ptr++)
	{
		if(*ptr == ' ' || *ptr == '\0')
			break;

		GameID[i] = *ptr;
	}

	GameID[i] = 0;

	return true;
}

void CGameSettings::ParseLine(char *line)
{
	char GameID[8];

	if(!ReadGameID(line, GameID, 6))
		return;

	if(strlen(GameID) != 6 && strlen(GameID) != 4)
		return;

	GameCFG NewCFG;
	SetDefault(NewCFG);

	strcpy(NewCFG.id, GameID);

	char * LinePtr = strchr(line, '=');

	while(LinePtr != NULL)
	{
		LinePtr++;

		char * eq = strchr(LinePtr, ':');

		if (!eq) break;

		std::string name, value;

		this->TrimLine(name, LinePtr, ':');
		this->TrimLine(value, eq + 1, ';');

		SetSetting(NewCFG, name.c_str(), value.c_str());

		LinePtr = strchr(LinePtr, ';');
	}

	AddGame(NewCFG);
}

void CGameSettings::TrimLine(std::string &dest, const char *src, char stopChar)
{
	if(!src)
		return;

	while (*src == ' ')
		src++;

	while(*src != 0)
	{
		if(*src == stopChar || *src == '\n' || *src == '\r')
			break;

		dest.push_back(*src);
		src++;
	}
}

int CGameSettings::GetPartenalPEGI(int parental)
{
	switch(parental)
	{
		case 1: return 7;
		case 2: return 12;
		case 3: return 16;
		case 4: return 18;
		default: return -1;
	}
}

void CGameSettings::SetDefault(GameCFG &game)
{
	memset(game.id, 0, sizeof(game.id));
	game.video = INHERIT;
	game.videoPatchDol = INHERIT;
	game.aspectratio = INHERIT;
	game.language = INHERIT;
	game.ocarina = INHERIT;
	game.vipatch = INHERIT;
	game.ios = INHERIT;
	game.parentalcontrol = PARENTAL_LVL_EVERYONE;
	game.patchcountrystrings = INHERIT;
	game.loadalternatedol = ALT_DOL_DEFAULT;
	game.alternatedolstart = 0;
	game.iosreloadblock = INHERIT;
	game.alternatedolname.clear();
	game.returnTo = ON;
	game.sneekVideoPatch = INHERIT;
	game.NandEmuMode = INHERIT;
	game.NandEmuPath.clear();
	game.Hooktype = INHERIT;
	game.WiirdDebugger = INHERIT;
	game.GameCubeMode = INHERIT;
	game.DMLVideo = INHERIT;
	game.DMLProgPatch = INHERIT;
	game.DMLNMM = INHERIT;
	game.DMLActivityLED = INHERIT;
	game.DMLPADHOOK = INHERIT;
	game.DMLNoDisc2 = INHERIT;
	game.DMLWidescreen = INHERIT;
	game.DMLScreenshot = INHERIT;
	game.DMLJPNPatch = INHERIT;
	game.DMLDebug = INHERIT;
	game.NINDeflicker = INHERIT;
	game.NINMCEmulation = INHERIT;
	game.NINMCSize = INHERIT;
	game.NINUSBHID = INHERIT;
	game.NINMaxPads = INHERIT;
	game.NINNativeSI = INHERIT;
	game.NINWiiUWide = INHERIT;
	game.NINOSReport = INHERIT;
	game.NINLED = INHERIT;
	game.NINLog = INHERIT;
	game.NINLoaderPath.clear();
	game.DEVOMCEmulation = INHERIT;
	game.DEVOWidescreen = INHERIT;
	game.DEVOActivityLED = INHERIT;
	game.DEVOFZeroAX = INHERIT;
	game.DEVOTimerFix = INHERIT;
	game.DEVODButtons = INHERIT;
	game.DEVOCropOverscan = INHERIT;
	game.DEVODiscDelay = INHERIT;
	game.PrivateServer = INHERIT;
	game.Locked = OFF;
}

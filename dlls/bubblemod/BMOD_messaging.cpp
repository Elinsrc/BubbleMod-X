// ---------------------------------------------------------------
// BubbleMod
// 
// AUTHOR
//        Tyler Lund <halflife@bubblemod.org>
//
// LICENSE                                                            
//                                                                    
//        Permission is granted to anyone to use this  software  for  
//        any purpose on any computer system, and to redistribute it  
//        in any way, subject to the following restrictions:          
//                                                                    
//        1. The author is not responsible for the  consequences  of  
//           use of this software, no matter how awful, even if they  
//           arise from defects in it.                                
//        2. The origin of this software must not be misrepresented,  
//           either by explicit claim or by omission.                 
//        3. Altered  versions  must  be plainly marked as such, and  
//           must  not  be  misrepresented  (by  explicit  claim  or  
//           omission) as being the original software.                
//        3a. It would be nice if I got  a  copy  of  your  improved  
//            version  sent to halflife@bubblemod.org. 
//        4. This notice must not be removed or altered.              
//                                                                    
// ---------------------------------------------------------------

#include "BMOD_messaging.h"
#include "game.h"
#include "BMOD_constants.h"
#include "build_info.h"

extern cvar_t fragsleft;
// extern cvar_t timelimit;
extern cvar_t fraglimit;

// pEnt is the player the message needs to go to
// iChannel is the message channel defind in BMOD_messaging.h
// vecColor is r, g, b color values for the message
// vecTime is fadein, hold, and fadeout times in seconds.
// msg is the message to display. 
//
// Special msg values:
//   "SPEC" - spectator message.
//
void PrintMessage( CBaseEntity *pEnt, int iChannel, Vector vecColor, Vector vecTime, const char *msg )
{
	char szText[256];
	hudtextparms_t hText = {0};

	if (!strcmp("SPEC", msg))
	{
		char szTime[51] = "No Limit";

		sprintf(szText, "BubbleMod-X\n%s\n%s\nBuilt %s, commit %s, architecture %s, platform %s\n\nCurrent Map: %s\nNext Map: %s\nFrags Left: %i/%i",
		BMOD_BRANCH_NAME,
		BuildInfo::GetGitHubLink(),
		BuildInfo::GetDate(),
		BuildInfo::GetCommitHash(),
		BuildInfo::GetArchitecture(),
		BuildInfo::GetPlatform(),
		CVAR_GET_STRING("bm_map"), 
		CVAR_GET_STRING("bm_nextmap"), 
		(int)fragsleft.value,
		(int)fraglimit.value
		);
	}
	else
	{
		sprintf(szText, "%s",msg);
	}

	switch (iChannel)
	{
		case BMOD_CHAN_INFO:
			hText.x = 0.01;
			hText.y = 0.1;
			break;

		case BMOD_CHAN_RUNE:
			hText.x = -1;
			hText.y = 1;
			break;

		case BMOD_CHAN_COUNTDOWN:
			hText.x = -1;
			hText.y = -1;
			break;

		case BMOD_CHAN_WEAPON:
			hText.x = -1;
			hText.y = 0.875;
			break;
	}

	hText.a1 = 240; // Brightness
	hText.a2 = 240;

	hText.r1 = vecColor.x; // Color
	hText.r2 = vecColor.x;
	hText.g1 = vecColor.y;
	hText.g2 = vecColor.y;
	hText.b1 = vecColor.z;
	hText.b2 = vecColor.z;

	hText.channel = iChannel; // Channel       

	hText.effect = 0;       //Just fade in fade out
	hText.fadeinTime = vecTime.x; 
	hText.fadeoutTime = vecTime.z;
	hText.holdTime = vecTime.y;     
	hText.fxTime = 0.5;     //Does nothing in this case

	UTIL_HudMessage(pEnt, hText, szText);
}

void PrintClientMessage( entvars_t *client, int iChannel, Vector vecColor, Vector vecTime, const char *msg )
{
	char szText[256];
	hudtextparms_t hText = {0};

	sprintf(szText, "%s",msg);

	switch (iChannel)
	{
		case BMOD_CHAN_DEFENDER:
			hText.x = -1;
			hText.y = 0.6;
			break;

		case BMOD_CHAN_ATTACKER:
			hText.x = -1;
			hText.y = -0.6;
			break;
	}

	hText.a1 = 240; // Brightness
	hText.a2 = 240;

	hText.r1 = vecColor.x; // Color
	hText.r2 = vecColor.x;
	hText.g1 = vecColor.y;
	hText.g2 = vecColor.y;
	hText.b1 = vecColor.z;
	hText.b2 = vecColor.z;

	hText.channel = iChannel; // Channel

	hText.fadeinTime = vecTime.x;
	hText.fadeoutTime = vecTime.z;
	hText.holdTime = vecTime.y;
	hText.fxTime = 0,25;     //Does nothing in this case

	UTIL_HudMessagePlayer(client, hText, szText);
}

void PrintMessageAll( int iChannel, Vector vecColor, Vector vecTime, const char *msg )
{
	char szText[256];
	hudtextparms_t hText = {0};

	sprintf(szText, "%s",msg);

	switch (iChannel)
	{
		case BMOD_CHAN_TIMER:
			hText.x = -1;
			hText.y = 0.01;
			break;
	}

	hText.a1 = 240; // Brightness
	hText.a2 = 240;

	hText.r1 = vecColor.x; // Color
	hText.r2 = vecColor.x;
	hText.g1 = vecColor.y;
	hText.g2 = vecColor.y;
	hText.b1 = vecColor.z;
	hText.b2 = vecColor.z;

	hText.channel = iChannel; // Channel

	hText.fadeinTime = vecTime.x;
	hText.fadeoutTime = vecTime.z;
	hText.holdTime = vecTime.y;
	hText.fxTime = 0,25;     //Does nothing in this case

	UTIL_HudMessageAll(hText, szText);
}

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "util.h"
#include "BMOD_admin.h"
#include <ctime>

// Original code from https://github.com/tyabus/hlsdk-xash3d/

extern int gEvilImpulse101;

cvar_t admin_password = { "admin_password", "", FCVAR_SERVER | FCVAR_UNLOGGED };
cvar_t admin_kickonfail = { "admin_kickonfail", "0", FCVAR_SERVER };

void Admin_RegisterCVars( void )
{
	CVAR_REGISTER( &admin_kickonfail );
	CVAR_REGISTER( &admin_password );
}

void Admin_LogAttempt( CBasePlayer *pPlayer, char *LogType )
{
		FILE *fladminlog = fopen("adminattempts.log", "a");
		char time_str[80];
		time_t date_time = time(0);
		strftime(time_str, 80, "%Y.%m.%d %T", localtime(&date_time));
		const char *ip = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "ip" );

		if( !ip || !ip[0] )
			ip = "UNKNOWN";

		fprintf( fladminlog, "%s %s %s %s %s\n", time_str, LogType, ip, GETPLAYERAUTHID( pPlayer->edict() ), PlayerName( pPlayer ) ); // Timestamp, LogType, IP Address, EngineID, Nickname
		fclose( fladminlog );

		if( admin_kickonfail.value && !strcmp( LogType, "Failure login:" ) )
		{
			SERVER_COMMAND( UTIL_VarArgs( "kick #%i ""authorization failure""\n", GETPLAYERUSERID( pPlayer->edict() ) ) );
		}
}

bool Admin_ClientCommand( edict_t *pEntity )
{
    const char *pCmd = CMD_ARGV(0);
    CBasePlayer *pPlayer = (CBasePlayer*)GET_PRIVATE(pEntity);

	if( FStrEq(pCmd, "admin_login") )
	{
		if( !admin_password.string[0] || pPlayer->IsAdmin )
		{
			return false;
		}

		if( CMD_ARGC() != 2 )
		{
			PrintClientMsg( pPlayer, "Usage: admin_login <password>\n" );
			return true;
		}

		const char *passwordargv = CMD_ARGV( 1 );

		if( !strcmp( passwordargv, admin_password.string ) )
		{
			pPlayer->IsAdmin = true;
			PrintClientMsg( pPlayer, "Login successful\n" );
			ALERT( at_console, "Player %s: Became admin\n", PlayerName( pPlayer ) );
			Admin_LogAttempt( pPlayer, "Became admin:" );
		}
		else
		{
			PrintClientMsg( pPlayer, "Login failed\n" );
			ALERT( at_console, "Player %s: Login failure\n", PlayerName( pPlayer ) );
			Admin_LogAttempt( pPlayer, "Failure login:" );
		}
		return true;
	}
	if( !pPlayer->IsAdmin )
		return true;

	if( FStrEq(pCmd, "admin_notarget") || FStrEq(pCmd, "admin_notar") )
	{
		if( !FBitSet( pPlayer->pev->flags, FL_NOTARGET ) )
		{
			pPlayer->pev->flags |= FL_NOTARGET;
			PrintClientMsg( pPlayer, "Admin notarget ON\n" );
		}
		else
		{
			pPlayer->pev->flags &= ~FL_NOTARGET;
			PrintClientMsg( pPlayer, "Admin notarget OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_god") || FStrEq(pCmd, "admin_godmode") )
	{
		if( !FBitSet( pPlayer->pev->flags, FL_GODMODE ) )
		{
			pPlayer->pev->flags |= FL_GODMODE;
			PrintClientMsg( pPlayer, "Admin godmode ON\n" );
		}
		else
		{
			pPlayer->pev->flags &= ~FL_GODMODE;
			PrintClientMsg( pPlayer, "Admin godmode OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_invis") || FStrEq(pCmd, "admin_invisibility") )
	{
		if( pPlayer->pev->solid != SOLID_NOT )
		{
			pPlayer->pev->movetype = MOVETYPE_NOCLIP;
			pPlayer->pev->solid = SOLID_NOT;
			pPlayer->pev->takedamage = DAMAGE_NO;
			pPlayer->pev->effects |= EF_NODRAW;
			pPlayer->pev->flags |= FL_NOTARGET;
			pPlayer->pev->flags |= FL_GODMODE;
			pPlayer->m_fNoPlayerSound = TRUE;
			PrintClientMsg( pPlayer, "Admin invisibility ON\n" );
		}
		else
		{
			pPlayer->pev->takedamage = DAMAGE_YES;
			pPlayer->pev->movetype = MOVETYPE_WALK;
			pPlayer->pev->flags &= ~FL_NOTARGET;
			pPlayer->pev->flags &= ~FL_GODMODE;
			pPlayer->pev->effects &= ~EF_NODRAW;
			pPlayer->pev->solid = SOLID_SLIDEBOX;
			pPlayer->m_fNoPlayerSound = FALSE;
			PrintClientMsg( pPlayer, "Admin invisibility OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_noclip") )
	{
		if( pPlayer->pev->movetype != MOVETYPE_NOCLIP )
		{
			pPlayer->pev->movetype = MOVETYPE_NOCLIP;
			PrintClientMsg( pPlayer, "Admin noclip ON\n" );
		}
		else
		{
			pPlayer->pev->movetype = MOVETYPE_WALK;
			PrintClientMsg( pPlayer, "Admin noclip OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_101" ) )
	{
		gEvilImpulse101 = true;
		pPlayer->GiveNamedItem( "item_longjump" );
		pPlayer->GiveNamedItem( "item_battery" );
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveNamedItem( "ammo_9mmclip" );
		pPlayer->GiveNamedItem( "weapon_shotgun" );
		pPlayer->GiveNamedItem( "ammo_buckshot" );
		pPlayer->GiveNamedItem( "weapon_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_ARgrenades" );
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
		pPlayer->GiveNamedItem( "weapon_tripmine" );
		pPlayer->GiveNamedItem( "weapon_357" );
		pPlayer->GiveNamedItem( "ammo_357" );
		pPlayer->GiveNamedItem( "weapon_crossbow" );
		pPlayer->GiveNamedItem( "ammo_crossbow" );
		pPlayer->GiveNamedItem( "weapon_egon" );
		pPlayer->GiveNamedItem( "weapon_gauss" );
		pPlayer->GiveNamedItem( "ammo_gaussclip" );
		pPlayer->GiveNamedItem( "weapon_rpg" );
		pPlayer->GiveNamedItem( "ammo_rpgclip" );
		pPlayer->GiveNamedItem( "weapon_satchel" );
		pPlayer->GiveNamedItem( "weapon_snark" );
		pPlayer->GiveNamedItem( "weapon_hornetgun" );
		gEvilImpulse101 = FALSE;
		return true;
	}
	else if( FStrEq(pCmd, "admin_logout" ) )
	{
		pPlayer->IsAdmin = false;
		PrintClientMsg( pPlayer, "Successfully logged out\n" );
		ALERT( at_console, "Admin %s: Logging out\n", PlayerName( pPlayer ) );
		Admin_LogAttempt( pPlayer, "Successful logout:" );
		return true;
	}
	else if( FStrEq(pCmd, "s" ) ) // What's below it will be completed in the future
	{
		BModCmd_AdminSay();
		return true;
	}
	else if( FStrEq(pCmd, "w" ) )
	{
		BModCmd_AdminWhisper();
		return true;
	}
	else if( FStrEq(pCmd, "markspawnpoints" ) )
	{
		BModCmd_ShowSpawns();
		return true;
	}
	else if( FStrEq(pCmd, "sspeak" ) )
	{
		BModCmd_SpeakAll();
		return true;
	}
	else if( FStrEq(pCmd, "create" ) )
	{
		BModCmd_Create();
		return true;
	}
	else if( FStrEq(pCmd, "remove" ) )
	{
		BModCmd_Remove();
		return true;
	}
	else if( FStrEq(pCmd, "delete" ) )
	{
		BModCmd_Delete();
		return true;
	}
	else if( FStrEq(pCmd, "replace" ) )
	{
		BModCmd_Replace();
		return true;
	}
	else if( FStrEq(pCmd, "info" ) )
	{
		BModCmd_Info();
		return true;
	}
	else if( FStrEq(pCmd, "llama" ) )
	{
		BModCmd_Llama();
		return true;
	}
	else if( FStrEq(pCmd, "unlama" ) )
	{
		BModCmd_Unllama();
		return true;
	}
	return false;
}

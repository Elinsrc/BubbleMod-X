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

		fprintf( fladminlog, "%s %s %s %s %s\n", time_str, LogType, ip, GETPLAYERAUTHID( pPlayer->edict() ), PlayerName( pPlayer ) ); // Timestamp, LogType, IP Address, XashID, Nickname
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

	if( FStrEq(pCmd, "admin_sudo") )
	{
		int	j;
		char	*p;
		char	text[128];
		const char *pc, *pcmd = CMD_ARGV( 0 );

		if( CMD_ARGC() < 3 )
		{
			PrintClientMsg( pPlayer, "Usage: admin_sudo <UserID> <Command>\n" );
			return true;
		}

		short int UserID;

		if( CMD_ARGV(1)[0] == '#' )
			UserID = atoi( CMD_ARGV( 1 ) + 1 );
		else
			UserID = atoi( CMD_ARGV( 1 ) );

		p = (char *)CMD_ARGS();

		// Skip over the UID
		while( *p != ' ')
			p++;
		while( *p == ' ')
			p++;

		// make sure the text has content
		for( pc = p; pc != NULL && *pc != 0; pc++ )
		{
			if( isprint( *pc ) && !isspace( *pc ) )
			{
				pc = NULL;	// we've found an alphanumeric character,  so text is valid
				break;
			}
		}

		j = sizeof(text) - 2 - strlen( text );  // -2 for /n and null terminator
		if( (int)strlen( p ) > j )
			p[j] = 0;

		strcat( text, p );
		strcat( text, "\n" );

		CBasePlayer *pSudoer = GetPlayerByUID( UserID );

		if( !pSudoer )
		{
			PrintClientMsg( pPlayer, "Invalid player!\n" );
			return true;
		}

		CLIENT_COMMAND( pSudoer->edict(), "%s\n", text );
		PrintClientMsg( pPlayer, "Command was sent to %s\n", STRING( pSudoer->pev->netname ) );
		return true;
	}
	else if( FStrEq(pCmd, "admin_strip") )
	{
		if( CMD_ARGC() > 3 )
		{
			PrintClientMsg( pPlayer, "Usage: admin_strip <1/0> <UserID>\n" );
			return true;
		}

		BOOL StripSuit = atoi( CMD_ARGV( 1 ) );
		short int UserID = atoi( CMD_ARGV( 2 ) );
		CBasePlayer *pStripper = NULL;

		if( CMD_ARGV( 2 ) != NULL )
			pStripper = GetPlayerByUID( UserID );
		else
			pStripper = pPlayer;

		if( !pStripper && !UserID && pStripper != pPlayer )
		{
			PrintClientMsg( pPlayer, "Invalid player!\n" );
			return true;
		}

		if( pStripper == pPlayer )
			PrintClientMsg( pPlayer, "Removed all your items\n" );
		else
			PrintClientMsg( pPlayer, "Removed all %s items\n", STRING( pStripper->pev->netname ) );

		if( StripSuit )
		{
			pStripper->RemoveAllItems( TRUE );
		}
		else
		{
			pStripper->RemoveAllItems( FALSE );
		}

		return true;
	}
	else if( FStrEq(pCmd, "admin_notarget") || FStrEq(pCmd, "admin_notar") )
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
	else if( FStrEq(pCmd, "s" ) )
    {
        BModCmd_AdminSay();
    }
    else if( FStrEq(pCmd, "w" ) )
    {
        BModCmd_AdminWhisper();
    }
    else if( FStrEq(pCmd, "markspawnpoints" ) )
    {
        BModCmd_ShowSpawns();
    }
    else if( FStrEq(pCmd, "sspeak" ) )
    {
        BModCmd_SpeakAll();
    }
    else if( FStrEq(pCmd, "create" ) )
    {
        BModCmd_Create();
    }
    else if( FStrEq(pCmd, "remove" ) )
    {
        BModCmd_Remove();
    }
    else if( FStrEq(pCmd, "delete" ) )
    {
        BModCmd_Delete();
    }
    else if( FStrEq(pCmd, "replace" ) )
    {
        BModCmd_Replace();
    }
    else if( FStrEq(pCmd, "info" ) )
    {
        BModCmd_Info();
    }
    else if( FStrEq(pCmd, "llama" ) )
    {
        BModCmd_Llama();
    }
    else if( FStrEq(pCmd, "unlama" ) )
    {
        BModCmd_Unllama();
    }
	return false;
}

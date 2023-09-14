#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "util.h"
#include "BMOD_admin.h"
#include "BMOD_boxmarker.h"
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
	else if( FStrEq(pCmd, "admin_say" ) )
	{
		int	j;
		char	*p;
		char	text[128];
		const char *pc;

		// We can get a raw string now, without the "say " prepended
		if( CMD_ARGC() < 2 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_say \"<string>\"\n" );
			return true;
		}

		p = (char *)CMD_ARGS();

		// make sure the text has content
		for( pc = p; pc != NULL && *pc != 0; pc++ )
		{
			if( isprint( *pc ) && !isspace( *pc ) )
			{
				pc = NULL;	// we've found an alphanumeric character,  so text is valid
				break;
			}
		}

		if( pc != NULL )
			return true;  // no character found, so say nothing

		sprintf( text, "%c%s ", 2, "<ADMIN>" );

		j = sizeof(text) - 2 - strlen( text );  // -2 for /n and null terminator
		if( (int)strlen( p ) > j )
			p[j] = 0;

		strcat( text, p );
		strcat( text, "\n" );

		UTIL_ClientPrintAll( HUD_PRINTTALK, text );
		return true;
	}
	else if( FStrEq(pCmd, "admin_whisper" ) )
	{
		int	j;
		char	*p;
		char	text[128];
		const char *pc;

		// We can get a raw string now, without the "say " prepended
		if( CMD_ARGC() < 3 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_whisper <PlayerUID> \"<string>\"\n" );
			return true;
		}

		int UID = atoi( CMD_ARGV( 1 ) );

		CBasePlayer *Player = GetPlayerByUID( UID );
		if( Player == NULL )
		{
			PrintClientMsg( pPlayer, "Invalid Player UID.\n" );
			return true;
		}

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

		if( pc != NULL )
			return true;  // no character found, so say nothing

		sprintf( text, "%c%s ", 2, "<ADMIN>(whispers)" );

		j = sizeof(text) - 2 - strlen( text );  // -2 for /n and null terminator
		if( (int)strlen( p ) > j )
			p[j] = 0;

		strcat( text, p );
		strcat( text, "\n" );

		ClientPrint( Player->pev, HUD_PRINTTALK, text );
		return true;
	}
	else if( FStrEq(pCmd, "admin_showspawns" ) )
	{
		BOOL marked = UTIL_FindEntityByClassname( NULL, "boxmarker" ) ? TRUE : FALSE;

		if( marked )
		{
			PrintClientMsg( pPlayer, "Spawn points already marked!\n" );
			return true;
		}

		marked = TRUE;

		CBaseEntity *pSpot = NULL;
		CBaseEntity *pEnt = NULL;
		CBoxMarker *pBox = NULL;

		TraceResult tr;

		while( ( ( pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" ) ) != NULL ) )
		{
			UTIL_TraceLine( pSpot->pev->origin, pSpot->pev->origin - Vector( 0, 0, 1024 ), ignore_monsters, pSpot->edict(), &tr );
			Vector vecTop = pSpot->pev->origin + Vector( 0, 0, 36 );
			float height = fabs( vecTop.z - tr.vecEndPos.z ) / 2;

			pEnt = CBaseEntity::Create( "boxmarker", Vector( vecTop.x, vecTop.y, ( vecTop.z + tr.vecEndPos.z ) / 2), g_vecZero, NULL );
			// CBaseEntity *pEnt2 = CBaseEntity::Create( "zaprift", Vector( vecTop.x, vecTop.y, ( vecTop.z + tr.vecEndPos.z ) / 2 ), g_vecZero, NULL );
			pBox = (CBoxMarker *)pEnt;
			pBox->m_vecExtents = Vector(16,16,height);
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_speak" ) )
	{
		int	j;
		char	*p, *pc;
		char	text[128];

		if( CMD_ARGC() < 2 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_speak \"<vox string>\"\n" );
			return true;
		}

		p = (char *)CMD_ARGS();

		// remove quotes if present
		if( *p == '"' )
		{
			p++;
			p[strlen( p ) - 1] = 0;
		}

		// make sure the text has content
		for( pc = p; pc != NULL && *pc != 0; pc++ )
		{
			if( isprint( *pc ) && !isspace( *pc ) )
			{
				pc = NULL;	// we've found an alphanumeric character,  so text is valid
				break;
			}
		}
		if( pc != NULL )
			return true;  // no character found, so say nothing

		strcpy( text, "");

		j = sizeof( text ) - 2 - strlen( text );  // -2 for /n and null terminator
		if( (int)strlen( p ) > j )
			p[j] = 0;

		strcat( text, p );
		//strcat( text, "\n" );

		UTIL_SpeakAll( text );
		return true;
	}
	else if( FStrEq(pCmd, "admin_create" ) )
	{
		if( ( CMD_ARGC() < 6 ) )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_create <entity> <xpos> <ypos> <zpos> <y angle>\n" );
			return true;
		}

		char *entity = (char *)CMD_ARGV( 1 );
		Vector position = Vector( atoi( CMD_ARGV( 2 ) ), atoi( CMD_ARGV( 3 ) ), atoi( CMD_ARGV( 4 ) ) );
		Vector angle = Vector( 0, atoi( CMD_ARGV( 5 ) ), 0 );

		if( angle.y < 0 )
		{
			angle.y = RANDOM_LONG( 0, 360 );
		}

		// Fix weapon names
		if( !strcmp( entity, "weapon_9mmhandgun" ) )
		{
			strcpy( entity, "weapon_glock" );
		}
		else if( !strcmp( entity, "weapon_9mmAR" ) )
		{
			strcpy( entity, "weapon_mp5" );
		}
		else if( !strcmp( entity, "weapon_python" ) )
		{
			strcpy( entity, "weapon_357" );
		}

		// Fix ammo names
		if( !strcmp( entity, "ammo_9mmclip" ) )
		{
			strcpy( entity, "ammo_glockclip" );
		}
		else if( !strcmp( entity, "ammo_9mmAR" ) )
		{
			strcpy( entity, "ammo_mp5clip" );
		}
		else if( !strcmp( entity, "ammo_ARgrenades" ) )
		{
			strcpy( entity, "ammo_mp5grenades" );
		}

		if( !strncmp( entity, "weapon_", 7 ) ||
			!strcmp( entity, "item_healthkit" ) ||
			!strcmp( entity, "item_battery" ) ||
			!strcmp( entity, "item_longjump" ) ||
			!strncmp( entity, "ammo_", 5 ) ||
			!strcmp( entity, "info_player_deathmatch" ) )
		{
			CBaseEntity *ent = CBaseEntity::Create(entity, position, angle, NULL );

			if( !strcmp( entity, "info_player_deathmatch" ) )
				ent->pev->classname = MAKE_STRING( "info_player_deathmatch" );
		}
		else
		{
			PrintClientMsg( pPlayer, "You only add items, ammo, weapons, or spawn points with this command.\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_remove" ) )
	{
		if( ( CMD_ARGC() < 2 ) )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_remove <entity name>\n" );
			return true;
		}

		char *entity = (char *)CMD_ARGV( 1 );
		CBaseEntity *target = NULL;

		// Fix weapon names
		if( !strcmp(entity, "weapon_glock" ) )
		{
			strcpy(entity, "weapon_9mmhandgun" );
		}
		else if( !strcmp( entity, "weapon_mp5" ) )
		{
			strcpy( entity, "weapon_9mmAR" );
		}
		else if( !strcmp( entity, "weapon_python" ) )
		{
			strcpy( entity, "weapon_357" );
		}

		// Fix ammo names
		if( !strcmp( entity, "ammo_9mmclip" ) )
		{
			strcpy( entity, "ammo_glockclip" );
		}
		else if( !strcmp( entity, "ammo_9mmAR") )
		{
			strcpy( entity, "ammo_mp5clip");
		}
		else if( !strcmp( entity, "ammo_ARgrenades") )
		{
			strcpy( entity, "ammo_mp5grenades");
		}

		if( !strncmp( entity, "weapon_", 7 ) ||
			!strcmp( entity, "item_healthkit" ) ||
			!strcmp( entity, "item_battery" ) ||
			!strcmp( entity, "item_longjump" ) ||
			!strncmp( entity, "ammo_", 5 ) )
		{
			while( ( target = UTIL_FindEntityInSphere( target, Vector( 0, 0, 0 ), 4096 ) ) != NULL )
			{
				if( !strcmp( STRING( target->pev->classname ), entity ) && ( target->pev->owner == NULL ) )
				{
					target->Killed( NULL, 0 );
				}
			}
		}
		else
		{
			PrintClientMsg( pPlayer, "You only remove items, ammo, or weapons with this command.\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_delete" ) )
	{
		if( ( CMD_ARGC() < 5 ) )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_delete <entity name> <xpos> <ypos> <zpos>\n" );
			return true;
		}

		char *entity = (char *)CMD_ARGV( 1 );
		Vector position = Vector( atoi( CMD_ARGV( 2 ) ), atoi( CMD_ARGV( 3 ) ), atoi( CMD_ARGV( 4 ) ) );
		CBaseEntity *target = NULL;

		// Fix weapon names
		if( !strcmp( entity, "weapon_glock" ) )
		{
			strcpy( entity, "weapon_9mmhandgun" );
		}
		else if( !strcmp( entity, "weapon_mp5" ) )
		{
			strcpy( entity, "weapon_9mmAR" );
		}
		else if( !strcmp( entity, "weapon_python" ) )
		{
			strcpy( entity, "weapon_357" );
		}

		// Fix ammo names
		if( !strcmp( entity, "ammo_9mmclip" ) )
		{
			strcpy( entity, "ammo_glockclip" );
		}
		else if( !strcmp( entity, "ammo_9mmAR" ) )
		{
			strcpy( entity, "ammo_mp5clip" );
		}
		else if( !strcmp( entity, "ammo_ARgrenades" ) )
		{
			strcpy( entity, "ammo_mp5grenades" );
		}

		if( !strncmp( entity, "weapon_", 7 ) ||
			!strcmp( entity, "item_healthkit" ) ||
			!strcmp( entity, "item_battery" ) ||
			!strcmp( entity, "item_longjump" ) ||
			!strncmp( entity, "ammo_", 5 ) ||
			!strcmp( entity, "info_player_deathmatch" ) )
		{
			bool deleted = FALSE;
			while( !deleted && ( target = UTIL_FindEntityInSphere( target, position, 64 ) ) != NULL )
			{
				if( !strcmp( STRING( target->pev->classname ), entity ) && ( target->pev->owner == NULL ) )
				{
					target->Killed( NULL, 0 );
					deleted = TRUE;
					PrintClientMsg( pPlayer, "Entity deleted.\n" );
				}
			}

			if( !deleted )
			{
				PrintClientMsg( pPlayer, "Entity not found.\n" );
			}
		}
		else
		{
			PrintClientMsg( pPlayer, "You only delete items, ammo, weapons, or spawn points with this command.\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_replace" ) )
	{
		if( ( CMD_ARGC() < 3 ) )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_replace <entity> <with entity>\n" );
			return true;
		}

		char *entity = (char *)CMD_ARGV( 1 );
		char *entity2 = (char *)CMD_ARGV( 2 );
		CBaseEntity *target = NULL;

		// Fix weapon names
		if( !strcmp( entity, "weapon_glock" ) )
		{
			strcpy( entity, "weapon_9mmhandgun" );
		}
		else if( !strcmp( entity, "weapon_mp5" ) )
		{
			strcpy( entity, "weapon_9mmAR" );
		}
		else if( !strcmp( entity, "weapon_python" ) )
		{
			strcpy( entity, "weapon_357" );
		}

		if( !strcmp( entity2, "weapon_9mmhandgun" ) )
		{
			strcpy( entity2, "weapon_glock" );
		}
		else if( !strcmp( entity2, "weapon_9mmAR" ) )
		{
			strcpy( entity2, "weapon_mp5" );
		}
		else if( !strcmp( entity2, "weapon_python" ) )
		{
			strcpy( entity2, "weapon_357" );
		}

		// Fix ammo names
		if( !strcmp( entity, "ammo_9mmclip" ) )
		{
			strcpy( entity, "ammo_glockclip" );
		}
		else if( !strcmp( entity, "ammo_9mmAR") )
		{
			strcpy( entity, "ammo_mp5clip");
		}
		else if( !strcmp( entity, "ammo_ARgrenades") )
		{
			strcpy( entity, "ammo_mp5grenades" );
		}

		if( !strcmp( entity2, "ammo_9mmclip") )
		{
			strcpy( entity2, "ammo_glockclip" );
		}
		else if( !strcmp( entity2, "ammo_9mmAR" ) )
		{
			strcpy( entity2, "ammo_mp5clip" );
		}
		else if( !strcmp( entity2, "ammo_ARgrenades" ) )
		{
			strcpy( entity2, "ammo_mp5grenades" );
		}

		if( ( !strncmp( entity, "weapon_", 7 ) ||
			!strcmp( entity, "item_healthkit" ) ||
			!strcmp( entity, "item_battery" ) ||
			!strcmp( entity, "item_longjump" ) ||
			!strncmp( entity, "ammo_", 5 ) ) &&
		( !strncmp( entity2, "weapon_", 7 ) ||
			!strcmp( entity2, "item_healthkit" ) ||
			!strcmp( entity2, "item_battery" ) ||
			!strcmp( entity2, "item_longjump" ) ||
			!strncmp( entity2, "ammo_", 5 ) ) )
		{

			while( ( target = UTIL_FindEntityInSphere( target, Vector( 0, 0, 0 ), 4096 ) ) != NULL )
			{
				if( !strcmp( STRING( target->pev->classname ), entity ) && ( target->pev->owner == NULL ) )
				{
					CBaseEntity::Create(entity2, target->pev->origin, target->pev->angles, NULL );
					target->Killed(NULL, 0);
				}
			}
		}
		else
		{
			PrintClientMsg( pPlayer, "You only replace items, ammo, or weapons with this command.\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_info" ) )
	{
		if( CMD_ARGC() < 2 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_info <PlayerUID>\n" );
			return true;
		}

		int UID = atoi( CMD_ARGV( 1 ) );

		CBasePlayer *Player = GetPlayerByUID( UID );
		if( Player == NULL )
		{
			PrintClientMsg( pPlayer, "Invalid Player UID.\n" );
			return true;
		}

		PrintClientMsg( pPlayer, "Player: %s\n",STRING( Player->pev->netname ) );
		PrintClientMsg( pPlayer, "Health/Armor: %d/%d\n",(int)Player->pev->health, (int)Player->pev->armorvalue );

		PrintClientMsg( pPlayer, "Spawn Kills: %d\n",Player->m_iSpawnKills );
		PrintClientMsg( pPlayer, "Type Kills: %d\n",Player->m_iTypeKills );
		PrintClientMsg( pPlayer, "Leet: %d\n",Player->m_LeetSpeak );
		PrintClientMsg( pPlayer, "Locate: %d\n",Player->m_LocateMode );
		PrintClientMsg( pPlayer, "Llama: %d\n",Player->m_IsLlama );
		PrintClientMsg( pPlayer, "\n" );
		return true;
	}
	else if( FStrEq(pCmd, "admin_llama" ) )
	{
		if( CMD_ARGC() < 2 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_llama <PlayerUID>\n" );
			return true;
		}

		int UID = atoi( CMD_ARGV( 1 ) );

		CBasePlayer *Player = GetPlayerByUID( UID );
		if( Player == NULL )
		{
			PrintClientMsg( pPlayer, "Invalid Player UID.\n" );
			return true;
		}

		Player->m_IsLlama = TRUE;

		UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs( "<SERVER> %s is now a llama! Bleeet!\n", STRING( Player->pev->netname ) ) );
		PrintClientMsg( pPlayer, "%s is now a llama! Bleeet!\n", STRING( Player->pev->netname ) );
		return true;
	}
	else if( FStrEq(pCmd, "admin_unllama" ) )
	{
		if( CMD_ARGC() < 2 )
		{
			PrintClientMsg( pPlayer, "Not enough arguments.\nUSAGE: admin_unllama <PlayerUID>\n" );
			return true;
		}

		int UID = atoi( CMD_ARGV( 1 ) );

		CBasePlayer *Player = GetPlayerByUID( UID );
		if( Player == NULL )
		{
			PrintClientMsg( pPlayer, "Invalid Player UID.\n" );
			return true;
		}

		Player->m_IsLlama = FALSE;

		UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs( "<SERVER> %s is unllamafied.\n", STRING( Player->pev->netname ) ) );
		PrintClientMsg( pPlayer, "%s is unllamafied.\n", STRING( Player->pev->netname ) );
		return true;
	}
	return false;
}

#ifndef ADMIN_H
#define ADMIN_H

extern cvar_t admin_password;
extern cvar_t admin_kickonfail;

void Admin_RegisterCVars( void );
bool Admin_ClientCommand( edict_t *pEntity );
#endif


/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2007 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include <stdio.h>
#include <string>
#include "users.h"
#include "channels.h"
#include "modules.h"
#include "inspircd.h"

/* $ModDesc: Allows opers to set their idle time */

/** Handle /SETIDLE
 */
class cmd_setidle : public command_t
{
 public:
	cmd_setidle (InspIRCd* Instance) : command_t(Instance,"SETIDLE", 'o', 1)
	{
		this->source = "m_setidle.so";
		syntax = "<idle-seconds>";
	}

	CmdResult Handle (const char** parameters, int pcnt, userrec *user)
	{
		if (atoi(parameters[0]) < 1)
		{
			user->WriteServ("948 %s :Invalid idle time.",user->nick);
			return CMD_FAILURE;
		}
		user->idle_lastmsg = time(NULL) - atoi(parameters[0]);
		// minor tweak - we cant have signon time shorter than our idle time!
		if (user->signon > user->idle_lastmsg)
			user->signon = user->idle_lastmsg;
		ServerInstance->WriteOpers(std::string(user->nick)+" used SETIDLE to set their idle time to "+ConvToStr(atoi(parameters[0]))+" seconds");
		user->WriteServ("944 %s :Idle time set.",user->nick);

		return CMD_LOCALONLY;
	}
};


class ModuleSetIdle : public Module
{
	cmd_setidle*	mycommand;
 public:
	ModuleSetIdle(InspIRCd* Me)
		: Module::Module(Me)
	{
		
		mycommand = new cmd_setidle(ServerInstance);
		ServerInstance->AddCommand(mycommand);
	}
	
	virtual ~ModuleSetIdle()
	{
	}
	
	virtual Version GetVersion()
	{
		return Version(1,1,0,1,VF_VENDOR,API_VERSION);
	}
};

// stuff down here is the module-factory stuff. For basic modules you can ignore this.

class ModuleSetIdleFactory : public ModuleFactory
{
 public:
	ModuleSetIdleFactory()
	{
	}
	
	~ModuleSetIdleFactory()
	{
	}
	
	virtual Module * CreateModule(InspIRCd* Me)
	{
		return new ModuleSetIdle(Me);
	}
	
};


extern "C" void * init_module( void )
{
	return new ModuleSetIdleFactory;
}


/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2008 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include "inspircd.h"

/* $ModDesc: Sets (and unsets) modes on opers when they oper up */

class ModuleModesOnOper : public Module
{
 private:

	
	ConfigReader *Conf;

 public:
	ModuleModesOnOper(InspIRCd* Me)
		: Module(Me)
	{
		
		Conf = new ConfigReader(ServerInstance);
		Implementation eventlist[] = { I_OnPostOper, I_OnRehash };
		ServerInstance->Modules->Attach(eventlist, this, 2);
	}


	virtual void OnRehash(User* user, const std::string &parameter)
	{
		delete Conf;
		Conf = new ConfigReader(ServerInstance);
	}
	
	virtual ~ModuleModesOnOper()
	{
		delete Conf;
	}
	
	virtual Version GetVersion()
	{
		return Version(1,1,0,1,VF_VENDOR,API_VERSION);
	}
	
	virtual void OnPostOper(User* user, const std::string &opertype, const std::string &opername)
	{
		// whenever a user opers, go through the oper types, find their <type:modes>,
		// and if they have one apply their modes. The mode string can contain +modes
		// to add modes to the user or -modes to take modes from the user.
		for (int j =0; j < Conf->Enumerate("type"); j++)
		{
			std::string typen = Conf->ReadValue("type","name",j);
			if (typen == user->oper)
			{
				std::string ThisOpersModes = Conf->ReadValue("type","modes",j);
				if (!ThisOpersModes.empty())
				{
					ApplyModes(user, ThisOpersModes);
				}
				break;
			}
		}

		if (!opername.empty()) // if user is local ..
		{
			for (int j = 0; j < Conf->Enumerate("oper"); j++)
			{
				if (opername == Conf->ReadValue("oper", "name", j))
				{
					std::string ThisOpersModes = Conf->ReadValue("oper", "modes", j);
					if (!ThisOpersModes.empty())
					{
						ApplyModes(user, ThisOpersModes);
					}
					break;
				}
			}
		}
	}

	void ApplyModes(User *u, std::string &smodes)
	{
		char first = *(smodes.c_str());
		if ((first != '+') && (first != '-'))
			smodes = "+" + smodes;

		std::string buf;
		stringstream ss(smodes);
		vector<string> tokens;

		// split into modes and mode params
		while (ss >> buf)
			tokens.push_back(buf);

		int size = tokens.size() + 1;
		const char** modes = new const char*[size];
		modes[0] = u->nick;

		// process mode params
		int i = 1;
		for (unsigned int k = 0; k < tokens.size(); k++)
		{
			modes[i] = tokens[k].c_str();
			i++;
		}

		std::deque<std::string> n;
		Event rmode((char *)&n, NULL, "send_mode_explicit");
		for (unsigned int j = 0; j < tokens.size(); j++)
			n.push_back(modes[j]);

		rmode.Send(ServerInstance);
		ServerInstance->SendMode(modes, size, u);
		delete [] modes;
	}
};

MODULE_INIT(ModuleModesOnOper)

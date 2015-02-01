/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 1997-2014 ircd-hybrid development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 */

/*! \file m_ping.c
 * \brief Includes required functions for processing the PING command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "numeric.h"
#include "send.h"
#include "irc_string.h"
#include "parse.h"
#include "modules.h"
#include "hash.h"
#include "conf.h"
#include "server.h"

/*! \brief AUTHPONG command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = secret
 *		- parv[2] = hash
 */
static int
m_authpong(struct Client *source_p, int parc, char *parv[])
{
        assert(MyConnect(source_p));
        if (parc == 3 && !EmptyString(parv[1]) && !EmptyString(parv[2]))
        {
        uint8_t authpong[100];
        uint8_t hash[20];        
        memset(authpong, '\0', 100);
        memcpy(authpong, source_p->url, strlen(source_p->url));
        memcpy(authpong + strlen(source_p->url), source_p->authping, strlen(source_p->authping));
        computeRIPEMD160(authpong, strlen(source_p->url) + strlen(source_p->authping) , hash); 
        for(int i = 0;i < 20;i++)
                sprintf((char*)authpong + i*2, "%02X", hash[i]);
        
        if (memcmp(source_p->url, parv[1], strlen(source_p->url)) == 0 && memcmp(authpong, parv[2], strlen(authpong)) == 0)
        {
                SetAuthPing(source_p);
                if (!source_p->connection->registration)
                  register_local_user(source_p);
        }
        else
        exit_client(source_p,"Authentication Failed");   
  }
  else
    sendto_one(source_p, "Not enough parameters");

  return 0;
}


static struct Message authpong_msgtab =
{
  "AUTHPONG", NULL, 0, 0, 1, MAXPARA, MFLG_SLOW, 0,
  { m_authpong, m_ignore, m_ignore, m_ignore, m_ignore, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&authpong_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&authpong_msgtab);
}

struct module module_entry =
{
  .node    = { NULL, NULL, NULL },
  .name    = NULL,
  .version = "$Revision$",
  .handle  = NULL,
  .modinit = module_init,
  .modexit = module_exit,
  .flags   = 0
};

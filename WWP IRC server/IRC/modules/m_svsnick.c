/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 1999 Bahamut development team.
 *  Copyright (c) 2011-2014 ircd-hybrid development team
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

/*! \file m_svsnick.c
 * \brief Includes required functions for processing the SVSNICK command.
 * \version $Id$
 */


#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "channel_mode.h"
#include "send.h"
#include "parse.h"
#include "modules.h"
#include "irc_string.h"
#include "user.h"
#include "hash.h"
#include "watch.h"
#include "whowas.h"


/*! \brief SVSNICK command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = old nickname
 *      - parv[2] = new nickname
 *      - parv[3] = timestamp
 */
static int
ms_svsnick(struct Client *source_p, int parc, char *parv[])
{
  struct Client *target_p = NULL, *exists_p = NULL;

  if (!HasFlag(source_p, FLAGS_SERVICE) || !valid_nickname(parv[2], 1))
    return 0;

  if ((target_p = find_person(source_p, parv[1])) == NULL)
    return 0;

  if (!MyConnect(target_p))
  {
    if (target_p->from == source_p->from)
    {
      sendto_realops_flags(UMODE_DEBUG, L_ALL, SEND_NOTICE,
                           "Received wrong-direction SVSNICK "
                           "for %s (behind %s) from %s",
                           target_p->name, source_p->from->name,
                           get_client_name(source_p, HIDE_IP));
      return 0;
    }

    sendto_one(target_p, ":%s SVSNICK %s %s %s", source_p->id,
               target_p->id, parv[2], parv[3]);
    return 0;
  }

  if ((exists_p = hash_find_client(parv[2])))
  {
    if (target_p == exists_p)
    {
      if (!strcmp(target_p->name, parv[2]))
        return 0;
    }
    else if (IsUnknown(exists_p))
      exit_client(exists_p, "SVSNICK Override");
    else
    {
      exit_client(target_p, "SVSNICK Collide");
      return 0;
    }
  }

  target_p->tsinfo = atoi(parv[3]);
  clear_ban_cache_client(target_p);
  watch_check_hash(target_p, RPL_LOGOFF);

  if (HasUMode(target_p, UMODE_REGISTERED))
  {
    unsigned int oldmodes = target_p->umodes;
    char modebuf[IRCD_BUFSIZE] = "";

    DelUMode(target_p, UMODE_REGISTERED);
    send_umode(target_p, target_p, oldmodes, modebuf);
  }

  sendto_common_channels_local(target_p, 1, 0, ":%s!%s@%s NICK :%s",
                               target_p->name, target_p->username,
                               target_p->host, parv[2]);

  whowas_add_history(target_p, 1);

  sendto_server(NULL, 0, 0, ":%s NICK %s :%lu",
                target_p->id, parv[2], (unsigned long)target_p->tsinfo);
  hash_del_client(target_p);
  strlcpy(target_p->name, parv[2], sizeof(target_p->name));
  hash_add_client(target_p);

  watch_check_hash(target_p, RPL_LOGON);

  fd_note(&target_p->connection->fd, "Nick: %s", parv[2]);
  return 0;
}

static struct Message svsnick_msgtab =
{
  "SVSNICK", NULL, 0, 0, 4, MAXPARA, MFLG_SLOW, 0,
  { m_ignore, m_ignore, ms_svsnick, m_ignore, m_ignore, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&svsnick_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&svsnick_msgtab);
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

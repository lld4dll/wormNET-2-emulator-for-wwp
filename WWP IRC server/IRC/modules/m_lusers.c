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

/*! \file m_lusers.c
 * \brief Includes required functions for processing the LUSERS command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "numeric.h"
#include "server.h"
#include "user.h"
#include "send.h"
#include "conf.h"
#include "parse.h"
#include "modules.h"


/*! \brief LUSERS command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = ignored
 *      - parv[2] = nickname/servername
 */
static int
m_lusers(struct Client *source_p, int parc, char *parv[])
{
  static time_t last_used = 0;

  if ((last_used + ConfigGeneral.pace_wait_simple) > CurrentTime)
  {
    sendto_one_numeric(source_p, &me, RPL_LOAD2HI, "LUSERS");
    return 0;
  }

  last_used = CurrentTime;

  if (!ConfigServerHide.disable_remote_commands)
    if (hunt_server(source_p, ":%s LUSERS %s :%s", 2, parc, parv) != HUNTED_ISME)
      return 0;

  show_lusers(source_p);
  return 0;
}

/*! \brief LUSERS command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = ignored
 *      - parv[2] = nickname/servername
 */
static int
ms_lusers(struct Client *source_p, int parc, char *parv[])
{
  if (hunt_server(source_p, ":%s LUSERS %s :%s", 2, parc, parv) != HUNTED_ISME)
    return 0;

  show_lusers(source_p);
  return 0;
}

static struct Message lusers_msgtab =
{
  "LUSERS", NULL, 0, 0, 0, MAXPARA, MFLG_SLOW, 0,
  { m_unregistered, m_lusers, ms_lusers, m_ignore, ms_lusers, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&lusers_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&lusers_msgtab);
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

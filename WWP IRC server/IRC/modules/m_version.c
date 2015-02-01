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

/*! \file m_version.c
 * \brief Includes required functions for processing the VERSION command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "numeric.h"
#include "conf.h"
#include "server.h"
#include "user.h"
#include "send.h"
#include "parse.h"
#include "modules.h"


/* Option string. */
static const char serveropts[] =
{
  'T',
  'S',
#ifdef TS_CURRENT
  '0' + TS_CURRENT,
#endif
/* ONLY do TS */
/* ALWAYS do TS_WARNINGS */
  'o',
  'w',
  '\0'
};

/*! \brief VERSION command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = nickname/servername
 */
static int
m_version(struct Client *source_p, int parc, char *parv[])
{
  static time_t last_used = 0;

  if ((last_used + ConfigGeneral.pace_wait_simple) > CurrentTime)
  {
    sendto_one_numeric(source_p, &me, RPL_LOAD2HI, "VERSION");
    return 0;
  }

  last_used = CurrentTime;

  if (!ConfigServerHide.disable_remote_commands)
    if (hunt_server(source_p, ":%s VERSION :%s", 1, parc, parv) != HUNTED_ISME)
      return 0;

  sendto_one_numeric(source_p, &me, RPL_VERSION, ircd_version, serno,
                     me.name, serveropts);
  show_isupport(source_p);
  return 0;
}

/*! \brief VERSION command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = nickname/servername
 */
static int
ms_version(struct Client *source_p, int parc, char *parv[])
{
  if (hunt_server(source_p, ":%s VERSION :%s", 1, parc, parv) != HUNTED_ISME)
    return 0;

  sendto_one_numeric(source_p, &me, RPL_VERSION, ircd_version, serno,
                     me.name, serveropts);
  show_isupport(source_p);
  return 0;
}

static struct Message version_msgtab =
{
  "VERSION", NULL, 0, 0, 0, MAXPARA, MFLG_SLOW, 0,
  { m_unregistered, m_version, ms_version, m_ignore, ms_version, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&version_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&version_msgtab);
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

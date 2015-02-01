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

/*! \file m_svinfo.c
 * \brief Includes required functions for processing the SVINFO command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "irc_string.h"
#include "ircd.h"
#include "send.h"
#include "conf.h"
#include "log.h"
#include "parse.h"
#include "modules.h"


/*! \brief SVINFO command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = TS_CURRENT for the server
 *      - parv[2] = TS_MIN for the server
 *      - parv[3] = server is standalone or connected to non-TS only
 *      - parv[4] = server's idea of UTC time
 */
static int
ms_svinfo(struct Client *source_p, int parc, char *parv[])
{
  time_t deltat = 0, theirtime = 0;

  if (!IsServer(source_p) || !MyConnect(source_p))
    return 0;

  if (TS_CURRENT < atoi(parv[2]) || atoi(parv[1]) < TS_MIN)
  {
    /*
     * A server with the wrong TS version connected; since we're
     * TS_ONLY we can't fall back to the non-TS protocol so
     * we drop the link  -orabidoo
     */
    sendto_realops_flags(UMODE_ALL, L_ADMIN, SEND_NOTICE,
            "Link %s dropped, wrong TS protocol version (%s,%s)",
            get_client_name(source_p, SHOW_IP), parv[1], parv[2]);
    sendto_realops_flags(UMODE_ALL, L_OPER, SEND_NOTICE,
                 "Link %s dropped, wrong TS protocol version (%s,%s)",
                 get_client_name(source_p, MASK_IP), parv[1], parv[2]);
    exit_client(source_p, "Incompatible TS version");
    return 0;
  }

  /*
   * Since we're here, might as well set CurrentTime while we're at it
   */
  set_time();

  theirtime = atol(parv[4]);
  deltat = abs(theirtime - CurrentTime);

  if (deltat > ConfigGeneral.ts_max_delta)
  {
    sendto_realops_flags(UMODE_ALL, L_ADMIN, SEND_NOTICE,
          "Link %s dropped, excessive TS delta (my TS=%lu, their TS=%lu, delta=%d)",
          get_client_name(source_p, SHOW_IP),
          (unsigned long) CurrentTime,
          (unsigned long) theirtime,
          (int) deltat);
    sendto_realops_flags(UMODE_ALL, L_OPER, SEND_NOTICE,
          "Link %s dropped, excessive TS delta (my TS=%lu, their TS=%lu, delta=%d)",
           get_client_name(source_p, MASK_IP),
           (unsigned long) CurrentTime,
           (unsigned long) theirtime,
           (int) deltat);
    ilog(LOG_TYPE_IRCD,
         "Link %s dropped, excessive TS delta (my TS=%lu, their TS=%lu, delta=%d)",
         get_client_name(source_p, SHOW_IP),
         (unsigned long) CurrentTime,
         (unsigned long) theirtime,
         (int) deltat);
    exit_client(source_p, "Excessive TS delta");
    return 0;
  }

  if (deltat > ConfigGeneral.ts_warn_delta)
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                "Link %s notable TS delta (my TS=%lu, their TS=%lu, delta=%d)",
                source_p->name,
                (unsigned long) CurrentTime,
                (unsigned long) theirtime,
                (int) deltat);
  return 0;
}

static struct Message svinfo_msgtab =
{
  "SVINFO", NULL, 0, 0, 5, MAXPARA, MFLG_SLOW, 0,
  { m_unregistered, m_ignore, ms_svinfo, m_ignore, m_ignore, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&svinfo_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&svinfo_msgtab);
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

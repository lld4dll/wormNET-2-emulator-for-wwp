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

/*! \file m_svsmode.c
 * \brief Includes required functions for processing the SVSMODE command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "channel_mode.h"
#include "parse.h"
#include "modules.h"
#include "irc_string.h"
#include "user.h"
#include "conf.h"


/*! \brief SVSMODE command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = nickname
 *      - parv[2] = TS
 *      - parv[3] = mode
 *      - parv[4] = optional argument (services account, vhost)
 */
static int
ms_svsmode(struct Client *source_p, int parc, char *parv[])
{
  struct Client *target_p = NULL;
  int what = MODE_ADD;
  unsigned int flag = 0, setmodes = 0;
  const char *modes = NULL, *extarg = NULL;
  time_t ts = 0;

  if (!HasFlag(source_p, FLAGS_SERVICE))
    return 0;

  ts     = atol(parv[2]);
  modes  = parv[3];
  extarg = (parc > 4) ? parv[4] : NULL;

  if ((target_p = find_person(source_p, parv[1])) == NULL)
    return 0;

  if (ts && (ts != target_p->tsinfo))
    return 0;

  setmodes = target_p->umodes;

  for (const char *m = modes; *m; ++m)
  {
    switch (*m)
    {
      case '+':
        what = MODE_ADD;
        break;
      case '-':
        what = MODE_DEL;
        break;

      case 'd':
        if (!EmptyString(extarg))
          strlcpy(target_p->account, extarg, sizeof(target_p->account));
        break;

      case 'x':
        if (!EmptyString(extarg) && valid_hostname(extarg))
          user_set_hostmask(target_p, extarg, what);
        break;

      case 'o':
        if (what == MODE_DEL && HasUMode(target_p, UMODE_OPER))
        {
          ClearOper(target_p);
          --Count.oper;

          if (MyConnect(target_p))
          {
            dlink_node *node = NULL;

            detach_conf(target_p, CONF_OPER);
            ClrOFlag(target_p);
            DelUMode(target_p, ConfigGeneral.oper_only_umodes);

            if ((node = dlinkFindDelete(&oper_list, target_p)))
              free_dlink_node(node);
          }
        }

        break;

      case 'i':
        if (what == MODE_ADD && !HasUMode(target_p, UMODE_INVISIBLE))
        {
          AddUMode(target_p, UMODE_INVISIBLE);
          ++Count.invisi;
        }

        if (what == MODE_DEL && HasUMode(target_p, UMODE_INVISIBLE))
        {
          DelUMode(target_p, UMODE_INVISIBLE);
          --Count.invisi;
        }

        break;

      case 'S':  /* Only servers may set +S in a burst */
      case 'W':  /* Only servers may set +W in a burst */
        break;

      default:
        if ((flag = user_modes[(unsigned char)*m]))
        {
          if (what == MODE_ADD)
            AddUMode(target_p, flag);
          else
            DelUMode(target_p, flag);
        }

        break;
    }
  }

  if (extarg)
    sendto_server(source_p, 0, 0, ":%s SVSMODE %s %lu %s %s",
                  source_p->id,
                  target_p->id, (unsigned long)target_p->tsinfo, modes, extarg);
  else
    sendto_server(source_p, 0, 0, ":%s SVSMODE %s %lu %s",
                  source_p->id,
                  target_p->id, (unsigned long)target_p->tsinfo, modes);

  if (MyConnect(target_p) && (setmodes != target_p->umodes))
  {
    char modebuf[IRCD_BUFSIZE] = "";

    send_umode(target_p, target_p, setmodes, modebuf);
  }

  return 0;
}

static struct Message svsmode_msgtab =
{
  "SVSMODE", NULL, 0, 0, 4, MAXPARA, MFLG_SLOW, 0,
  { m_ignore, m_ignore, ms_svsmode, m_ignore, m_ignore, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&svsmode_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&svsmode_msgtab);
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

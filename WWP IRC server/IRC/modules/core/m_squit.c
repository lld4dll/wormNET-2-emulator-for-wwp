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

/*! \file m_squit.c
 * \brief Includes required functions for processing the SQUIT command.
 * \version $Id$
 */

#include "stdinc.h"
#include "list.h"
#include "client.h"
#include "hash.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "conf.h"
#include "log.h"
#include "server.h"
#include "send.h"
#include "parse.h"
#include "modules.h"


/*! \brief SQUIT command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = server name
 *      - parv[2] = comment
 */
static int
mo_squit(struct Client *source_p, int parc, char *parv[])
{
  char comment[REASONLEN + 1] = "";
  struct Client *target_p = NULL;
  dlink_node *node = NULL;
  const char *server = parv[1];

  if (parc < 2 || EmptyString(parv[1]))
  {
    sendto_one_numeric(source_p, &me, ERR_NEEDMOREPARAMS, "SQUIT");
    return 0;
  }

  /* The following allows wild cards in SQUIT. */
  DLINK_FOREACH(node, global_server_list.head)
  {
    struct Client *p = node->data;

    if (IsServer(p) || IsMe(p))
    {
      if (!match(server, p->name))
      {
        target_p = p;
        break;
      }
    }
  }

  if (!target_p || IsMe(target_p))
  {
    sendto_one_numeric(source_p, &me, ERR_NOSUCHSERVER, server);
    return 0;
  }

  if (!MyConnect(target_p) && !HasOFlag(source_p, OPER_FLAG_SQUIT_REMOTE))
  {
    sendto_one_numeric(source_p, &me, ERR_NOPRIVS, "squit:remote");
    return 0;
  }

  if (MyConnect(target_p) && !HasOFlag(source_p, OPER_FLAG_SQUIT))
  {
    sendto_one_numeric(source_p, &me, ERR_NOPRIVS, "squit");
    return 0;
  }

  if (!EmptyString(parv[2]))
    strlcpy(comment, parv[2], sizeof(comment));
  else
    strlcpy(comment, CONF_NOREASON, sizeof(comment));

  if (MyConnect(target_p))
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "Received SQUIT %s from %s (%s)",
                         target_p->name, get_client_name(source_p, HIDE_IP), comment);
    ilog(LOG_TYPE_IRCD, "Received SQUIT %s from %s (%s)",
         target_p->name, get_client_name(source_p, HIDE_IP), comment);

    /* To them, we are exiting */
    sendto_one(target_p, ":%s SQUIT %s :%s", source_p->id, me.id, comment);

    /* Send to everything but target */
    sendto_server(target_p, 0, 0, ":%s SQUIT %s :%s",
                  source_p->id, target_p->id, comment);
  }
  else
    /* Send to everything */
    sendto_server(NULL, 0, 0, ":%s SQUIT %s :%s",
                  source_p->id, target_p->id, comment);

  AddFlag(target_p, FLAGS_SQUIT);

  exit_client(target_p, comment);
  return 0;
}

/*! \brief SQUIT command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = server name
 *      - parv[2] = comment
 */
static int
ms_squit(struct Client *source_p, int parc, char *parv[])
{
  struct Client *target_p = NULL;
  const char *comment = NULL;
  dlink_node *node = NULL;

  if (parc < 2 || EmptyString(parv[1]))
    return 0;

  if ((target_p = hash_find_server(parv[1])) == NULL)
    return 0;

  if (!IsServer(target_p) && !IsMe(target_p))
    return 0;

  if (IsMe(target_p))
    target_p = source_p->from;

  comment = (parc > 2 && parv[parc - 1]) ? parv[parc - 1] : source_p->name;

  if (MyConnect(target_p))
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_GLOBAL, "from %s: Remote SQUIT %s from %s (%s)",
                         me.name, target_p->name, source_p->name, comment);
    sendto_server(source_p, 0, 0, ":%s GLOBOPS :Remote SQUIT %s from %s (%s)",
                  me.id, target_p->name, source_p->name, comment);
    ilog(LOG_TYPE_IRCD, "SQUIT From %s : %s (%s)", source_p->name,
         target_p->name, comment);

    /* To them, we are exiting */
    sendto_one(target_p, ":%s SQUIT %s :%s", source_p->id, me.id, comment);

    /* Send to everything but target and source */
    DLINK_FOREACH(node, local_server_list.head)
    {
      struct Client *client_p = node->data;

      if (client_p == target_p || client_p == source_p->from)
        continue;

      sendto_one(client_p, ":%s SQUIT %s :%s",
                 source_p->id, target_p->id, comment);
    }
  }
  else
    /* Send to everything but source */
    sendto_server(source_p, 0, 0, ":%s SQUIT %s :%s",
                  source_p->id, target_p->id, comment);

  AddFlag(target_p, FLAGS_SQUIT);

  exit_client(target_p, comment);
  return 0;
}

static struct Message squit_msgtab =
{
  "SQUIT", NULL, 0, 0, 1, MAXPARA, MFLG_SLOW, 0,
  { m_unregistered, m_not_oper, ms_squit, m_ignore, mo_squit, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&squit_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&squit_msgtab);
}

struct module module_entry =
{
  .node    = { NULL, NULL, NULL },
  .name    = NULL,
  .version = "$Revision$",
  .handle  = NULL,
  .modinit = module_init,
  .modexit = module_exit,
  .flags   = MODULE_FLAG_CORE
};

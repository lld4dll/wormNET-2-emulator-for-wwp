/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 2012-2014 ircd-hybrid development team
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

/*! \file m_webirc.c
 * \brief Includes required functions for processing the WEBIRC command.
 * \version $Id$
 */

#include "stdinc.h"
#include "list.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "irc_string.h"
#include "parse.h"
#include "modules.h"
#include "conf.h"
#include "hostmask.h"
#include "user.h"


/*! \brief WEBIRC command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = password
 *      - parv[2] = fake username (we ignore this)
 *      - parv[3] = fake hostname
 *      - parv[4] = fake ip
 */
static int
mr_webirc(struct Client *source_p, int parc, char *parv[])
{
  struct MaskItem *conf = NULL;
  struct addrinfo hints, *res;

  assert(MyConnect(source_p));

  if (!valid_hostname(parv[3]))
  {
    sendto_one_notice(source_p, &me, ":WEBIRC: Invalid hostname");
    return 0;
  }

  conf = find_address_conf(source_p->host,
                           IsGotId(source_p) ? source_p->username : "webirc",
                           &source_p->connection->ip,
                           source_p->connection->aftype, parv[1]);
  if (conf == NULL || !IsConfClient(conf))
    return 0;

  if (!IsConfWebIRC(conf))
  {
    sendto_one_notice(source_p, &me, ":Not a WEBIRC auth {} block");
    return 0;
  }

  if (EmptyString(conf->passwd))
  {
    sendto_one_notice(source_p, &me, ":WEBIRC auth {} blocks must have a password");
    return 0;
  }

  if (!match_conf_password(parv[1], conf))
  {
    sendto_one_notice(source_p, &me, ":WEBIRC password incorrect");
    return 0;
  }

  memset(&hints, 0, sizeof(hints));

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE | AI_NUMERICHOST;

  if (getaddrinfo(parv[4], NULL, &hints, &res))
  {
    sendto_one_notice(source_p, &me, ":Invalid WEBIRC IP %s", parv[4]);
    return 0;
  }

  assert(res);

  memcpy(&source_p->connection->ip, res->ai_addr, res->ai_addrlen);
  source_p->connection->ip.ss_len = res->ai_addrlen;
  source_p->connection->ip.ss.ss_family = res->ai_family;
  source_p->connection->aftype = res->ai_family;
  freeaddrinfo(res);

  strlcpy(source_p->sockhost, parv[4], sizeof(source_p->sockhost));
  strlcpy(source_p->host, parv[3], sizeof(source_p->host));

  /* Check dlines now, k/glines will be checked on registration */
  if ((conf = find_dline_conf(&source_p->connection->ip,
                               source_p->connection->aftype)))
  {
    if (!(conf->type == CONF_EXEMPT))
    {
      exit_client(source_p, "D-lined");
      return 0;
    }
  }

  AddUMode(source_p, UMODE_WEBIRC);
  sendto_one_notice(source_p, &me, ":WEBIRC host/IP set to %s %s",
                    parv[3], parv[4]);
  return 0;
}

static struct Message webirc_msgtab =
{
  "WEBIRC", NULL, 0, 0, 5, MAXPARA, MFLG_SLOW, 0,
  { mr_webirc, m_registered, m_ignore, m_ignore, m_registered, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&webirc_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&webirc_msgtab);
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

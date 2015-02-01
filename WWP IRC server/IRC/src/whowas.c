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

/*! \file whowas.c
 * \brief WHOWAS user cache.
 * \version $Id$
 */

#include "stdinc.h"
#include "list.h"
#include "whowas.h"
#include "client.h"
#include "hash.h"
#include "irc_string.h"
#include "ircd.h"


static struct Whowas WHOWAS[NICKNAMEHISTORYLENGTH];
dlink_list WHOWASHASH[HASHSIZE];


void
whowas_init(void)
{
  for (unsigned int i = 0; i < NICKNAMEHISTORYLENGTH; ++i)
    WHOWAS[i].hashv = -1;
}

void
whowas_add_history(struct Client *client_p, const int online)
{
  static unsigned int whowas_next = 0;
  struct Whowas *const who = &WHOWAS[whowas_next];

  assert(IsClient(client_p));

  if (++whowas_next == NICKNAMEHISTORYLENGTH)
    whowas_next = 0;

  if (who->hashv != -1)
  {
    if (who->online)
      dlinkDelete(&who->cnode, &who->online->whowas);

    dlinkDelete(&who->tnode, &WHOWASHASH[who->hashv]);
  }

  who->hashv = strhash(client_p->name);
  who->shide = IsHidden(client_p->servptr) != 0;
  who->logoff = CurrentTime;

  strlcpy(who->account, client_p->account, sizeof(who->account));
  strlcpy(who->name, client_p->name, sizeof(who->name));
  strlcpy(who->username, client_p->username, sizeof(who->username));
  strlcpy(who->hostname, client_p->host, sizeof(who->hostname));
  strlcpy(who->realname, client_p->info, sizeof(who->realname));
  strlcpy(who->servername, client_p->servptr->name, sizeof(who->servername));

  if (online)
  {
    who->online = client_p;
    dlinkAdd(who, &who->cnode, &client_p->whowas);
  }
  else
    who->online = NULL;

  dlinkAdd(who, &who->tnode, &WHOWASHASH[who->hashv]);
}

void
whowas_off_history(struct Client *client_p)
{
  dlink_node *node = NULL, *node_next = NULL;

  DLINK_FOREACH_SAFE(node, node_next, client_p->whowas.head)
  {
    struct Whowas *temp = node->data;

    temp->online = NULL;
    dlinkDelete(&temp->cnode, &client_p->whowas);
  }
}

struct Client *
whowas_get_history(const char *nick, time_t timelimit)
{
  dlink_node *node = NULL;

  timelimit = CurrentTime - timelimit;

  DLINK_FOREACH(node, WHOWASHASH[strhash(nick)].head)
  {
    struct Whowas *temp = node->data;

    if (temp->logoff < timelimit)
      continue;
    if (irccmp(nick, temp->name))
      continue;
    return temp->online;
  }

  return NULL;
}

void
whowas_count_memory(unsigned int *const count, uint64_t *const bytes)
{
  const struct Whowas *tmp = &WHOWAS[0];

  for (unsigned int i = 0; i < NICKNAMEHISTORYLENGTH; ++i, ++tmp)
  {
    if (tmp->hashv != -1)
    {
      (*count)++;
      (*bytes) += sizeof(struct Whowas);
    }
  }
}

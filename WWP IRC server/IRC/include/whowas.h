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

/*! \file whowas.h
 * \brief A header for the whowas functions.
 * \version $Id$
 */

#ifndef INCLUDED_whowas_h
#define INCLUDED_whowas_h

#include "ircd_defs.h"
#include "client.h"
#include "config.h"


struct Whowas
{
  int hashv;
  int shide;
  time_t logoff;
  char account[NICKLEN + 1];  /**< Services account */
  char name[NICKLEN + 1];
  char username[USERLEN + 1];
  char hostname[HOSTLEN + 1];
  char realname[REALLEN + 1];
  char servername[HOSTLEN + 1];
  struct Client *online; /* Pointer to new nickname for chasing or NULL */
  dlink_node tnode;      /* for hash table...                           */
  dlink_node cnode;      /* for client struct linked list               */
};

/*
** initwhowas
*/
extern void whowas_init(void);

/*
** whowas_add_history
**      Add the currently defined name of the client to history.
**      usually called before changing to a new name (nick).
**      Client must be a fully registered user.
*/
extern void whowas_add_history(struct Client *, const int);

/*
** whowas_off_history
**      This must be called when the client structure is about to
**      be released. History mechanism keeps pointers to client
**      structures and it must know when they cease to exist. This
**      also implicitly calls AddHistory.
*/
extern void whowas_off_history(struct Client *);

/*
** whowas_get_history
**      Return the current client that was using the given
**      nickname within the timelimit. Returns NULL, if no
**      one found...
*/
extern struct Client *whowas_get_history(const char *, time_t);

/*
** for debugging...counts related structures stored in whowas array.
*/
extern void whowas_count_memory(unsigned int *const, uint64_t *const);
extern dlink_list WHOWASHASH[];
#endif /* INCLUDED_whowas_h */

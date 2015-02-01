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

/*! \file hash.c
 * \brief Hash table management.
 * \version $Id$
 */

#include "stdinc.h"
#include "list.h"
#include "conf.h"
#include "channel.h"
#include "channel_mode.h"
#include "client.h"
#include "modules.h"
#include "hash.h"
#include "resv.h"
#include "rng_mt.h"
#include "userhost.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "send.h"
#include "memory.h"
#include "mempool.h"
#include "dbuf.h"
#include "user.h"


static mp_pool_t *userhost_pool = NULL;
static mp_pool_t *namehost_pool = NULL;

static unsigned int hashf_xor_key = 0;

/* The actual hash tables, They MUST be of the same HASHSIZE, variable
 * size tables could be supported but the rehash routine should also
 * rebuild the transformation maps, I kept the tables of equal size
 * so that I can use one hash function.
 */
static struct Client *idTable[HASHSIZE];
static struct Client *clientTable[HASHSIZE];
static struct Channel *channelTable[HASHSIZE];
static struct UserHost *userhostTable[HASHSIZE];


/* init_hash()
 *
 * inputs       - NONE
 * output       - NONE
 * side effects - Initialize the maps used by hash
 *                functions and clear the tables
 */
void
hash_init(void)
{
  userhost_pool = mp_pool_new(sizeof(struct UserHost), MP_CHUNK_SIZE_USERHOST);
  namehost_pool = mp_pool_new(sizeof(struct NameHost), MP_CHUNK_SIZE_NAMEHOST);

  hashf_xor_key = genrand_int32() % 256;  /* better than nothing --adx */
}

/*
 * New hash function based on the Fowler/Noll/Vo (FNV) algorithm from
 * http://www.isthe.com/chongo/tech/comp/fnv/
 *
 * Here, we use the FNV-1 method, which gives slightly better results
 * than FNV-1a.   -Michael
 */
unsigned int
strhash(const char *name)
{
  const unsigned char *p = (const unsigned char *)name;
  unsigned int hval = FNV1_32_INIT;

  if (EmptyString(p))
    return 0;

  for (; *p; ++p)
  {
    hval += (hval << 1) + (hval <<  4) + (hval << 7) +
            (hval << 8) + (hval << 24);
    hval ^= (ToLower(*p) ^ hashf_xor_key);
  }

  return (hval >> FNV1_32_BITS) ^ (hval & ((1 << FNV1_32_BITS) - 1));
}

/************************** Externally visible functions ********************/

/* Optimization note: in these functions I supposed that the CSE optimization
 * (Common Subexpression Elimination) does its work decently, this means that
 * I avoided introducing new variables to do the work myself and I did let
 * the optimizer play with more free registers, actual tests proved this
 * solution to be faster than doing things like tmp2=tmp->hnext... and then
 * use tmp2 myself which would have given less freedom to the optimizer.
 */

/* hash_add_client()
 *
 * inputs       - pointer to client
 * output       - NONE
 * side effects - Adds a client's name in the proper hash linked
 *                list, can't fail, client_p must have a non-null
 *                name or expect a coredump, the name is infact
 *                taken from client_p->name
 */
void
hash_add_client(struct Client *client_p)
{
  const unsigned int hashv = strhash(client_p->name);

  client_p->hnext = clientTable[hashv];
  clientTable[hashv] = client_p;
}

/* hash_add_channel()
 *
 * inputs       - pointer to channel
 * output       - NONE
 * side effects - Adds a channel's name in the proper hash linked
 *                list, can't fail. chptr must have a non-null name
 *                or expect a coredump. As before the name is taken
 *                from chptr->name, we do hash its entire lenght
 *                since this proved to be statistically faster
 */
void
hash_add_channel(struct Channel *chptr)
{
  const unsigned int hashv = strhash(chptr->name);

  chptr->hnextch = channelTable[hashv];
  channelTable[hashv] = chptr;
}

void
hash_add_userhost(struct UserHost *userhost)
{
  const unsigned int hashv = strhash(userhost->host);

  userhost->next = userhostTable[hashv];
  userhostTable[hashv] = userhost;
}

void
hash_add_id(struct Client *client_p)
{
  const unsigned int hashv = strhash(client_p->id);

  client_p->idhnext = idTable[hashv];
  idTable[hashv] = client_p;
}

/* hash_del_id()
 *
 * inputs       - pointer to client
 * output       - NONE
 * side effects - Removes an ID from the hash linked list
 */
void
hash_del_id(struct Client *client_p)
{
  const unsigned int hashv = strhash(client_p->id);
  struct Client *tmp = idTable[hashv];

  if (tmp)
  {
    if (tmp == client_p)
    {
      idTable[hashv] = client_p->idhnext;
      client_p->idhnext = client_p;
    }
    else
    {
      while (tmp->idhnext != client_p)
        if ((tmp = tmp->idhnext) == NULL)
          return;

      tmp->idhnext = tmp->idhnext->idhnext;
      client_p->idhnext = client_p;
    }
  }
}

/* hash_del_client()
 *
 * inputs       - pointer to client
 * output       - NONE
 * side effects - Removes a Client's name from the hash linked list
 */
void
hash_del_client(struct Client *client_p)
{
  const unsigned int hashv = strhash(client_p->name);
  struct Client *tmp = clientTable[hashv];

  if (tmp)
  {
    if (tmp == client_p)
    {
      clientTable[hashv] = client_p->hnext;
      client_p->hnext = client_p;
    }
    else
    {
      while (tmp->hnext != client_p)
        if ((tmp = tmp->hnext) == NULL)
          return;

      tmp->hnext = tmp->hnext->hnext;
      client_p->hnext = client_p;
    }
  }
}

/* hash_del_userhost()
 *
 * inputs       - pointer to userhost
 * output       - NONE
 * side effects - Removes a userhost from the hash linked list
 */
void
hash_del_userhost(struct UserHost *userhost)
{
  const unsigned int hashv = strhash(userhost->host);
  struct UserHost *tmp = userhostTable[hashv];

  if (tmp)
  {
    if (tmp == userhost)
    {
      userhostTable[hashv] = userhost->next;
      userhost->next = userhost;
    }
    else
    {
      while (tmp->next != userhost)
        if ((tmp = tmp->next) == NULL)
          return;

      tmp->next = tmp->next->next;
      userhost->next = userhost;
    }
  }
}

/* hash_del_channel()
 *
 * inputs       - pointer to client
 * output       - NONE
 * side effects - Removes the channel's name from the corresponding
 *                hash linked list
 */
void
hash_del_channel(struct Channel *chptr)
{
  const unsigned int hashv = strhash(chptr->name);
  struct Channel *tmp = channelTable[hashv];

  if (tmp)
  {
    if (tmp == chptr)
    {
      channelTable[hashv] = chptr->hnextch;
      chptr->hnextch = chptr;
    }
    else
    {
      while (tmp->hnextch != chptr)
        if ((tmp = tmp->hnextch) == NULL)
          return;

      tmp->hnextch = tmp->hnextch->hnextch;
      chptr->hnextch = chptr;
    }
  }
}

/* hash_find_client()
 *
 * inputs       - pointer to name
 * output       - NONE
 * side effects - New semantics: finds a client whose name is 'name'
 *                if can't find one returns NULL. If it finds one moves
 *                it to the top of the list and returns it.
 */
struct Client *
hash_find_client(const char *name)
{
  const unsigned int hashv = strhash(name);
  struct Client *client_p;

  if ((client_p = clientTable[hashv]))
  {
    if (irccmp(name, client_p->name))
    {
      struct Client *prev;

      while (prev = client_p, (client_p = client_p->hnext))
      {
        if (!irccmp(name, client_p->name))
        {
          prev->hnext = client_p->hnext;
          client_p->hnext = clientTable[hashv];
          clientTable[hashv] = client_p;
          break;
        }
      }
    }
  }

  return client_p;
}

struct Client *
hash_find_id(const char *name)
{
  const unsigned int hashv = strhash(name);
  struct Client *client_p;

  if ((client_p = idTable[hashv]))
  {
    if (strcmp(name, client_p->id))
    {
      struct Client *prev;

      while (prev = client_p, (client_p = client_p->idhnext))
      {
        if (!strcmp(name, client_p->id))
        {
          prev->idhnext = client_p->idhnext;
          client_p->idhnext = idTable[hashv];
          idTable[hashv] = client_p;
          break;
        }
      }
    }
  }

  return client_p;
}

struct Client *
hash_find_server(const char *name)
{
  const unsigned int hashv = strhash(name);
  struct Client *client_p = NULL;

  if (IsDigit(*name) && strlen(name) == IRC_MAXSID)
    return hash_find_id(name);

  if ((client_p = clientTable[hashv]))
  {
    if ((!IsServer(client_p) && !IsMe(client_p)) ||
        irccmp(name, client_p->name))
    {
      struct Client *prev;

      while (prev = client_p, (client_p = client_p->hnext))
      {
        if ((IsServer(client_p) || IsMe(client_p)) &&
            !irccmp(name, client_p->name))
        {
          prev->hnext = client_p->hnext;
          client_p->hnext = clientTable[hashv];
          clientTable[hashv] = client_p;
          break;
        }
      }
    }
  }

  return client_p;
}

/* hash_find_channel()
 *
 * inputs       - pointer to name
 * output       - NONE
 * side effects - New semantics: finds a channel whose name is 'name',
 *                if can't find one returns NULL, if can find it moves
 *                it to the top of the list and returns it.
 */
struct Channel *
hash_find_channel(const char *name)
{
  const unsigned int hashv = strhash(name);
  struct Channel *chptr = NULL;

  if ((chptr = channelTable[hashv]))
  {
    if (irccmp(name, chptr->name))
    {
      struct Channel *prev;

      while (prev = chptr, (chptr = chptr->hnextch))
      {
        if (!irccmp(name, chptr->name))
        {
          prev->hnextch = chptr->hnextch;
          chptr->hnextch = channelTable[hashv];
          channelTable[hashv] = chptr;
          break;
        }
      }
    }
  }

  return chptr;
}

/* hash_get_bucket(int type, unsigned int hashv)
 *
 * inputs       - hash value (must be between 0 and HASHSIZE - 1)
 * output       - NONE
 * returns      - pointer to first channel in channelTable[hashv]
 *                if that exists;
 *                NULL if there is no channel in that place;
 *                NULL if hashv is an invalid number.
 * side effects - NONE
 */
void *
hash_get_bucket(int type, unsigned int hashv)
{
  assert(hashv < HASHSIZE);

  if (hashv >= HASHSIZE)
      return NULL;

  switch (type)
  {
    case HASH_TYPE_ID:
      return idTable[hashv];
      break;
    case HASH_TYPE_CHANNEL:
      return channelTable[hashv];
      break;
    case HASH_TYPE_CLIENT:
      return clientTable[hashv];
      break;
    case HASH_TYPE_USERHOST:
      return userhostTable[hashv];
      break;
    default:
      assert(0);
  }

  return NULL;
}

struct UserHost *
hash_find_userhost(const char *host)
{
  const unsigned int hashv = strhash(host);
  struct UserHost *userhost;

  if ((userhost = userhostTable[hashv]))
  {
    if (irccmp(host, userhost->host))
    {
      struct UserHost *prev;

      while (prev = userhost, (userhost = userhost->next))
      {
        if (!irccmp(host, userhost->host))
        {
          prev->next = userhost->next;
          userhost->next = userhostTable[hashv];
          userhostTable[hashv] = userhost;
          break;
        }
      }
    }
  }

  return userhost;
}

/* count_user_host()
 *
 * inputs	- user name
 *		- hostname
 *		- int flag 1 if global, 0 if local
 * 		- pointer to where global count should go
 *		- pointer to where local count should go
 *		- pointer to where identd count should go (local clients only)
 * output	- none
 * side effects	-
 */
void
count_user_host(const char *user, const char *host, unsigned int *global_p,
                unsigned int *local_p, unsigned int *icount_p)
{
  dlink_node *node = NULL;
  struct UserHost *found_userhost;
  struct NameHost *nameh;

  if ((found_userhost = hash_find_userhost(host)) == NULL)
    return;

  DLINK_FOREACH(node, found_userhost->list.head)
  {
    nameh = node->data;

    if (!irccmp(user, nameh->name))
    {
      if (global_p)
        *global_p = nameh->gcount;
      if (local_p)
        *local_p  = nameh->lcount;
      if (icount_p)
        *icount_p = nameh->icount;

      return;
    }
  }
}

/* find_or_add_userhost()
 *
 * inputs       - host name
 * output       - none
 * side effects - find UserHost * for given host name
 */
static struct UserHost *
find_or_add_userhost(const char *host)
{
  struct UserHost *userhost = NULL;

  if ((userhost = hash_find_userhost(host)))
    return userhost;

  userhost = mp_pool_get(userhost_pool);

  strlcpy(userhost->host, host, sizeof(userhost->host));
  hash_add_userhost(userhost);

  return userhost;
}

/* add_user_host()
 *
 * inputs	- user name
 *		- hostname
 *		- int flag 1 if global, 0 if local
 * output	- none
 * side effects	- add given user@host to hash tables
 */
void
add_user_host(const char *user, const char *host, int global)
{
  dlink_node *node = NULL;
  struct UserHost *found_userhost;
  struct NameHost *nameh;
  unsigned int hasident = 1;

  if (*user == '~')
  {
    hasident = 0;
    ++user;
  }

  if ((found_userhost = find_or_add_userhost(host)) == NULL)
    return;

  DLINK_FOREACH(node, found_userhost->list.head)
  {
    nameh = node->data;

    if (!irccmp(user, nameh->name))
    {
      nameh->gcount++;

      if (!global)
      {
        if (hasident)
          nameh->icount++;
        nameh->lcount++;
      }

      return;
    }
  }

  nameh = mp_pool_get(namehost_pool);

  strlcpy(nameh->name, user, sizeof(nameh->name));

  nameh->gcount = 1;

  if (!global)
  {
    if (hasident)
      nameh->icount = 1;

    nameh->lcount = 1;
  }

  dlinkAdd(nameh, &nameh->node, &found_userhost->list);
}

/* delete_user_host()
 *
 * inputs	- user name
 *		- hostname
 *		- int flag 1 if global, 0 if local
 * output	- none
 * side effects	- delete given user@host to hash tables
 */
void
delete_user_host(const char *user, const char *host, int global)
{
  dlink_node *node = NULL;
  struct UserHost *found_userhost = NULL;
  unsigned int hasident = 1;

  if (*user == '~')
  {
    hasident = 0;
    ++user;
  }

  if ((found_userhost = hash_find_userhost(host)) == NULL)
    return;

  DLINK_FOREACH(node, found_userhost->list.head)
  {
    struct NameHost *nameh = node->data;

    if (!irccmp(user, nameh->name))
    {
      if (nameh->gcount > 0)
        nameh->gcount--;

      if (!global)
      {
        if (nameh->lcount > 0)
          nameh->lcount--;

        if (hasident && nameh->icount > 0)
          nameh->icount--;
      }

      if (nameh->gcount == 0 && nameh->lcount == 0)
      {
        dlinkDelete(&nameh->node, &found_userhost->list);
        mp_pool_release(nameh);
      }

      if (dlink_list_length(&found_userhost->list) == 0)
      {
        hash_del_userhost(found_userhost);
        mp_pool_release(found_userhost);
      }

      return;
    }
  }
}

/*
 * Safe list code.
 *
 * The idea is really quite simple. As the link lists pointed to in
 * each "bucket" of the channel hash table are traversed atomically
 * there is no locking needed. Overall, yes, inconsistent reported
 * state can still happen, but normally this isn't a big deal.
 * I don't like sticking the code into hash.c but oh well. Moreover,
 * if a hash isn't used in future, oops.
 *
 * - Dianora
 */

/* exceeding_sendq()
 *
 * inputs       - pointer to client to check
 * output	- 1 if client is in danger of blowing its sendq
 *		  0 if it is not.
 * side effects -
 *
 * Sendq limit is fairly conservative at 1/2 (In original anyway)
 */
static int
exceeding_sendq(const struct Client *to)
{
  if (dbuf_length(&to->connection->buf_sendq) > (get_sendq(&to->connection->confs) / 2))
    return 1;
  else
    return 0;
}

void
free_list_task(struct Client *source_p)
{
  struct ListTask *const lt = source_p->connection->list_task;
  dlink_node *node = NULL, *node_next = NULL;

  if ((node = dlinkFindDelete(&listing_client_list, source_p)))
    free_dlink_node(node);

  DLINK_FOREACH_SAFE(node, node_next, lt->show_mask.head)
  {
    MyFree(node->data);
    free_dlink_node(node);
  }

  DLINK_FOREACH_SAFE(node, node_next, lt->hide_mask.head)
  {
    MyFree(node->data);
    free_dlink_node(node);
  }

  MyFree(lt);
  source_p->connection->list_task = NULL;
}

/* list_allow_channel()
 *
 * inputs       - channel name
 *              - pointer to a list task
 * output       - 1 if the channel is to be displayed
 *                0 otherwise
 * side effects -
 */
static int
list_allow_channel(const char *name, const struct ListTask *lt)
{
  const dlink_node *node = NULL;

  DLINK_FOREACH(node, lt->show_mask.head)
    if (match(node->data, name) != 0)
      return 0;

  DLINK_FOREACH(node, lt->hide_mask.head)
    if (match(node->data, name) == 0)
      return 0;

  return 1;
}

/* list_one_channel()
 *
 * inputs       - client pointer to return result to
 *              - pointer to channel to list
 *              - pointer to ListTask structure
 * output	- none
 * side effects -
 */
static void
list_one_channel(struct Client *source_p, struct Channel *chptr)
{
  const struct ListTask *const lt = source_p->connection->list_task;
  char listbuf[MODEBUFLEN] = "";
  char modebuf[MODEBUFLEN] = "";
  char parabuf[MODEBUFLEN] = "";

  if (SecretChannel(chptr) &&
      !(HasUMode(source_p, UMODE_ADMIN) || IsMember(source_p, chptr)))
    return;

  if (dlink_list_length(&chptr->members) < lt->users_min ||
      dlink_list_length(&chptr->members) > lt->users_max ||
      (chptr->creationtime != 0 &&
       ((unsigned int)chptr->creationtime < lt->created_min ||
        (unsigned int)chptr->creationtime > lt->created_max)) ||
      (unsigned int)chptr->topic_time < lt->topicts_min ||
      (chptr->topic_time ? (unsigned int)chptr->topic_time : UINT_MAX) >
      lt->topicts_max)
    return;

  if (lt->topic[0] && match(lt->topic, chptr->topic))
    return;

  if (!list_allow_channel(chptr->name, lt))
    return;

  channel_modes(chptr, source_p, modebuf, parabuf);

  if (chptr->topic[0])
    snprintf(listbuf, sizeof(listbuf), "");
  else
    snprintf(listbuf, sizeof(listbuf), "");

  sendto_one_numeric(source_p, &me, RPL_LIST, chptr->name,
                     dlink_list_length(&chptr->members),
                     listbuf, chptr->topic);
}

/* safe_list_channels()
 *
 * inputs	- pointer to client requesting list
 * output	- 0/1
 * side effects	- safely list all channels to source_p
 *
 * Walk the channel buckets, ensure all pointers in a bucket are
 * traversed before blocking on a sendq. This means, no locking is needed.
 *
 * - Dianora
 */
void
safe_list_channels(struct Client *source_p, int only_unmasked_channels)
{
  struct ListTask *const lt = source_p->connection->list_task;
  struct Channel *chptr = NULL;

  if (!only_unmasked_channels)
  {
    for (unsigned int i = lt->hash_index; i < HASHSIZE; ++i)
    {
      if (exceeding_sendq(source_p))
      {
        lt->hash_index = i;
        return;  /* Still more to do */
      }

      for (chptr = channelTable[i]; chptr; chptr = chptr->hnextch)
        list_one_channel(source_p, chptr);
    }
  }
  else
  {
    dlink_node *node = NULL;

    DLINK_FOREACH(node, lt->show_mask.head)
      if ((chptr = hash_find_channel(node->data)))
        list_one_channel(source_p, chptr);
  }

  free_list_task(source_p);
  sendto_one_numeric(source_p, &me, RPL_LISTEND);
}

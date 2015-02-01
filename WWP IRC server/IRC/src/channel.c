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

/*! \file channel.c
 * \brief Responsible for managing channels, members, bans and topics
 * \version $Id$
 */

#include "stdinc.h"
#include "list.h"
#include "channel.h"
#include "channel_mode.h"
#include "client.h"
#include "hash.h"
#include "conf.h"
#include "hostmask.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "server.h"
#include "send.h"
#include "event.h"
#include "memory.h"
#include "mempool.h"
#include "misc.h"
#include "resv.h"


dlink_list channel_list;
mp_pool_t *ban_pool;    /*! \todo ban_pool shouldn't be a global var */

struct event splitmode_event =
{
  .name = "check_splitmode",
  .handler = check_splitmode,
  .when = 5
};

static mp_pool_t *member_pool, *channel_pool;


/*! \brief Initializes the channel blockheap, adds known channel CAPAB
 */
void
channel_init(void)
{
  add_capability("EX", CAP_EX, 1);
  add_capability("IE", CAP_IE, 1);

  channel_pool = mp_pool_new(sizeof(struct Channel), MP_CHUNK_SIZE_CHANNEL);
  ban_pool = mp_pool_new(sizeof(struct Ban), MP_CHUNK_SIZE_BAN);
  member_pool = mp_pool_new(sizeof(struct Membership), MP_CHUNK_SIZE_MEMBER);
}

/*! \brief Adds a user to a channel by adding another link to the
 *         channels member chain.
 * \param chptr      Pointer to channel to add client to
 * \param who        Pointer to client (who) to add
 * \param flags      Flags for chanops etc
 * \param flood_ctrl Whether to count this join in flood calculations
 */
void
add_user_to_channel(struct Channel *chptr, struct Client *who,
                    unsigned int flags, int flood_ctrl)
{
  struct Membership *member = NULL;

  if (GlobalSetOptions.joinfloodtime > 0)
  {
    if (flood_ctrl)
      ++chptr->number_joined;

    chptr->number_joined -= (CurrentTime - chptr->last_join_time) *
      (((float)GlobalSetOptions.joinfloodcount) /
       (float)GlobalSetOptions.joinfloodtime);

    if (chptr->number_joined <= 0)
    {
      chptr->number_joined = 0;
      ClearJoinFloodNoticed(chptr);
    }
    else if (chptr->number_joined >= GlobalSetOptions.joinfloodcount)
    {
      chptr->number_joined = GlobalSetOptions.joinfloodcount;

      if (!IsSetJoinFloodNoticed(chptr))
      {
        SetJoinFloodNoticed(chptr);
        sendto_realops_flags(UMODE_BOTS, L_ALL, SEND_NOTICE,
                             "Possible Join Flooder %s on %s target: %s",
                             get_client_name(who, HIDE_IP),
                             who->servptr->name, chptr->name);
      }
    }

    chptr->last_join_time = CurrentTime;
  }

  member = mp_pool_get(member_pool);
  member->client_p = who;
  member->chptr = chptr;
  member->flags = flags;

  dlinkAdd(member, &member->channode, &chptr->members);

  if (MyConnect(who))
    dlinkAdd(member, &member->locchannode, &chptr->locmembers);

  dlinkAdd(member, &member->usernode, &who->channel);
}

/*! \brief Deletes an user from a channel by removing a link in the
 *         channels member chain.
 * \param member Pointer to Membership struct
 */
void
remove_user_from_channel(struct Membership *member)
{
  struct Client *const client_p = member->client_p;
  struct Channel *const chptr = member->chptr;

  dlinkDelete(&member->channode, &chptr->members);

  if (MyConnect(client_p))
    dlinkDelete(&member->locchannode, &chptr->locmembers);

  dlinkDelete(&member->usernode, &client_p->channel);

  mp_pool_release(member);

  if (chptr->members.head == NULL)
    destroy_channel(chptr);
}

/* send_members()
 *
 * inputs       -
 * output       - NONE
 * side effects -
 */
static void
send_members(struct Client *client_p, const struct Channel *chptr,
             char *modebuf, char *parabuf)
{
  char buf[IRCD_BUFSIZE] = "";
  const dlink_node *node = NULL;
  int tlen;              /* length of text to append */
  char *t, *start;       /* temp char pointer */

  start = t = buf + snprintf(buf, sizeof(buf), ":%s SJOIN %lu %s %s %s:",
                             me.id, (unsigned long)chptr->creationtime,
                             chptr->name, modebuf, parabuf);

  DLINK_FOREACH(node, chptr->members.head)
  {
    const struct Membership *member = node->data;

    tlen = strlen(member->client_p->id) + 1;  /* +1 for space */

    if (member->flags & CHFL_CHANOP)
      ++tlen;
    if (member->flags & CHFL_HALFOP)
      ++tlen;
    if (member->flags & CHFL_VOICE)
      ++tlen;

    /*
     * Space will be converted into CR, but we also need space for LF..
     * That's why we use '- 1' here -adx
     */
    if (t + tlen - buf > IRCD_BUFSIZE - 1)
    {
      *(t - 1) = '\0';  /* Kill the space and terminate the string */
      sendto_one(client_p, "%s", buf);
      t = start;
    }

    if (member->flags & CHFL_CHANOP)
      *t++ = '@';
    if (member->flags & CHFL_HALFOP)
      *t++ = '%';
    if (member->flags & CHFL_VOICE)
      *t++ = '+';

    strcpy(t, member->client_p->id);

    t += strlen(t);
    *t++ = ' ';
  }

  /* Should always be non-NULL unless we have a kind of persistent channels */
  if (chptr->members.head)
    t--;  /* Take the space out */
  *t = '\0';
  sendto_one(client_p, "%s", buf);
}

/*! \brief Sends +b/+e/+I
 * \param client_p Client pointer to server
 * \param chptr    Pointer to channel
 * \param list     Pointer to list of modes to send
 * \param flag     Char flag flagging type of mode. Currently this can be 'b', e' or 'I'
 */
static void
send_mode_list(struct Client *client_p, const struct Channel *chptr,
               const dlink_list *list, const char flag)
{
  const dlink_node *node = NULL;
  char mbuf[IRCD_BUFSIZE] = "";
  char pbuf[IRCD_BUFSIZE] = "";
  int tlen, mlen, cur_len;
  char *pp = pbuf;

  if (!list->length)
    return;

  mlen = snprintf(mbuf, sizeof(mbuf), ":%s BMASK %lu %s %c :", me.id,
                  (unsigned long)chptr->creationtime, chptr->name, flag);
  cur_len = mlen;

  DLINK_FOREACH(node, list->head)
  {
    const struct Ban *ban = node->data;

    tlen = ban->len + 3;  /* +3 for ! + @ + space */

    /*
     * Send buffer and start over if we cannot fit another ban
     */
    if (cur_len + (tlen - 1) > IRCD_BUFSIZE - 2)
    {
      *(pp - 1) = '\0';  /* Get rid of trailing space on buffer */
      sendto_one(client_p, "%s%s", mbuf, pbuf);

      cur_len = mlen;
      pp = pbuf;
    }

    pp += sprintf(pp, "%s!%s@%s ", ban->name, ban->user, ban->host);
    cur_len += tlen;
  }

  *(pp - 1) = '\0';  /* Get rid of trailing space on buffer */
  sendto_one(client_p, "%s%s", mbuf, pbuf);
}

/*! \brief Send "client_p" a full list of the modes for channel chptr
 * \param client_p Pointer to client client_p
 * \param chptr    Pointer to channel pointer
 */
void
send_channel_modes(struct Client *client_p, struct Channel *chptr)
{
  char modebuf[MODEBUFLEN] = "";
  char parabuf[MODEBUFLEN] = "";

  channel_modes(chptr, client_p, modebuf, parabuf);
  send_members(client_p, chptr, modebuf, parabuf);

  send_mode_list(client_p, chptr, &chptr->banlist, 'b');
  send_mode_list(client_p, chptr, &chptr->exceptlist, 'e');
  send_mode_list(client_p, chptr, &chptr->invexlist, 'I');
}

/*! \brief Check channel name for invalid characters
 * \param name Pointer to channel name string
 * \param local Indicates whether it's a local or remote creation
 * \return 0 if invalid, 1 otherwise
 */
int
check_channel_name(const char *name, const int local)
{
  const char *p = name;

  assert(p);

  if (!IsChanPrefix(*p))
    return 0;

  if (!local || !ConfigChannel.disable_fake_channels)
  {
    while (*++p)
      if (!IsChanChar(*p))
        return 0;
  }
  else
  {
    while (*++p)
      if (!IsVisibleChanChar(*p))
        return 0;
  }

  return p - name <= CHANNELLEN;
}

void
remove_ban(struct Ban *ban, dlink_list *list)
{
  dlinkDelete(&ban->node, list);

  MyFree(ban->name);
  MyFree(ban->user);
  MyFree(ban->host);
  MyFree(ban->who);

  mp_pool_release(ban);
}

/* free_channel_list()
 *
 * inputs       - pointer to dlink_list
 * output       - NONE
 * side effects -
 */
void
free_channel_list(dlink_list *list)
{
  dlink_node *node = NULL, *node_next = NULL;

  DLINK_FOREACH_SAFE(node, node_next, list->head)
    remove_ban(node->data, list);

  assert(list->tail == NULL && list->head == NULL);
}

/*! \brief Get Channel block for name (and allocate a new channel
 *         block, if it didn't exist before)
 * \param name Channel name
 * \return Channel block
 */
struct Channel *
make_channel(const char *name)
{
  struct Channel *chptr = NULL;

  assert(!EmptyString(name));

  chptr = mp_pool_get(channel_pool);

  /* Doesn't hurt to set it here */
  chptr->creationtime = CurrentTime;
  chptr->last_join_time = CurrentTime;

  strlcpy(chptr->name, name, sizeof(chptr->name));
  dlinkAdd(chptr, &chptr->node, &channel_list);

  hash_add_channel(chptr);

  return chptr;
}

/*! \brief Walk through this channel, and destroy it.
 * \param chptr Channel pointer
 */
void
destroy_channel(struct Channel *chptr)
{
  clear_invites(chptr);

  /* Free ban/exception/invex lists */
  free_channel_list(&chptr->banlist);
  free_channel_list(&chptr->exceptlist);
  free_channel_list(&chptr->invexlist);

  dlinkDelete(&chptr->node, &channel_list);
  hash_del_channel(chptr);

  mp_pool_release(chptr);
}

/*!
 * \param chptr Pointer to channel
 * \return String pointer "=" if public, "@" if secret else "*"
 */
static const char *
channel_pub_or_secret(const struct Channel *chptr)
{
  if (SecretChannel(chptr))
    return "@";
  if (PrivateChannel(chptr))
    return "*";
  return "=";
}

/*! \brief lists all names on given channel
 * \param source_p Pointer to client struct requesting names
 * \param chptr    Pointer to channel block
 * \param show_eon Show RPL_ENDOFNAMES numeric or not
 *                 (don't want it with /names with no params)
 */
void
channel_member_names(struct Client *source_p, struct Channel *chptr,
                     int show_eon)
{
  const dlink_node *node = NULL;
  char buf[IRCD_BUFSIZE + 1] = "";
  char *t = NULL, *start = NULL;
  int tlen = 0;
  const int is_member = IsMember(source_p, chptr);
  const int multi_prefix = HasCap(source_p, CAP_MULTI_PREFIX) != 0;
  const int uhnames = HasCap(source_p, CAP_UHNAMES) != 0;

  if (PubChannel(chptr) || is_member)
  {
    t = buf + snprintf(buf, sizeof(buf), numeric_form(RPL_NAMREPLY),
                       me.name, source_p->name,
                       channel_pub_or_secret(chptr), chptr->name);
    start = t;

    DLINK_FOREACH(node, chptr->members.head)
    {
      const struct Membership *member = node->data;

      if (HasUMode(member->client_p, UMODE_INVISIBLE) && !is_member)
        continue;

      if (!uhnames)
        tlen = strlen(member->client_p->name) + 1;  /* +1 for space */
      else
        tlen = strlen(member->client_p->name) + strlen(member->client_p->username) +
               strlen(member->client_p->host) + 3;  /* +3 for ! + @ + space */

      if (!multi_prefix)
      {
        if (member->flags & (CHFL_CHANOP | CHFL_HALFOP | CHFL_VOICE))
          ++tlen;
      }
      else
      {
        if (member->flags & CHFL_CHANOP)
          ++tlen;
        if (member->flags & CHFL_HALFOP)
          ++tlen;
        if (member->flags & CHFL_VOICE)
          ++tlen;
      }

      if (t + tlen - buf > IRCD_BUFSIZE - 2)
      {
        *(t - 1) = '\0';
        sendto_one(source_p, "%s", buf);
        t = start;
      }

      if (!uhnames)
        t += sprintf(t, "%s%s ", get_member_status(member, multi_prefix),
                     member->client_p->name);
      else
        t += sprintf(t, "%s%s!%s@%s ", get_member_status(member, multi_prefix),
                     member->client_p->name, member->client_p->username,
                     member->client_p->host);
    }

    if (tlen)
    {
      *(t - 1) = '\0';
      sendto_one(source_p, "%s", buf);
    }
  }

  if (show_eon)
    sendto_one_numeric(source_p, &me, RPL_ENDOFNAMES, chptr->name);
}

/*! \brief Adds client to invite list
 * \param chptr Pointer to channel block
 * \param who   Pointer to client to add invite to
 */
void
add_invite(struct Channel *chptr, struct Client *who)
{
  del_invite(chptr, who);

  /*
   * Delete last link in chain if the list is max length
   */
  if (dlink_list_length(&who->connection->invited) >=
      ConfigChannel.max_channels)
    del_invite(who->connection->invited.tail->data, who);

  /* Add client to channel invite list */
  dlinkAdd(who, make_dlink_node(), &chptr->invites);

  /* Add channel to the end of the client invite list */
  dlinkAdd(chptr, make_dlink_node(), &who->connection->invited);
}

/*! \brief Delete Invite block from channel invite list
 *         and client invite list
 * \param chptr Pointer to Channel struct
 * \param who   Pointer to client to remove invites from
 */
void
del_invite(struct Channel *chptr, struct Client *who)
{
  dlink_node *node = NULL;

  if ((node = dlinkFindDelete(&who->connection->invited, chptr)))
    free_dlink_node(node);

  if ((node = dlinkFindDelete(&chptr->invites, who)))
    free_dlink_node(node);
}

/*! \brief Removes all invites of a specific channel
 * \param chptr Pointer to Channel struct
 */
void
clear_invites(struct Channel *chptr)
{
  dlink_node *node = NULL, *node_next = NULL;

  DLINK_FOREACH_SAFE(node, node_next, chptr->invites.head)
    del_invite(chptr, node->data);
}

/* get_member_status()
 *
 * inputs       - pointer to struct Membership
 *              - YES if we can combine different flags
 * output       - string either @, +, % or "" depending on whether
 *                chanop, voiced or user
 * side effects -
 *
 * NOTE: Returned string is usually a static buffer
 * (like in get_client_name)
 */
const char *
get_member_status(const struct Membership *member, const int combine)
{
  static char buffer[4];  /* 4 for @%+\0 */
  char *p = buffer;

  if (member->flags & CHFL_CHANOP)
  {
    if (!combine)
      return "@";
    *p++ = '@';
  }

  if (member->flags & CHFL_HALFOP)
  {
    if (!combine)
      return "%";
    *p++ = '%';
  }

  if (member->flags & CHFL_VOICE)
    *p++ = '+';
  *p = '\0';

  return buffer;
}

/*!
 * \param who  Pointer to Client to check
 * \param list Pointer to ban list to search
 * \return 1 if ban found for given n!u\@h mask, 0 otherwise
 *
 */
static int
find_bmask(const struct Client *who, const dlink_list *const list)
{
  const dlink_node *node = NULL;

  DLINK_FOREACH(node, list->head)
  {
    const struct Ban *ban = node->data;

    if (!match(ban->name, who->name) && !match(ban->user, who->username))
    {
      switch (ban->type)
      {
        case HM_HOST:
          if (!match(ban->host, who->host) || !match(ban->host, who->sockhost))
            return 1;
          break;
        case HM_IPV4:
          if (who->connection->aftype == AF_INET)
            if (match_ipv4(&who->connection->ip, &ban->addr, ban->bits))
              return 1;
          break;
        case HM_IPV6:
          if (who->connection->aftype == AF_INET6)
            if (match_ipv6(&who->connection->ip, &ban->addr, ban->bits))
              return 1;
          break;
        default:
          assert(0);
      }
    }
  }

  return 0;
}

/*!
 * \param chptr Pointer to channel block
 * \param who   Pointer to client to check access fo
 * \return 0 if not banned, 1 otherwise
 */
int
is_banned(const struct Channel *chptr, const struct Client *who)
{
  if (find_bmask(who, &chptr->banlist))
    if (!find_bmask(who, &chptr->exceptlist))
      return 1;

  return 0;
}

/*! Tests if a client can join a certain channel
 * \param source_p Pointer to client attempting to join
 * \param chptr    Pointer to channel
 * \param key      Key sent by client attempting to join if present
 * \return ERR_BANNEDFROMCHAN, ERR_INVITEONLYCHAN, ERR_CHANNELISFULL
 *         or 0 if allowed to join.
 */
int
can_join(struct Client *source_p, const struct Channel *chptr, const char *key)
{
  if ((chptr->mode.mode & MODE_SSLONLY) && !HasUMode(source_p, UMODE_SSL))
    return ERR_SSLONLYCHAN;

  if ((chptr->mode.mode & MODE_REGONLY) && !HasUMode(source_p, UMODE_REGISTERED))
    return ERR_NEEDREGGEDNICK;

  if ((chptr->mode.mode & MODE_OPERONLY) && !HasUMode(source_p, UMODE_OPER))
    return ERR_OPERONLYCHAN;

  if (chptr->mode.mode & MODE_INVITEONLY)
    if (!dlinkFind(&source_p->connection->invited, chptr))
      if (!find_bmask(source_p, &chptr->invexlist))
        return ERR_INVITEONLYCHAN;

  if (chptr->mode.key[0] && (!key || strcmp(chptr->mode.key, key)))
    return ERR_BADCHANNELKEY;

  if (chptr->mode.limit && dlink_list_length(&chptr->members) >=
      chptr->mode.limit)
    return ERR_CHANNELISFULL;

  if (is_banned(chptr, source_p))
    return ERR_BANNEDFROMCHAN;

  return 0;
}

int
has_member_flags(const struct Membership *member, const unsigned int flags)
{
  return member && (member->flags & flags);
}

struct Membership *
find_channel_link(struct Client *client_p, struct Channel *chptr)
{
  dlink_node *node = NULL;

  if (!IsClient(client_p))
    return NULL;

  if (dlink_list_length(&chptr->members) < dlink_list_length(&client_p->channel))
  {
    DLINK_FOREACH(node, chptr->members.head)
      if (((struct Membership *)node->data)->client_p == client_p)
        return node->data;
  }
  else
  {
    DLINK_FOREACH(node, client_p->channel.head)
      if (((struct Membership *)node->data)->chptr == chptr)
        return node->data;
  }

  return NULL;
}

/*! Checks if a message contains control codes
 * \param message The actual message string the client wants to send
 * \return 1 if the message does contain any control codes, 0 otherwise
 */
static int
msg_has_ctrls(const char *message)
{
  const unsigned char *p = (const unsigned char *)message;

  for (; *p; ++p)
  {
    if (*p > 31 || *p == 1)
      continue;  /* No control code or CTCP */

    if (*p == 27)  /* Escape */
    {
      /* ISO 2022 charset shift sequence */
      if (*(p + 1) == '$' ||
          *(p + 1) == '(')
      {
        ++p;
        continue;
      }
    }

    return 1;  /* Control code */
  }

  return 0;  /* No control code found */
}

/*! Tests if a client can send to a channel
 * \param chptr    Pointer to Channel struct
 * \param source_p Pointer to Client struct
 * \param member   Pointer to Membership struct (can be NULL)
 * \param message  The actual message string the client wants to send
 * \return CAN_SEND_OPV if op or voiced on channel\n
 *         CAN_SEND_NONOP if can send to channel but is not an op\n
 *         ERR_CANNOTSENDTOCHAN or ERR_NEEDREGGEDNICK if they cannot send to channel\n
 */
int
can_send(struct Channel *chptr, struct Client *source_p,
         struct Membership *member, const char *message)
{
  const struct MaskItem *conf = NULL;

  if (IsServer(source_p) || HasFlag(source_p, FLAGS_SERVICE))
    return CAN_SEND_OPV;

  if (MyClient(source_p) && !IsExemptResv(source_p))
    if (!(HasUMode(source_p, UMODE_OPER) && ConfigGeneral.oper_pass_resv))
      if ((conf = match_find_resv(chptr->name)) && !resv_find_exempt(source_p, conf))
        return ERR_CANNOTSENDTOCHAN;

  if ((chptr->mode.mode & MODE_NOCTRL) && msg_has_ctrls(message))
    return ERR_NOCTRLSONCHAN;

  if (member || (member = find_channel_link(source_p, chptr)))
    if (member->flags & (CHFL_CHANOP|CHFL_HALFOP|CHFL_VOICE))
      return CAN_SEND_OPV;

  if (!member && (chptr->mode.mode & MODE_NOPRIVMSGS))
    return ERR_CANNOTSENDTOCHAN;

  if (chptr->mode.mode & MODE_MODERATED)
    return ERR_CANNOTSENDTOCHAN;

  if ((chptr->mode.mode & MODE_MODREG) && !HasUMode(source_p, UMODE_REGISTERED))
    return ERR_NEEDREGGEDNICK;

  /* Cache can send if banned */
  if (MyClient(source_p))
  {
    if (member)
    {
      if (member->flags & CHFL_BAN_SILENCED)
        return ERR_CANNOTSENDTOCHAN;

      if (!(member->flags & CHFL_BAN_CHECKED))
      {
        if (is_banned(chptr, source_p))
        {
          member->flags |= (CHFL_BAN_CHECKED|CHFL_BAN_SILENCED);
          return ERR_CANNOTSENDTOCHAN;
        }

        member->flags |= CHFL_BAN_CHECKED;
      }
    }
    else if (is_banned(chptr, source_p))
      return ERR_CANNOTSENDTOCHAN;
  }

  return CAN_SEND_NONOP;
}

/*! \brief Updates the client's oper_warn_count_down, warns the
 *         IRC operators if necessary, and updates
 *         join_leave_countdown as needed.
 * \param source_p Pointer to struct Client to check
 * \param name     Channel name or NULL if this is a part.
 */
void
check_spambot_warning(struct Client *source_p, const char *name)
{
  int t_delta = 0;
  int decrement_count = 0;

  if ((GlobalSetOptions.spam_num &&
       (source_p->connection->join_leave_count >=
        GlobalSetOptions.spam_num)))
  {
    if (source_p->connection->oper_warn_count_down > 0)
      source_p->connection->oper_warn_count_down--;
    else
      source_p->connection->oper_warn_count_down = 0;

    if (source_p->connection->oper_warn_count_down == 0)
    {
      /* It's already known as a possible spambot */
      if (name)
        sendto_realops_flags(UMODE_BOTS, L_ALL, SEND_NOTICE,
                             "User %s (%s@%s) trying to join %s is a possible spambot",
                             source_p->name, source_p->username,
                             source_p->host, name);
      else
        sendto_realops_flags(UMODE_BOTS, L_ALL, SEND_NOTICE,
                             "User %s (%s@%s) is a possible spambot",
                             source_p->name, source_p->username,
                             source_p->host);
      source_p->connection->oper_warn_count_down = OPER_SPAM_COUNTDOWN;
    }
  }
  else
  {
    if ((t_delta = (CurrentTime - source_p->connection->last_leave_time)) >
         JOIN_LEAVE_COUNT_EXPIRE_TIME)
    {
      decrement_count = (t_delta / JOIN_LEAVE_COUNT_EXPIRE_TIME);

      if (decrement_count > source_p->connection->join_leave_count)
        source_p->connection->join_leave_count = 0;
      else
        source_p->connection->join_leave_count -= decrement_count;
    }
    else
    {
      if ((CurrentTime - (source_p->connection->last_join_time)) <
          GlobalSetOptions.spam_time)
        source_p->connection->join_leave_count++;  /* It's a possible spambot */
    }

    if (name)
      source_p->connection->last_join_time = CurrentTime;
    else
      source_p->connection->last_leave_time = CurrentTime;
  }
}

/*! \brief Compares usercount and servercount against their split
 *         values and adjusts splitmode accordingly
 * \param unused Unused address pointer
 */
void
check_splitmode(void *unused)
{
  if (splitchecking && (ConfigChannel.no_join_on_split ||
                        ConfigChannel.no_create_on_split))
  {
    const unsigned int server = dlink_list_length(&global_server_list);

    if (!splitmode && ((server < split_servers) || (Count.total < split_users)))
    {
      splitmode = 1;

      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "Network split, activating splitmode");
      event_add(&splitmode_event, NULL); 
    }
    else if (splitmode && (server >= split_servers) && (Count.total >= split_users))
    {
      splitmode = 0;

      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "Network rejoined, deactivating splitmode");
      event_delete(&splitmode_event);
    }
  }
}

/*! \brief Sets the channel topic for a certain channel
 * \param chptr      Pointer to struct Channel
 * \param topic      The topic string
 * \param topic_info n!u\@h formatted string of the topic setter
 * \param topicts    Timestamp on the topic
 * \param local      Whether the topic is set by a local client
 */
void
channel_set_topic(struct Channel *chptr, const char *topic,
                  const char *topic_info, time_t topicts, int local)
{
  if (local)
    strlcpy(chptr->topic, topic, IRCD_MIN(sizeof(chptr->topic), ConfigServerInfo.max_topic_length + 1));
  else
    strlcpy(chptr->topic, topic, sizeof(chptr->topic));

  strlcpy(chptr->topic_info, topic_info, sizeof(chptr->topic_info));
  chptr->topic_time = topicts;
}

/* do_join_0()
 *
 * inputs	- pointer to client doing join 0
 * output	- NONE
 * side effects	- Use has decided to join 0. This is legacy
 *		  from the days when channels were numbers not names. *sigh*
 *		  There is a bunch of evilness necessary here due to
 * 		  anti spambot code.
 */
void
channel_do_join_0(struct Client *source_p)
{
  dlink_node *node = NULL, *node_next = NULL;

  if (source_p->channel.head)
    if (MyConnect(source_p) && !HasUMode(source_p, UMODE_OPER))
      check_spambot_warning(source_p, NULL);

  DLINK_FOREACH_SAFE(node, node_next, source_p->channel.head)
  {
    struct Channel *chptr = ((struct Membership *)node->data)->chptr;

    sendto_server(source_p, 0, 0, ":%s PART %s",
                  source_p->id, chptr->name);
    sendto_channel_local(0, chptr, ":%s!%s@%s PART %s",
                         source_p->name, source_p->username,
                         source_p->host, chptr->name);

    remove_user_from_channel(node->data);
  }
}

static char *
channel_find_last0(struct Client *source_p, char *chanlist)
{
  int join0 = 0;

  for (char *p = chanlist; *p; ++p)  /* Find last "JOIN 0" */
  {
    if (*p == '0' && (*(p + 1) == ',' || *(p + 1) == '\0'))
    {
      if (*(p + 1) == ',')
        ++p;

      chanlist = p + 1;
      join0 = 1;
    }
    else
    {
      while (*p != ',' && *p != '\0')  /* Skip past channel name */
        ++p;

      if (*p == '\0')  /* Hit the end */
        break;
    }
  }

  if (join0)
    channel_do_join_0(source_p);

  return chanlist;
}

void
channel_do_join(struct Client *source_p, char *channel, char *key_list)
{
  char *p = NULL;
  char *chan = NULL;
  char *chan_list = NULL;
  struct Channel *chptr = NULL;
  struct MaskItem *conf = NULL;
  const struct ClassItem *const class = get_class_ptr(&source_p->connection->confs);
  int i = 0;
  unsigned int flags = 0;

  chan_list = channel_find_last0(source_p, channel);

  for (chan = strtoken(&p, chan_list, ","); chan;
       chan = strtoken(&p,      NULL, ","))
  {
    const char *key = NULL;

    /* If we have any more keys, take the first for this channel. */
    if (!EmptyString(key_list) && (key_list = strchr(key = key_list, ',')))
      *key_list++ = '\0';

    /* Empty keys are the same as no keys. */
    if (key && *key == '\0')
      key = NULL;

    if (!check_channel_name(chan, 1))
    {
      sendto_one_numeric(source_p, &me, ERR_BADCHANNAME, chan);
      continue;
    }

    if (!IsExemptResv(source_p) &&
        !(HasUMode(source_p, UMODE_OPER) && ConfigGeneral.oper_pass_resv) &&
        ((conf = match_find_resv(chan)) && !resv_find_exempt(source_p, conf)))
    {
      ++conf->count;
      sendto_one_numeric(source_p, &me, ERR_CHANBANREASON,
                         chan, conf->reason ? conf->reason : "Reserved channel");
      sendto_realops_flags(UMODE_REJ, L_ALL, SEND_NOTICE,
                           "Forbidding reserved channel %s from user %s",
                           chan, get_client_name(source_p, HIDE_IP));
      continue;
    }

    if (dlink_list_length(&source_p->channel) >=
        ((class->max_channels) ? class->max_channels : ConfigChannel.max_channels))
    {
      sendto_one_numeric(source_p, &me, ERR_TOOMANYCHANNELS, chan);
      break;
    }

    if ((chptr = hash_find_channel(chan)))
    {
      if (IsMember(source_p, chptr))
        continue;

      if (splitmode && !HasUMode(source_p, UMODE_OPER) &&
          ConfigChannel.no_join_on_split)
      {
        sendto_one_numeric(source_p, &me, ERR_UNAVAILRESOURCE, chptr->name);
        continue;
      }

      /*
       * can_join checks for +i key, bans.
       */
      if ((i = can_join(source_p, chptr, key)))
      {
        sendto_one_numeric(source_p, &me, i, chptr->name);
        continue;
      }

      /*
       * This should never be the case unless there is some sort of
       * persistant channels.
       */
      if (!dlink_list_length(&chptr->members))
        flags = CHFL_CHANOP;
      else
        flags = 0;
    }
    else
    {
      if (splitmode && !HasUMode(source_p, UMODE_OPER) &&
          (ConfigChannel.no_create_on_split || ConfigChannel.no_join_on_split))
      {
        sendto_one_numeric(source_p, &me, ERR_UNAVAILRESOURCE, chan);
        continue;
      }

      flags = CHFL_CHANOP;
      chptr = make_channel(chan);
    }

    if (!HasUMode(source_p, UMODE_OPER))
      check_spambot_warning(source_p, chptr->name);

    add_user_to_channel(chptr, source_p, flags, 1);

    /*
     *  Set timestamp if appropriate, and propagate
     */
    if (flags == CHFL_CHANOP)
    {
      chptr->creationtime = CurrentTime;
      chptr->mode.mode |= MODE_TOPICLIMIT;
      chptr->mode.mode |= MODE_NOPRIVMSGS;

      sendto_server(source_p, 0, 0, ":%s SJOIN %lu %s +nt :@%s",
                    me.id, (unsigned long)chptr->creationtime,
                    chptr->name, source_p->id);

      /*
       * Notify all other users on the new channel
       */
      sendto_channel_local_butone(NULL, CAP_EXTENDED_JOIN, 0, chptr, ":%s!%s@%s JOIN %s %s :%s",
                                  source_p->name, source_p->username,
                                  source_p->host, chptr->name,
                                  (!IsDigit(source_p->account[0]) && source_p->account[0] != '*') ? source_p->account : "*",
                                  source_p->info);
      sendto_channel_local_butone(NULL, 0, CAP_EXTENDED_JOIN, chptr, ":%s!%s@%s JOIN :%s",
                                  source_p->name, source_p->username,
                                  source_p->host, chptr->name);
      sendto_channel_local(0, chptr, ":%s MODE %s +nt",
                           me.name, chptr->name);

      if (source_p->away[0])
        sendto_channel_local_butone(source_p, CAP_AWAY_NOTIFY, 0, chptr,
                                    ":%s!%s@%s AWAY :%s",
                                    source_p->name, source_p->username,
                                    source_p->host, source_p->away);
    }
    else
    {
      sendto_server(source_p, 0, 0, ":%s JOIN %lu %s +",
                    source_p->id, (unsigned long)chptr->creationtime,
                    chptr->name);

      sendto_channel_local_butone(NULL, CAP_EXTENDED_JOIN, 0, chptr, ":%s!%s@%s JOIN %s %s :%s",
                                  source_p->name, source_p->username,
                                  source_p->host, chptr->name,
                                  (!IsDigit(source_p->account[0]) && source_p->account[0] != '*') ? source_p->account : "*",
                                  source_p->info);
      sendto_channel_local_butone(NULL, 0, CAP_EXTENDED_JOIN, chptr, ":%s!%s@%s JOIN :%s",
                                  source_p->name, source_p->username,
                                  source_p->host, chptr->name);

      if (source_p->away[0])
        sendto_channel_local_butone(source_p, CAP_AWAY_NOTIFY, 0, chptr,
                                    ":%s!%s@%s AWAY :%s",
                                    source_p->name, source_p->username,
                                    source_p->host, source_p->away);
    }

    del_invite(chptr, source_p);

    if (chptr->topic[0])
    {
      sendto_one_numeric(source_p, &me, RPL_TOPIC, chptr->name, chptr->topic);
      sendto_one_numeric(source_p, &me, RPL_TOPICWHOTIME, chptr->name,
                         chptr->topic_info, chptr->topic_time);
    }

    channel_member_names(source_p, chptr, 1);

    source_p->connection->last_join_time = CurrentTime;
  }
}

/*! \brief Removes a client from a specific channel
 * \param source_p Pointer to source client to remove
 * \param name     Name of channel to remove from
 * \param reason   Part reason to show
 */
static void
channel_part_one_client(struct Client *source_p, const char *name, const char *reason)
{
  struct Channel *chptr = NULL;
  struct Membership *member = NULL;

  if ((chptr = hash_find_channel(name)) == NULL)
  {
    sendto_one_numeric(source_p, &me, ERR_NOSUCHCHANNEL, name);
    return;
  }

  if ((member = find_channel_link(source_p, chptr)) == NULL)
  {
    sendto_one_numeric(source_p, &me, ERR_NOTONCHANNEL, chptr->name);
    return;
  }

  if (MyConnect(source_p) && !HasUMode(source_p, UMODE_OPER))
    check_spambot_warning(source_p, NULL);

  /*
   * Remove user from the old channel (if any)
   * only allow /part reasons in -m chans
   */
  if (*reason && (!MyConnect(source_p) ||
      ((can_send(chptr, source_p, member, reason) &&
       (source_p->connection->firsttime + ConfigGeneral.anti_spam_exit_message_time)
        < CurrentTime))))
  {
    sendto_server(source_p, 0, 0, ":%s PART %s :%s",
                  source_p->id, chptr->name, reason);
    sendto_channel_local(0, chptr, ":%s!%s@%s PART %s :%s",
                         source_p->name, source_p->username,
                         source_p->host, chptr->name, reason);
  }
  else
  {
    sendto_server(source_p, 0, 0, ":%s PART %s",
                  source_p->id, chptr->name);
    sendto_channel_local(0, chptr, ":%s!%s@%s PART %s",
                         source_p->name, source_p->username,
                         source_p->host, chptr->name);
  }

  remove_user_from_channel(member);
}

void
channel_do_part(struct Client *source_p, char *channel, const char *reason)
{
  char *p = NULL, *name = NULL;
  char buf[KICKLEN + 1] = "";

  if (!EmptyString(reason))
    strlcpy(buf, reason, sizeof(buf));

  for (name = strtoken(&p, channel, ","); name;
       name = strtoken(&p,    NULL, ","))
    channel_part_one_client(source_p, name, buf);
}

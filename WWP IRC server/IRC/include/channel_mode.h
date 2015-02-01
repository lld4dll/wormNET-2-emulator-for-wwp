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

/*! \file channel_mode.h
 * \brief Includes channel mode related definitions and structures.
 * \version $Id$
 */

#ifndef INCLUDED_channel_mode_h
#define INCLUDED_channel_mode_h

#define MODEBUFLEN    200

/* Maximum mode changes allowed per client, per server is different */
#define MAXMODEPARAMS 6

enum
{
  MODE_QUERY =  0,
  MODE_ADD   =  1,
  MODE_DEL   = -1
};

enum
{
  CHACCESS_NOTONCHAN = -1,
  CHACCESS_PEON      =  0,
  CHACCESS_HALFOP    =  1,
  CHACCESS_CHANOP    =  2,
  CHACCESS_REMOTE    =  3
};

/* can_send results */
enum
{
  CAN_SEND_NO    =  0,
  CAN_SEND_NONOP = -1,
  CAN_SEND_OPV   = -2
};


/* Channel related flags */
#define CHFL_CHANOP     0x0001U  /* Channel operator   */
#define CHFL_HALFOP     0x0002U  /* Channel half op    */
#define CHFL_VOICE      0x0004U  /* the power to speak */
#define CHFL_BAN        0x0008U  /* ban channel flag */
#define CHFL_EXCEPTION  0x0010U  /* exception to ban channel flag */
#define CHFL_INVEX      0x0020U

/* channel modes ONLY */
#define MODE_PRIVATE    0x0001U  /**< */
#define MODE_SECRET     0x0002U  /**< Channel does not show up on NAMES or LIST */
#define MODE_MODERATED  0x0004U  /**< Users without +v/h/o cannot send text to the channel */
#define MODE_TOPICLIMIT 0x0008U  /**< Only chanops can change the topic */
#define MODE_INVITEONLY 0x0010U  /**< Only invited usersmay join this channel */
#define MODE_NOPRIVMSGS 0x0020U  /**< Users must be in the channel to send text to it */
#define MODE_SSLONLY    0x0040U  /**< Prevents anyone who isn't connected via SSL/TLS from joining the channel */
#define MODE_OPERONLY   0x0080U  /**< Prevents anyone who hasn't obtained IRC operator status from joining the channel */
#define MODE_REGISTERED 0x0100U  /**< Channel has been registered with ChanServ */
#define MODE_REGONLY    0x0200U  /**< Only registered clients may join a channel with that mode set */
#define MODE_NOCTRL     0x0400U  /**< Prevents users from sending messages containing control codes to the channel */
#define MODE_MODREG     0x0800U  /**< Unregistered/unidentified clients cannot send text to the channel */

/* cache flags for silence on ban */
#define CHFL_BAN_CHECKED  0x0080U
#define CHFL_BAN_SILENCED 0x0100U

/* name invisible */
#define SecretChannel(x)        (((x)->mode.mode & MODE_SECRET))
#define PubChannel(x)           (((x)->mode.mode & (MODE_PRIVATE | MODE_SECRET)) == 0)
/* knock is forbidden, halfops can't kick/deop other halfops. */
#define PrivateChannel(x)       (((x)->mode.mode & MODE_PRIVATE))

struct ChannelMode
{
  void (*func)(struct Client *,
               struct Channel *, int, int *, char **,
               int *, int, int, char, unsigned int);
  unsigned int d;
};

struct ChModeChange
{
  char letter;
  const char *arg;
  const char *id;
  int dir;
};

struct mode_letter
{
  const unsigned int mode;
  const unsigned char letter;
};

extern const struct mode_letter chan_modes[];
extern const struct ChannelMode ModeTable[];

extern int add_id(struct Client *, struct Channel *, char *, unsigned int);
extern void set_channel_mode(struct Client *, struct Channel *,
                             struct Membership *, int, char **);
extern void clear_ban_cache_channel(struct Channel *);
extern void clear_ban_cache_client(struct Client *);
#endif /* INCLUDED_channel_mode_h */

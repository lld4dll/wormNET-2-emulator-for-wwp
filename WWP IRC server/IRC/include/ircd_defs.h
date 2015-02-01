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

/*! \file ircd_defs.h
 * \brief A header for ircd global definitions.
 * \version $Id$
 */

/* ircd_defs.h - Global size definitions for record entries used
 * througout ircd. Please think 3 times before adding anything to this
 * file.
 */
#ifndef INCLUDED_ircd_defs_h
#define INCLUDED_ircd_defs_h
#include "stdinc.h"

/* Right out of the RFC */
#define IRCD_BUFSIZE    512     /* WARNING: *DONT* CHANGE THIS!!!!         */
#define HOSTLEN          63     /* Length of hostname. Updated to comply
                                   with RFC 1123 */
#define NICKLEN         30
#define USERLEN         10
#define SVIDLEN         30
#define PORTNAMELEN 6  /* ":31337" */

#define HOSTIPLEN       45 /* sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") */
#define PASSWDLEN       128
#define IDLEN           12 /* this is the maximum length, not the actual
                              generated length; DO NOT CHANGE! */
#define REALLEN         50
#define CHANNELLEN      50
#define TOPICLEN        300
#define KILLLEN         180
#define REASONLEN       180
#define KICKLEN         180
#define AWAYLEN         180
#define KEYLEN          23

#define USERHOST_REPLYLEN       (NICKLEN+HOSTLEN+USERLEN+5)
#define MAX_DATE_STRING 32      /* maximum string length for a date string */
#define IRCD_MAXNS      3       /* Maximum number of nameservers in
                                   /etc/resolv.conf we care about */

#define LOWEST_SAFE_FD  4  /* skip stdin, stdout, stderr, and profiler */

/* This is to get around the fact that some implementations have ss_len and
 * others do not
 */
struct irc_ssaddr
{
  struct sockaddr_storage ss;
  unsigned char   ss_len;
  in_port_t       ss_port;
};
#endif /* INCLUDED_ircd_defs_h */

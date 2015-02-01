/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 2003-2014 ircd-hybrid development team
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

/*! \file userhost.h
 * \brief A header for global user limits.
 * \version $Id$
 */

#ifndef INCLUDED_userhost_h
#define INCLUDED_userhost_h

struct NameHost
{
  dlink_node node;  /* Point to other names on this hostname */
  char name[USERLEN + 1];
  unsigned int icount;  /* Number of =local= identd on this name*/
  unsigned int gcount;  /* Global user count on this name */
  unsigned int lcount;  /* Local user count on this name */
};

struct UserHost
{
  dlink_list list;  /* List of names on this hostname */
  struct UserHost *next;
  char host[HOSTLEN + 1];
};

extern void count_user_host(const char *, const char *, unsigned int *, unsigned int *, unsigned int *);
extern void add_user_host(const char *, const char *, int);
extern void delete_user_host(const char *, const char *, int);
#endif  /* INCLUDED_userhost_h */

/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 2001-2014 ircd-hybrid development team
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

/*! \file resv.h
 * \brief A header for the RESV functions.
 * \version $Id$
 */

#ifndef INCLUDED_resv_h
#define INCLUDED_resv_h

extern struct MaskItem *create_resv(const char *, const char *, const dlink_list *);
extern int resv_find_exempt(const struct Client *, const struct MaskItem *);
extern struct MaskItem *match_find_resv(const char *);
#endif  /* INCLUDED_resv_h */

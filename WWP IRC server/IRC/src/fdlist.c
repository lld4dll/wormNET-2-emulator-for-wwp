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

/*! \file fdlist.c
 * \brief Maintains a list of file descriptors.
 * \version $Id$
 */

#include "stdinc.h"
#include "fdlist.h"
#include "client.h"  /* struct Client */
#include "event.h"
#include "ircd.h"    /* GlobalSetOptions */
#include "irc_string.h"
#include "s_bsd.h"   /* comm_setselect */
#include "conf.h"  /* ConfigServerInfo */
#include "send.h"
#include "memory.h"
#include "numeric.h"
#include "misc.h"
#include "res.h"

fde_t *fd_hash[FD_HASH_SIZE];
fde_t *fd_next_in_loop = NULL;
int number_fd = LEAKED_FDS;
int hard_fdlimit = 0;


void
fdlist_init(void)
{
  int fdmax;
  struct rlimit limit;

  if (!getrlimit(RLIMIT_NOFILE, &limit))
  {
    limit.rlim_cur = limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limit);
  }

  fdmax = getdtablesize();

  /* allow MAXCLIENTS_MIN clients even at the cost of MAX_BUFFER and
   * some not really LEAKED_FDS */
  fdmax = IRCD_MAX(fdmax, LEAKED_FDS + MAX_BUFFER + MAXCLIENTS_MIN);

  /* under no condition shall this raise over 65536
   * for example user ip heap is sized 2*hard_fdlimit */
  hard_fdlimit = IRCD_MIN(fdmax, 65536);
}

static inline unsigned int
hash_fd(int fd)
{
  return ((unsigned int)fd) % FD_HASH_SIZE;
}

fde_t *
lookup_fd(int fd)
{
  fde_t *F = fd_hash[hash_fd(fd)];

  while (F)
  {
    if (F->fd == fd)
      return F;
    F = F->hnext;
  }

  return NULL;
}

/* Called to open a given filedescriptor */
void
fd_open(fde_t *F, int fd, int is_socket, const char *desc)
{
  unsigned int hashv = hash_fd(fd);
  assert(fd >= 0);

  F->fd = fd;
  F->comm_index = -1;

  if (desc)
    strlcpy(F->desc, desc, sizeof(F->desc));

  /* Note: normally we'd have to clear the other flags,
   * but currently F is always cleared before calling us.. */
  F->flags.open = 1;
  F->flags.is_socket = is_socket;
  F->hnext = fd_hash[hashv];
  fd_hash[hashv] = F;

  number_fd++;
}

/* Called to close a given filedescriptor */
void
fd_close(fde_t *F)
{
  unsigned int hashv = hash_fd(F->fd);

  if (F == fd_next_in_loop)
    fd_next_in_loop = F->hnext;

  if (F->flags.is_socket)
    comm_setselect(F, COMM_SELECT_WRITE | COMM_SELECT_READ, NULL, NULL, 0);

  delete_resolver_queries(F);

#ifdef HAVE_LIBCRYPTO
  if (F->ssl)
    SSL_free(F->ssl);
#endif

  if (fd_hash[hashv] == F)
    fd_hash[hashv] = F->hnext;
  else
  {
    fde_t *prev;

    /* let it core if not found */
    for (prev = fd_hash[hashv]; prev->hnext != F; prev = prev->hnext)
      ;
    prev->hnext = F->hnext;
  }

  /* Unlike squid, we're actually closing the FD here! -- adrian */
  close(F->fd);
  number_fd--;

  memset(F, 0, sizeof(fde_t));
}

/*
 * fd_dump() - dump the list of active filedescriptors
 */
void
fd_dump(struct Client *source_p, int parc, char *parv[])
{
  for (unsigned int i = 0; i < FD_HASH_SIZE; ++i)
    for (fde_t *F = fd_hash[i]; F; F = F->hnext)
      sendto_one_numeric(source_p, &me, RPL_STATSDEBUG|SND_EXPLICIT,
                         "F :fd %-5d desc '%s'", F->fd, F->desc);
}

/*
 * fd_note() - set the fd note
 *
 * Note: must be careful not to overflow fd_table[fd].desc when
 *       calling.
 */
void
fd_note(fde_t *F, const char *format, ...)
{
  va_list args;

  if (format)
  {
    va_start(args, format);
    vsnprintf(F->desc, sizeof(F->desc), format, args);
    va_end(args);
  }
  else
    F->desc[0] = '\0';
}

/* Make sure stdio descriptors (0-2) and profiler descriptor (3)
 * always go somewhere harmless.  Use -foreground for profiling
 * or executing from gdb */
void
close_standard_fds(void)
{
  for (unsigned int i = 0; i < LOWEST_SAFE_FD; ++i)
  {
    close(i);

    if (open("/dev/null", O_RDWR) < 0)
      exit(-1); /* we're hosed if we can't even open /dev/null */
  }
}

void
close_fds(fde_t *one)
{
  for (unsigned int i = 0; i < FD_HASH_SIZE; ++i)
    for (fde_t *F = fd_hash[i]; F; F = F->hnext)
      if (F != one)
        close(F->fd);
}

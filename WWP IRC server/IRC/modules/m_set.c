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

/*! \file m_set.c
 * \brief Includes required functions for processing the SET command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "event.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "send.h"
#include "conf.h"
#include "parse.h"
#include "modules.h"
#include "misc.h"


/* SET AUTOCONN */
static void
quote_autoconn(struct Client *source_p, const char *arg, int newval)
{
  if (!EmptyString(arg))
  {
    struct MaskItem *conf = find_exact_name_conf(CONF_SERVER, NULL, arg, NULL, NULL);

    if (conf)
    {
      if (newval)
        SetConfAllowAutoConn(conf);
      else
        ClearConfAllowAutoConn(conf);

      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "%s has changed AUTOCONN for %s to %i",
                           get_oper_name(source_p), arg, newval);
      sendto_one_notice(source_p, &me, ":AUTOCONN for %s is now set to %i",
                        arg, newval);
    }
    else
      sendto_one_notice(source_p, &me, ":Cannot find %s", arg);
  }
  else
    sendto_one_notice(source_p, &me, ":Please specify a server name!");
}

/* SET AUTOCONNALL */
static void
quote_autoconnall(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed AUTOCONNALL to %i",
                         get_oper_name(source_p), newval);

    GlobalSetOptions.autoconn = newval;
  }
  else
    sendto_one_notice(source_p, &me, ":AUTOCONNALL is currently %i",
                      GlobalSetOptions.autoconn);
}

/* SET FLOODCOUNT */
static void
quote_floodcount(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    GlobalSetOptions.floodcount = newval;
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed FLOODCOUNT to %i",
                         get_oper_name(source_p), GlobalSetOptions.floodcount);
  }
  else
    sendto_one_notice(source_p, &me, ":FLOODCOUNT is currently %i",
                      GlobalSetOptions.floodcount);
}

/* SET IDENTTIMEOUT */
static void
quote_identtimeout(struct Client *source_p, const char *arg, int newval)
{
  if (!HasUMode(source_p, UMODE_ADMIN))
  {
    sendto_one_numeric(source_p, &me, ERR_NOPRIVS, "set");
    return;
  }

  if (newval > 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed IDENTTIMEOUT to %d",
                         get_oper_name(source_p), newval);
    GlobalSetOptions.ident_timeout = newval;
  }
  else
    sendto_one_notice(source_p, &me, ":IDENTTIMEOUT is currently %d",
                      GlobalSetOptions.ident_timeout);
}

/* SET MAX */
static void
quote_max(struct Client *source_p, const char *arg, int newval)
{
  if (newval > 0)
  {
    if (newval > MAXCLIENTS_MAX)
    {
      sendto_one_notice(source_p, &me, ":You cannot set MAXCLIENTS to > %d, restoring to %d",
                        MAXCLIENTS_MAX, ConfigServerInfo.max_clients);
      return;
    }

    if (newval < MAXCLIENTS_MIN)
    {
      sendto_one_notice(source_p, &me, ":You cannot set MAXCLIENTS to < %d, restoring to %d",
                        MAXCLIENTS_MIN, ConfigServerInfo.max_clients);
      return;
    }

    ConfigServerInfo.max_clients = newval;

    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
        "%s set new MAXCLIENTS to %d (%d current)",
        get_oper_name(source_p), ConfigServerInfo.max_clients, Count.local);
  }
  else
    sendto_one_notice(source_p, &me, ":Current MAXCLIENTS = %d (%d)",
                      ConfigServerInfo.max_clients, Count.local);
}

/* SET SPAMNUM */
static void
quote_spamnum(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    if (newval == 0)
    {
      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "%s has disabled ANTI_SPAMBOT", source_p->name);
      GlobalSetOptions.spam_num = newval;
      return;
    }

    GlobalSetOptions.spam_num = IRCD_MAX(newval, MIN_SPAM_NUM);
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed SPAMNUM to %i",
                         get_oper_name(source_p), GlobalSetOptions.spam_num);
  }
  else
    sendto_one_notice(source_p, &me, ":SPAMNUM is currently %i",
                      GlobalSetOptions.spam_num);
}

/* SET SPAMTIME */
static void
quote_spamtime(struct Client *source_p, const char *arg, int newval)
{
  if (newval > 0)
  {
    GlobalSetOptions.spam_time = IRCD_MAX(newval, MIN_SPAM_TIME);
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed SPAMTIME to %i",
                         get_oper_name(source_p), GlobalSetOptions.spam_time);
  }
  else
    sendto_one_notice(source_p, &me, ":SPAMTIME is currently %i",
                      GlobalSetOptions.spam_time);
}

/* this table is what splitmode may be set to */
static const char *splitmode_values[] =
{
  "OFF",
  "ON",
  "AUTO",
  NULL
};

/* this table is what splitmode may be */
static const char *splitmode_status[] =
{
  "OFF",
  "AUTO (OFF)",
  "ON",
  "AUTO (ON)",
  NULL
};

/* SET SPLITMODE */
static void
quote_splitmode(struct Client *source_p, const char *charval, int val)
{
  if (charval)
  {
    int newval;

    for (newval = 0; splitmode_values[newval]; ++newval)
      if (!irccmp(splitmode_values[newval], charval))
        break;

    /* OFF */
    if (newval == 0)
    {
      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "%s is disabling splitmode",
                           get_oper_name(source_p));

      splitmode = 0;
      splitchecking = 0;

      event_delete(&splitmode_event);
    }
    /* ON */
    else if (newval == 1)
    {
      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "%s is enabling and activating splitmode",
                           get_oper_name(source_p));

      splitmode = 1;
      splitchecking = 0;

      /* we might be deactivating an automatic splitmode, so pull the event */
      event_delete(&splitmode_event);
    }
    /* AUTO */
    else if (newval == 2)
    {
      sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                           "%s is enabling automatic splitmode",
                           get_oper_name(source_p));

      splitchecking = 1;
      check_splitmode(NULL);
    }
  }
  else
    /* if we add splitchecking to splitmode*2 we get a unique table to
     * pull values back out of, splitmode can be four states - but you can
     * only set to three, which means we cant use the same table --fl_
     */
    sendto_one_notice(source_p, &me, ":SPLITMODE is currently %s",
                      splitmode_status[(splitchecking + (splitmode * 2))]);
}

/* SET SPLITNUM */
static void
quote_splitnum(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed SPLITNUM to %i",
                         get_oper_name(source_p), newval);
    split_servers = newval;

    if (splitchecking)
      check_splitmode(NULL);
  }
  else
    sendto_one_notice(source_p, &me, ":SPLITNUM is currently %i",
                      split_servers);
}

/* SET SPLITUSERS */
static void
quote_splitusers(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed SPLITUSERS to %i",
                         get_oper_name(source_p), newval);
    split_users = newval;

    if (splitchecking)
      check_splitmode(NULL);
  }
  else
    sendto_one_notice(source_p, &me, ":SPLITUSERS is currently %i",
                      split_users);
}

/* SET JFLOODTIME */
static void
quote_jfloodtime(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed JFLOODTIME to %i",
                         get_oper_name(source_p), newval);
    GlobalSetOptions.joinfloodtime = newval;
  }
  else
    sendto_one_notice(source_p, &me, ":JFLOODTIME is currently %i",
                      GlobalSetOptions.joinfloodtime);
}

/* SET JFLOODCOUNT */
static void
quote_jfloodcount(struct Client *source_p, const char *arg, int newval)
{
  if (newval >= 0)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "%s has changed JFLOODCOUNT to %i",
                         get_oper_name(source_p), newval);
    GlobalSetOptions.joinfloodcount = newval;
  }
  else
    sendto_one_notice(source_p, &me, ":JFLOODCOUNT is currently %i",
                      GlobalSetOptions.joinfloodcount);
}

/* Structure used for the SET table itself */
struct SetStruct
{
  const char *name;
  void (*handler)(struct Client *, const char *, int);
  const unsigned int wants_char;  /* 1 if it expects (char *, [int]) */
  const unsigned int wants_int;  /* 1 if it expects ([char *], int) */
  /* eg:  0, 1 == only an int arg
   * eg:  1, 1 == char and int args */
};

/*
 * If this ever needs to be expanded to more than one arg of each
 * type, want_char/want_int could be the count of the arguments,
 * instead of just a boolean flag...
 *
 * -davidt
 */
static const struct SetStruct set_cmd_table[] =
{
  /* name               function        string arg  int arg */
  /* -------------------------------------------------------- */
  { "AUTOCONN",         quote_autoconn,         1,      1 },
  { "AUTOCONNALL",      quote_autoconnall,      0,      1 },
  { "FLOODCOUNT",       quote_floodcount,       0,      1 },
  { "IDENTTIMEOUT",     quote_identtimeout,     0,      1 },
  { "MAX",              quote_max,              0,      1 },
  { "SPAMNUM",          quote_spamnum,          0,      1 },
  { "SPAMTIME",         quote_spamtime,         0,      1 },
  { "SPLITMODE",        quote_splitmode,        1,      0 },
  { "SPLITNUM",         quote_splitnum,         0,      1 },
  { "SPLITUSERS",       quote_splitusers,       0,      1 },
  { "JFLOODTIME",       quote_jfloodtime,       0,      1 },
  { "JFLOODCOUNT",      quote_jfloodcount,      0,      1 },
  /* -------------------------------------------------------- */
  { NULL,               NULL,                   0,      0 }
};

/*
 * list_quote_commands() sends the client all the available commands.
 * Four to a line for now.
 */
static void
list_quote_commands(struct Client *source_p)
{
  unsigned int j = 0;
  const char *names[4] = { "", "", "", "" };

  sendto_one_notice(source_p, &me, ":Available QUOTE SET commands:");

  for (const struct SetStruct *tab = set_cmd_table; tab->handler; ++tab)
  {
    names[j++] = tab->name;

    if (j > 3)
    {
      sendto_one_notice(source_p, &me, ":%s %s %s %s",
                        names[0], names[1],
                        names[2], names[3]);
      j = 0;
      names[0] = names[1] = names[2] = names[3] = "";
    }
  }

  if (j)
    sendto_one_notice(source_p, &me, ":%s %s %s %s",
                      names[0], names[1],
                      names[2], names[3]);
}

/*
 * mo_set - SET command handler
 * set options while running
 */
static int
mo_set(struct Client *source_p, int parc, char *parv[])
{
  int n;
  int newval;
  const char *strarg = NULL;
  const char *intarg = NULL;

  if (!HasOFlag(source_p, OPER_FLAG_SET))
  {
    sendto_one_numeric(source_p, &me, ERR_NOPRIVS, "set");
    return 0;
  }

  if (parc > 1)
  {
    /*
     * Go through all the commands in set_cmd_table, until one is
     * matched.
     */
    for (const struct SetStruct *tab = set_cmd_table; tab->handler; ++tab)
    {
      if (irccmp(tab->name, parv[1]))
        continue;

      /*
       * Command found; now execute the code
       */
      n = 2;

      if (tab->wants_char)
        strarg = parv[n++];

      if (tab->wants_int)
        intarg = parv[n++];

      if ((n - 1) > parc)
        sendto_one_notice(source_p, &me, ":SET %s expects (\"%s%s\") args", tab->name,
                          (tab->wants_char ? "string, " : ""),
                          (tab->wants_int ? "int" : ""));

      if (parc <= 2)
      {
        strarg = NULL;
        intarg = NULL;
      }

      if (tab->wants_int && parc > 2)
      {
        if (intarg)
        {
          if (!irccmp(intarg, "yes") || !irccmp(intarg, "on"))
            newval = 1;
          else if (!irccmp(intarg, "no") || !irccmp(intarg, "off"))
            newval = 0;
          else
            newval = atoi(intarg);
        }
        else
          newval = -1;

        if (newval < 0)
        {
          sendto_one_notice(source_p, &me, ":Value less than 0 illegal for %s",
                            tab->name);
          return 0;
        }
      }
      else
        newval = -1;

      tab->handler(source_p, strarg, newval);
      return 0;
    }

    /*
     * Code here will be executed when a /QUOTE SET command is not
     * found within set_cmd_table.
     */
    sendto_one_notice(source_p, &me, ":Variable not found.");
    return 0;
  }

  list_quote_commands(source_p);
  return 0;
}

static struct Message set_msgtab =
{
  "SET", NULL, 0, 0, 0, MAXPARA, MFLG_SLOW, 0,
  { m_unregistered, m_not_oper, m_ignore, m_ignore, mo_set, m_ignore }
};

static void
module_init(void)
{
  mod_add_cmd(&set_msgtab);
}

static void
module_exit(void)
{
  mod_del_cmd(&set_msgtab);
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

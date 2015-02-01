/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 2000-2014 ircd-hybrid development team
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

/*! \file modules.c
 * \brief A module loader.
 * \version $Id$
 */

#include "ltdl.h"

#include "stdinc.h"
#include "list.h"
#include "modules.h"
#include "log.h"
#include "ircd.h"
#include "client.h"
#include "send.h"
#include "conf.h"
#include "numeric.h"
#include "parse.h"
#include "ircd_defs.h"
#include "irc_string.h"
#include "memory.h"


static dlink_list modules_list;
static dlink_list modules_path;
static dlink_list modules_conf;

static const char *const unknown_ver = "<unknown>";
static const char *const core_module_table[] =
{
  "m_bmask.la",
  "m_die.la",
  "m_error.la",
  "m_join.la",
  "m_kick.la",
  "m_kill.la",
  "m_message.la",
  "m_mode.la",
  "m_nick.la",
  "m_part.la",
  "m_quit.la",
  "m_server.la",
  "m_sjoin.la",
  "m_squit.la",
  "m_tmode.la",
  NULL
};


dlink_list *
modules_get_list(void)
{
  return &modules_list;
}

int
modules_valid_suffix(const char *name)
{
  return ((name = strrchr(name, '.'))) && !strcmp(name, ".la");
}

/* unload_one_module()
 *
 * inputs       - name of module to unload
 *              - 1 to say modules unloaded, 0 to not
 * output       - 0 if successful, -1 if error
 * side effects - module is unloaded
 */
int
unload_one_module(const char *name, int warn)
{
  struct module *modp = NULL;

  if ((modp = findmodule_byname(name)) == NULL)
    return -1;

  if (modp->modexit)
   modp->modexit();

  assert(dlink_list_length(&modules_list) > 0);
  dlinkDelete(&modp->node, &modules_list);
  MyFree(modp->name);

  lt_dlclose(modp->handle);

  if (warn == 1)
  {
    ilog(LOG_TYPE_IRCD, "Module %s unloaded", name);
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "Module %s unloaded", name);
  }

  return 0;
}

/* load_a_module()
 *
 * inputs       - path name of module, int to notice, int of core
 * output       - -1 if error 0 if success
 * side effects - loads a module if successful
 */
int
load_a_module(const char *path, int warn)
{
  lt_dlhandle tmpptr = NULL;
  const char *mod_basename = NULL;
  struct module *modp = NULL;

  if (findmodule_byname((mod_basename = libio_basename(path))))
    return 1;

  if (!(tmpptr = lt_dlopen(path)))
  {
    const char *err = ((err = lt_dlerror())) ? err : "<unknown>";

    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "Error loading module %s: %s",
                         mod_basename, err);
    ilog(LOG_TYPE_IRCD, "Error loading module %s: %s", mod_basename, err);
    return -1;
  }

  if ((modp = lt_dlsym(tmpptr, "module_entry")) == NULL)
  {
    const char *err = ((err = lt_dlerror())) ? err : "<unknown>";

    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "Error loading module %s: %s",
                         mod_basename, err);
    ilog(LOG_TYPE_IRCD, "Error loading module %s: %s", mod_basename, err);
    lt_dlclose(tmpptr);
    return -1;
  }

  modp->handle = tmpptr;

  if (EmptyString(modp->version))
    modp->version = unknown_ver;

  modp->name = xstrdup(mod_basename);
  dlinkAdd(modp, &modp->node, &modules_list);

  if (modp->modinit)
    modp->modinit();

  if (warn == 1)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                         "Module %s [version: %s handle: %p] loaded.",
                         modp->name, modp->version, tmpptr);
    ilog(LOG_TYPE_IRCD, "Module %s [version: %s handle: %p] loaded.",
         modp->name, modp->version, tmpptr);
  }

  return 0;
}

/*
 * modules_init
 *
 * input	- NONE
 * output	- NONE
 * side effects	- The basic module manipulation modules are loaded
 */
void
modules_init(void)
{
  if (lt_dlinit())
  {
    ilog(LOG_TYPE_IRCD, "Couldn't initialize the libltdl run time dynamic"
         " link library. Exiting.");
    exit(0);
  }
}

/* mod_find_path()
 *
 * input	- path
 * output	- none
 * side effects - returns a module path from path
 */
static struct module_path *
mod_find_path(const char *path)
{
  dlink_node *node = NULL;

  DLINK_FOREACH(node, modules_path.head)
  {
    struct module_path *mpath = node->data;

    if (!strcmp(path, mpath->path))
      return mpath;
  }

  return NULL;
}

/* mod_add_path()
 *
 * input	- path
 * output	- NONE
 * side effects - adds path to list
 */
void
mod_add_path(const char *path)
{
  struct module_path *pathst;

  if (mod_find_path(path))
    return;

  pathst = MyCalloc(sizeof(struct module_path));

  strlcpy(pathst->path, path, sizeof(pathst->path));
  dlinkAdd(pathst, &pathst->node, &modules_path);
}

/* add_conf_module
 *
 * input	- module name
 * output	- NONE
 * side effects - adds module to conf_mod
 */
void
add_conf_module(const char *name)
{
  struct module_path *pathst;

  pathst = MyCalloc(sizeof(struct module_path));

  strlcpy(pathst->path, name, sizeof(pathst->path));
  dlinkAdd(pathst, &pathst->node, &modules_conf);
}

/* mod_clear_paths()
 *
 * input	- NONE
 * output	- NONE
 * side effects - clear the lists of paths and conf modules
 */
void
mod_clear_paths(void)
{
  dlink_node *node = NULL, *node_next = NULL;

  DLINK_FOREACH_SAFE(node, node_next, modules_path.head)
  {
    dlinkDelete(node, &modules_path);
    MyFree(node->data);
  }

  DLINK_FOREACH_SAFE(node, node_next, modules_conf.head)
  {
    dlinkDelete(node, &modules_conf);
    MyFree(node->data);
  }
}

/* findmodule_byname
 *
 * input        - name of module
 * output       - NULL if not found or pointer to module
 * side effects - NONE
 */
struct module *
findmodule_byname(const char *name)
{
  dlink_node *node = NULL;

  DLINK_FOREACH(node, modules_list.head)
  {
    struct module *modp = node->data;

    if (!strcmp(modp->name, name))
      return modp;
  }

  return NULL;
}

/* load_all_modules()
 *
 * input        - int flag warn
 * output       - NONE
 * side effects - load all modules found in autoload directory
 */
void
load_all_modules(int warn)
{
  DIR *system_module_dir = NULL;
  struct dirent *ldirent = NULL;
  char module_fq_name[HYB_PATH_MAX + 1];

  if ((system_module_dir = opendir(AUTOMODPATH)) == NULL)
  {
    ilog(LOG_TYPE_IRCD, "Could not load modules from %s: %s",
         AUTOMODPATH, strerror(errno));
    return;
  }

  while ((ldirent = readdir(system_module_dir)))
  {
    if (modules_valid_suffix(ldirent->d_name))
    {
       snprintf(module_fq_name, sizeof(module_fq_name), "%s/%s",
                AUTOMODPATH, ldirent->d_name);
       load_a_module(module_fq_name, warn);
    }
  }

  closedir(system_module_dir);
}

/* load_conf_modules()
 *
 * input        - NONE
 * output       - NONE
 * side effects - load modules given in ircd.conf
 */
void
load_conf_modules(void)
{
  dlink_node *node = NULL;

  DLINK_FOREACH(node, modules_conf.head)
  {
    struct module_path *mpath = node->data;

    if (findmodule_byname(mpath->path) == NULL)
      load_one_module(mpath->path);
  }
}

/* load_core_modules()
 *
 * input        - int flag warn
 * output       - NONE
 * side effects - core modules are loaded, if any fail, kill ircd
 */
void
load_core_modules(int warn)
{
  char module_name[HYB_PATH_MAX + 1];

  for (unsigned int i = 0; core_module_table[i]; ++i)
  {
    snprintf(module_name, sizeof(module_name), "%s%s",
             MODPATH, core_module_table[i]);

    if (load_a_module(module_name, warn) == -1)
    {
      ilog(LOG_TYPE_IRCD, "Error loading core module %s: terminating ircd",
           core_module_table[i]);
      exit(EXIT_FAILURE);
    }
  }
}

/* load_one_module()
 *
 * input        - pointer to path
 *		- flagged as core module or not
 * output       - -1 if error
 * side effects - module is loaded if found.
 */
int
load_one_module(const char *name)
{
  dlink_node *node = NULL;
  char path[HYB_PATH_MAX + 1];
  struct stat statbuf;

  DLINK_FOREACH(node, modules_path.head)
  {
    const struct module_path *mpath = node->data;

    snprintf(path, sizeof(path), "%s/%s", mpath->path, name);

    if (!modules_valid_suffix(name))
      continue;

    if (strstr(path, "../") == NULL &&
        strstr(path, "/..") == NULL)
      if (!stat(path, &statbuf))
        if (S_ISREG(statbuf.st_mode))  /* Regular files only please */
          return load_a_module(path, 1);
  }

  sendto_realops_flags(UMODE_ALL, L_ALL, SEND_NOTICE,
                       "Cannot locate module %s", name);
  ilog(LOG_TYPE_IRCD, "Cannot locate module %s", name);
  return -1;
}

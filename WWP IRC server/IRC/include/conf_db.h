/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 1996-2009 by Andrew Church <achurch@achurch.org>
 *  Copyright (c) 2012-2014 ircd-hybrid development team
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

/*! \file conf_db.h
 * \brief Includes file utilities for database handling
 * \version $Id$
 */

#ifndef DATAFILES_H
#define DATAFILES_H

struct dbFILE
{
  char mode;  /**< 'r' for reading, 'w' for writing */
  FILE *fp;  /**< The file pointer itself */
  char filename[HYB_PATH_MAX + 1];  /**< Name of the database file */
  char tempname[HYB_PATH_MAX + 1];  /**< Name of the temporary file (for writing) */
};

extern void check_file_version(struct dbFILE *);
extern uint32_t get_file_version(struct dbFILE *);
extern int write_file_version(struct dbFILE *, uint32_t);

extern struct dbFILE *open_db(const char *, const char *, uint32_t);
extern void restore_db(struct dbFILE *);  /* Restore to state before open_db() */
extern int close_db(struct dbFILE *);
extern void backup_databases(void);

#define read_db(f,buf,len)      (fread((buf),1,(len),(f)->fp))
#define write_db(f,buf,len)     (fwrite((buf),1,(len),(f)->fp))
#define getc_db(f)              (fgetc((f)->fp))

extern int read_uint8(uint8_t *, struct dbFILE *);
extern int write_uint8(uint8_t, struct dbFILE *);
extern int read_uint16(uint16_t *, struct dbFILE *);
extern int write_uint16(uint16_t, struct dbFILE *);
extern int read_uint32(uint32_t *, struct dbFILE *);
extern int write_uint32(uint32_t, struct dbFILE *);
extern int read_uint64(uint64_t *, struct dbFILE *);
extern int write_uint64(uint64_t, struct dbFILE *);
extern int read_ptr(void **, struct dbFILE *);
extern int write_ptr(const void *, struct dbFILE *);
extern int read_string(char **, struct dbFILE *);
extern int write_string(const char *, struct dbFILE *);

extern void load_kline_database(void);
extern void save_kline_database(void);
extern void load_dline_database(void);
extern void save_dline_database(void);
extern void load_gline_database(void);
extern void save_gline_database(void);
extern void load_xline_database(void);
extern void save_xline_database(void);
extern void load_resv_database(void);
extern void save_resv_database(void);
extern void save_all_databases(void *);

#define read_buffer(buf,f)      (read_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define write_buffer(buf,f)     (write_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define read_buflen(buf,len,f)  (read_db((f),(buf),(len)) == (len))
#define write_buflen(buf,len,f) (write_db((f),(buf),(len)) == (len))
#define read_variable(var,f)    (read_db((f),&(var),sizeof(var)) == sizeof(var))
#define write_variable(var,f)   (write_db((f),&(var),sizeof(var)) == sizeof(var))

#define DATABASE_UPDATE_TIMEOUT 300
#define KLINE_DB_VERSION 1
#endif

/****************************************************************************
 * apps/include/netutils/ftpc.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __APPS_INCLUDE_NETUTILS_FTPC_H
#define __APPS_INCLUDE_NETUTILS_FTPC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

#ifndef CONFIG_FTP_DEFTIMEO
#  define CONFIG_FTP_DEFTIMEO 30
#endif

#ifndef CONFIG_FTP_ANONPWD
#  define CONFIG_FTP_ANONPWD ""
#endif

#ifndef CONFIG_FTP_DEFPORT
#  define CONFIG_FTP_DEFPORT 21
#endif

#ifndef CONFIG_FTP_MAXREPLY
#  define CONFIG_FTP_MAXREPLY 256
#endif

#ifndef CONFIG_FTP_TMPDIR
#  define CONFIG_FTP_TMPDIR "/tmp"
#endif

#ifndef CONFIG_FTP_BUFSIZE
#  define CONFIG_FTP_BUFSIZE 1024
#endif

#ifndef CONFIG_FTP_MAXPATH
#  define CONFIG_FTP_MAXPATH 256
#endif

#ifndef CONFIG_FTP_SIGNAL
#  define CONFIG_FTP_SIGNAL SIGUSR1
#endif

/* Interface arguments ******************************************************/

/* These definitions describe how a put operation should be performed */

#define FTPC_PUT_NORMAL      0  /* Just PUT the file on the server */
#define FTPC_PUT_APPEND      1  /* Append file to an existing file on the server */
#define FTPC_PUT_UNIQUE      2  /* Create a uniquely named file on the server */
#define FTPC_PUT_RESUME      3  /* Resume a previously started PUT transfer */

/* These definitions describe how a get operation should be performed */

#define FTPC_GET_NORMAL      0  /* Just GET the file from the server */
#define FTPC_GET_APPEND      1  /* Append new file to an existing file */
#define FTPC_GET_RESUME      3  /* Resume a previously started GET transfer */

/* Transfer mode encoding */

#define FTPC_XFRMODE_UNKNOWN  0 /* Nothing has been transferred yet */
#define FTPC_XFRMODE_ASCII    1 /* Last transfer was ASCII mode */
#define FTPC_XFRMODE_BINARY   2 /* Last transfer was binary mode */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This "handle" describes the FTP session */

typedef FAR void *SESSION;

/* This union provides socket address to connect to a host FTP server.
 *
 * in4, in6 - The IPv4 or IPv6 address of the FTP server (or the proxy)
 *            for the FTP server.
 */

union ftpc_sockaddr_u
{
  uint8_t                    raw[sizeof(struct sockaddr_storage)];
  struct sockaddr_storage    ss;
  struct sockaddr            sa;
#ifdef CONFIG_NET_IPv6
  struct sockaddr_in6        in6;
#endif
#ifdef CONFIG_NET_IPv4
  struct sockaddr_in         in4;
#endif
};

/* This structure provides FTP login information */

struct ftpc_login_s
{
  FAR const char *uname;   /* Login uname */
  FAR const char *pwd;     /* Login pwd  */
  FAR const char *rdir;    /* Initial remote directory */
  bool            pasv;    /* true: passive connection mode */
};

/* This structure describes one simple directory listing.  The directory
 * list container as well the individual filename strings are allocated.
 * The number of names in the actual allocated array is variable, given
 * by the nnames field.
 *
 * Since the structure and file names are allocated, they must be freed
 * by calling ftpc_dirfree() when they are no longer needed.  Allocated
 * name strings may be "stolen" from the array but the pointer int the
 * array should be nullified so that the string is not freed by
 * ftpc_dirfree().
 */

struct ftpc_dirlist_s
{
  unsigned int    nnames;  /* Number of entries in name[] array */
  FAR char       *name[1]; /* Filename with absolute path */
};

#define SIZEOF_FTPC_DIRLIST(n) \
  (sizeof(struct ftpc_dirlist_s) + ((n)-1)*sizeof(FAR char *))

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* Connection management ****************************************************/

SESSION ftpc_connect(FAR union ftpc_sockaddr_u *server);
void ftpc_disconnect(SESSION handle);

/* FTP commands *************************************************************/

int ftpc_login(SESSION handle, FAR struct ftpc_login_s *login);
int ftpc_quit(SESSION handle);

int ftpc_chdir(SESSION handle, FAR const char *path);
FAR char *ftpc_rpwd(SESSION handle);
int ftpc_cdup(SESSION handle);
int ftpc_mkdir(SESSION handle, FAR const char *path);
int ftpc_rmdir(SESSION handle, FAR const char *path);

int ftpc_unlink(SESSION handle, FAR const char *path);
int ftpc_chmod(SESSION handle, FAR const char *path, FAR const char *mode);
int ftpc_rename(SESSION handle, FAR const char *oldname,
                FAR const char *newname);
off_t ftpc_filesize(SESSION handle, FAR const char *path);
time_t ftpc_filetime(SESSION handle, FAR const char *filename);

int ftpc_idle(SESSION handle, unsigned int idletime);
int ftpc_noop(SESSION handle);
int ftpc_help(SESSION handle, FAR const char *arg);

/* Directory listings *******************************************************/

FAR struct ftpc_dirlist_s *ftpc_listdir(SESSION handle,
                                        FAR const char *dirpath);
void ftpc_dirfree(FAR struct ftpc_dirlist_s *dirlist);

/* File transfers ***********************************************************/

int ftpc_getfile(SESSION handle, FAR const char *rname,
                 FAR const char *lname, uint8_t how, uint8_t xfrmode);
int ftp_putfile(SESSION handle, FAR const char *lname,
                FAR const char *rname, uint8_t how, uint8_t xfrmode);

/* FTP response *************************************************************/

FAR char *ftpc_response(SESSION handle);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_FTPC_H */

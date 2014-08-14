// pam_rd.c : The PAM authentication module of Rivendell
//
// (C) Copyright 2006 Federico Grau <donfede@casagrau.org> <grauf@rfa.org>
//
//      $Id: pam_rd.cpp,v 1.10 2010/09/10 18:16:27 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// created 2006-06-16 grauf@rfa.org
//

// ideas borrowed from:
//  pam_permit.c from Linux-PAM 
//      by Andrew Morgan <morgan@parc.power.net> 1996/3/11
//  pam_deny.c from Linux-PAM 
//      by Andrew Morgan <morgan@parc.power.net> 1996/3/11
//  pam_mysql.c 
//      by James O'Kane <jo2y@midnightlinux.com> and others
//  pam_winbind.c 
//      by Andrew Bartlett <abartlet@samba.org> 2002 and others
//  Rivendell by Fred Gleason <fredg@paravelsystems.com>

/* 
 * include and definitions 
 */
#define PAM_SM_AUTH

#include <security/pam_modules.h>
/* note: _pam_macros.h is for _drop_reply() macros.  This is specific to
 * Linux-PAM and not generally distributed by Sun. */
#include <security/_pam_macros.h>

#include <syslog.h>
#define SYSLOG_IDENT    "pam_rd"
#define SYSLOG_FACILITY LOG_AUTHPRIV

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <errno.h>

#include <qapplication.h>
#include <qsqldatabase.h>
#include <rd.h>
#include <rdconfig.h>
#include <rdcheck_daemons.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rduser.h>

// The pam module name, this should be unique amongst pam modules.
#define MODULE_NAME SYSLOG_IDENT



/* 
 * utility functions and related definitions 
 */

#define PAM_RD_DEBUG (1<<0)
#define PAM_RD_TRY_FIRST_PASS (1<<1)
#define PAM_RD_USE_FIRST_PASS (1<<2)
#define PAM_RD_KILL_RD_DAEMONS (1<<3)
#define PAM_RD_IGNORE_PASS (1<<4)
#define PAM_RD_FAIL_DEFAULT_USER (1<<5)

/* utility function to parse module arguements and return an int with bits set
 * to corresponding options  */
static int parse_args(int argc, const char **argv, char **tmp_user)
{
    int ctrl;
    /* step through arguments */
    for (ctrl = 0; argc-- > 0; ++argv) {
        /* generic options */
        if (!strcmp(*argv,"debug")) {
            ctrl |= PAM_RD_DEBUG;
        }
        else if (!strcasecmp(*argv, "use_first_pass")) {
            ctrl |= PAM_RD_USE_FIRST_PASS;
        }
        else if (!strcasecmp(*argv, "try_first_pass")) {
            ctrl |= PAM_RD_TRY_FIRST_PASS;
        }
        /* pam_rd specific options */
        else if (!strcasecmp(*argv, "kill_rd_daemons")) {
            /* kill any previously running rivendell daemons */
            ctrl |= PAM_RD_KILL_RD_DAEMONS;
        }
        else if (!strcasecmp(*argv, "ignore_pass")) {
            /* log the user into rivendell, ignoring any password check.  the
             * idea is to "trust" the network logon credentials and ignore the
             * rivendell credentials */
            ctrl |= PAM_RD_IGNORE_PASS;
        }
        else if (!strncasecmp(*argv, "fail_default_user=", 18)) {
            /* the Rivendell user account is set to the the user specified by
             * this option if there is a failure authenticating */
            *tmp_user = strdup(18+*argv);
            if (*tmp_user != NULL) {
syslog(LOG_NOTICE, "DEBUG ... read fail_default_user|%s|", *tmp_user);
                ctrl |= PAM_RD_FAIL_DEFAULT_USER;
            } else {
                syslog(LOG_ERR, 
                "parse_args(): invalid fail_default_user specified - ignored");
            }
        } else {
            syslog(LOG_ERR, "parse_args(): ignoring unknown option; %s", *argv);
        }
    }
    return ctrl;
}


/* utility function to be used for PAM conversation callback */
int converse(pam_handle_t *pamh, 
             int nargs,
             struct pam_message **message,
             struct pam_response **response)
{
    int retval;
    struct pam_conv *conv;

    retval = pam_get_item(pamh, PAM_CONV, (const void **) &conv); 
    if (retval == PAM_SUCCESS) {
        retval = conv->conv(nargs,
                            (const struct pam_message **) message,
                            response, conv->appdata_ptr);
        if ((retval != PAM_SUCCESS) && (retval != PAM_CONV_AGAIN))
            syslog(LOG_ERR, "conversation failure [%s]",
                   pam_strerror(pamh, retval));
    } else {
        syslog(LOG_ERR, "couldn't obtain coversation function [%s]",
               pam_strerror(pamh, retval));
    }

    return retval;  /* propagate error status */
}


/* utility function to setup PAM conversation callback */
int ask_passwd(pam_handle_t *pamh, int pwtype)
{
    struct pam_message msg[1], *mesg[1];
    struct pam_response *resp = NULL;
    int i = 0;
    int retval;

    msg[i].msg = "Password: ";
    msg[i].msg_style = PAM_PROMPT_ECHO_OFF;
    mesg[i] = &msg[i];

    retval = converse(pamh, ++i, mesg, &resp);

    if (retval != PAM_SUCCESS) {
        if (resp != NULL)
            _pam_drop_reply(resp,i);
        return ((retval == PAM_CONV_AGAIN)
                ? PAM_INCOMPLETE : PAM_AUTHINFO_UNAVAIL);
    }

    /* we have a password so set AUTHTOK
     */
    retval=pam_set_item(pamh, pwtype, resp->resp);
    _pam_drop_reply(resp,i);
    return retval;
}






/* PAM authentication management functions */

/* Performs the task of authenticating the user. */
PAM_EXTERN int pam_sm_authenticate (pam_handle_t *pamh,
                                    int flags,
                                    int argc,
                                    const char **argv)
{
    int retval;
    int ctrl = 0;
    const char *username=NULL;
    char *password=NULL;
    char *tmp_buf=NULL; // Temporary buffer to get default_username option.
    const char *fail_default_username=RD_USER_LOGIN_NAME; // Initialize default value.
    RDConfig *login_config;
    RDStation *login_station;
    RDUser * login_user;
    QSqlDatabase *login_db;

    openlog(SYSLOG_IDENT, LOG_CONS|LOG_PID, SYSLOG_FACILITY);

    /* parse arguments */
    ctrl = parse_args(argc, argv, &tmp_buf);
    if (ctrl & PAM_RD_DEBUG) syslog(LOG_DEBUG, "pam_sm_authenticate called.");
    if (ctrl & PAM_RD_DEBUG) {
        syslog(LOG_DEBUG, "argument flags ctrl:%d (0x%x)", ctrl, ctrl);
    }
    if (ctrl & PAM_RD_FAIL_DEFAULT_USER) {
        fail_default_username = tmp_buf;
    }

    /* get username */
    retval = pam_get_user(pamh, &username, NULL);
    if ((retval != PAM_SUCCESS) 
        || (username == NULL) 
        || (*username == '\0')) {
        syslog(LOG_ERR, "get username returned error: %s", 
               pam_strerror(pamh, retval));
        return retval;
    }
    if (ctrl & PAM_RD_DEBUG) syslog(LOG_DEBUG, "got user:%s", username);

    if (ctrl & ~PAM_RD_IGNORE_PASS) {
        /* get password */
        if ((ctrl & PAM_RD_TRY_FIRST_PASS) || (ctrl & PAM_RD_USE_FIRST_PASS)) {
            retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **) &password);
        }

        /* if no password yet, and not configured to only use the first
         * password, then ask */
        if ((password == NULL) && (ctrl & ~PAM_RD_USE_FIRST_PASS)) {
            retval = ask_passwd(pamh, PAM_AUTHTOK);
        }
        retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&password);

        if (password == NULL) {
            syslog(LOG_ERR, "Could not retrieve user's password");
            return PAM_AUTHINFO_UNAVAIL;
        }
        if (ctrl & PAM_RD_DEBUG) syslog(LOG_DEBUG, "got password");
    }

    /* Load configs */
    login_config=new RDConfig();
    login_config->load();

    /* kill daemons if requested */
    if (ctrl & PAM_RD_KILL_RD_DAEMONS) {
        if (ctrl & PAM_RD_DEBUG) syslog(LOG_DEBUG, "killing RD daemons");
        RDKillDaemons();
    }

    /* authenticate the user */
    login_station=new RDStation(login_config->stationName());

    // Initialize QT application, without the gui.  Much like the database
    // connection, while not used here, it is used by underlying functions.
    QApplication a(argc, (char **) argv, false); // FIXME: in QT4 there is a QCoreApplication object that could be used here better.

    // Open Database.  
    // NOTE: while the database connection is not used in this module/file
    // directly, it is required by the underlying functions, ex:
    // login_user->checkPassword()
    login_db=QSqlDatabase::addDatabase(login_config->mysqlDriver());
    if(!login_db) {
        syslog(LOG_ERR, QString().sprintf("Unable to load QSql driver: %s", 
                    login_config->mysqlDriver().ascii()));
        return PAM_AUTHINFO_UNAVAIL;
    }
    login_db->setDatabaseName(login_config->mysqlDbname());
    login_db->setUserName(login_config->mysqlUsername());
    login_db->setPassword(login_config->mysqlPassword());
    login_db->setHostName(login_config->mysqlHostname());
    if(!login_db->open()) {
        syslog(LOG_ERR, "Unable to connet to mySQL server");
        login_db->removeDatabase(login_config->mysqlDbname());
        return PAM_AUTHINFO_UNAVAIL;
    }
    if (ctrl & PAM_RD_DEBUG) syslog(LOG_DEBUG, "connected to database");

    login_user = new RDUser(username);
    if (ctrl & PAM_RD_IGNORE_PASS) {
        // Ignore password, simply check if corresponding user exists.
        if (login_user->exists()) {
            login_station->setDefaultName(username);
            syslog(LOG_NOTICE, 
                   "ignoring password.  RD Default User set to: %s", username);
        } else {
            login_station->setDefaultName(fail_default_username);
            syslog(LOG_NOTICE, 
                   "ignoring password.  RDUser not found, defaulting to: %s", 
                   fail_default_username);
        }
    } else {
        // Check password.
        if ( login_user->checkPassword(QString(password), false) ) {
            login_station->setDefaultName(username);
            syslog(LOG_NOTICE, 
                   "authenticated.  RD Default User set to: %s", username);
        } else {
            login_station->setDefaultName(fail_default_username);
            syslog(LOG_NOTICE, 
                   "authentication failed.  RDUser defaulting to: %s",
                   fail_default_username);
        }
    }

    /* cleanup */
    delete login_user;
    login_db->removeDatabase(login_config->mysqlDbname());
    delete login_station;
    delete login_config;
     
syslog(LOG_NOTICE, "DEBUG ... end pam_rd");
    return PAM_SUCCESS;
}

/* Performs setting of credentials (after the user is authenticated). In the
 * case of Rivendell, the "credential" is registering with the database. */
PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh,int flags,int argc
                   ,const char **argv)
{
     return PAM_SUCCESS;
}


/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <jansen@kde.org>
 * 
 * This is free software; you can use this library under the GNU Library 
 * General Public License, version 2. See the file "COPYING.LIB" for the 
 * exact licensing terms.
 *
 * client.h: client to access kdesud.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <qcstring.h>

/**
 * A client class to access kdesud, the KDE su daemon. Kdesud can assist in 
 * password caching in two ways:
 *
 * @li For high security passwords, like for su and ssh, it executes the
 * password requesting command for you. It feeds the password to the
 * command, without ever returning it to you, the user. The daemon should 
 * be installed setgid nogroup, in order to be able to act as an inaccessible, 
 * trusted 3rd party. 
 * See @ref #exec, @ref #setPass, @ref #delCommand.
 *
 * @li For lower security passwords, like web and ftp passwords, it can act
 * as a persistent storage for string variables. These variables are
 * returned to the user, and the daemon doesn't need to be setgid nogroup
 * for this.
 * See @ref #setVar, @ref #delVar, @ref #delGroup.
 */

class KDEsuClient {
public:
    KDEsuClient();
    ~KDEsuClient();

    /**
     * Lets kdesud execute a command. If the daemon does not have a password 
     * for this command, this will fail and you need to call @ref #setPass().
     *
     * @param command The command to execute.
     * @param user The user to run the command as.
     * @return Zero on success, -1 on failure.
     */
    int exec(QCString command, QCString user);

    /**
     * Set root's password, lasts one session.
     *
     * @param pass Root's password.
     * @param timeout The time that a password will live.
     * @return Zero on success, -1 on failure.
     */
    int setPass(const char *pass, int timeout);

    /**
     * Set the target host (optional).
     */
    int setHost(QCString host);

    /**
     * Set the desired priority (optional), see @ref #StubProcess.
     */
    int setPriority(int priority);

    /**
     * Set the desired scheduler (optional), see @ref #StubProcess.
     */
    int setScheduler(int scheduler);

    /**
     * Remove a password for a user/command.
     * @param command The command.
     * @param user The user.
     * @return zero on success, -1 on an error
     */
    int delCommand(QCString command, QCString user);

    /**
     * Set a persistent variable.
     * @param key The name of the variable.
     * @param value Its value.
     * @param timeout The timeout in seconds for this key. Zero means 
     * no timeout.
     * @param group Make the key part of a group. See @ref #delGroup.
     * @return zero on success, -1 on failure.
     */
    int setVar(QCString key, QCString value, int timeout=0, QCString group=0);

    /**
     * Get a persistent variable.
     * @param key The name of the variable.
     * @return Its value.
     */
    QCString getVar(QCString key);

    /**
     * Delete a persistent variable.
     * @param key The name of the variable.
     * @return zero on success, -1 on failure.
     */
    int delVar(QCString key);

    /**
     * Delete all persistent variables in a group.
     * @param group The group name. See @ref #setVar.
     */
    int delGroup(QCString group);

    /**
     * Ping kdesud. This can be used for diagnostics.
     * @return Zero on success, -1 on failure
     */
    int ping();

    /** Stop the daemon.  */
    int stopServer();

    /** Try to start up kdesud */
    int startServer();

    /** Returns true if the server is safe (installed setgid), false otherwise. */
    bool isServerSGID();

private:
    int connect();

    class KDEsuClientPrivate;
    KDEsuClientPrivate *d;

    int sockfd;
    QCString sock;

    int command(QCString cmd, QCString *result=0L);
    QCString escape(QCString str);
};


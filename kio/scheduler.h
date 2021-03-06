// -*- c++ -*-
/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       Waldo Bastian <bastian@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef _kio_scheduler_h
#define _kio_scheduler_h

#include "kio/job.h"
#include "kio/jobclasses.h"
#include <qtimer.h>
#include <qptrdict.h>

#include <dcopobject.h>

namespace KIO {

    class Slave;
    class SlaveList;
    class SlaveConfig;
    class SessionData;

    /**
     * The KIO::Scheduler manages io-slaves for the application.
     * It also queues jobs and assigns the job to a slave when one
     * becomes available.
     *
     * There are 3 possible ways for a job to get a slave:
     *
     * 1) Direct.
     * This is the default. When you create a job the
     * KIO::Scheduler will be notified and will find either an existing
     * slave that is idle or it will create a new slave for the job.
     *
     * Example:
     *    TransferJob *job = KIO::get(KURL("http://www.kde.org"));
     *
     *
     * 2) Scheduled
     * If you create a lot of jobs, you might want not want to have a
     * slave for each job. If you schedule a job, a maximum number
     * of slaves will be created. When more jobs arrive, they will be
     * queued. When a slave is finished with a job, it will be assigned
     * a job from the queue.
     *
     * Example:
     *    TransferJob *job = KIO::get(KURL("http://www.kde.org"));
     *    KIO::Scheduler::scheduleJob(job);
     *
     * 3) Connection Oriented
     * For some operations it is important that multiple jobs use
     * the same connection. This can only be ensured if all these jobs
     * use the same slave.
     *
     * You can ask the scheduler to open a slave for connection oriented
     * operations. You can then use the scheduler to assign jobs to this
     * slave. The jobs will be queued and the slave will handle these jobs
     * one after the other.
     *
     * Example:
     *    Slave *slave = KIO::Scheduler::getConnectedSlave(
     *            KURL("pop3://bastian:password@mail.kde.org"));
     *    TransferJob *job1 = KIO::get(
     *            KURL("pop3://bastian:password@mail.kde.org/msg1"));
     *    KIO::Scheduler::assignJobToSlave(slave, job1);
     *    TransferJob *job2 = KIO::get(
     *            KURL("pop3://bastian:password@mail.kde.org/msg2"));
     *    KIO::Scheduler::assignJobToSlave(slave, job2);
     *    TransferJob *job3 = KIO::get(
     *            KURL("pop3://bastian:password@mail.kde.org/msg3"));
     *    KIO::Scheduler::assignJobToSlave(slave, job3);
     *
     *    ... Wait for jobs to finish...
     *
     *    KIO::Scheduler::disconnectSlave(slave);
     **/

    class Scheduler : public QObject, virtual public DCOPObject {
        Q_OBJECT

    public:
        typedef QList<SimpleJob> JobList;

        // InfoDict needs Info, so we can't declare it private
        class ProtocolInfo;
        class JobData;

        ~Scheduler();

        static void doJob(SimpleJob *job)
        { self()->_doJob(job); }

        static void scheduleJob(SimpleJob *job)
        { self()->_scheduleJob(job); }

        static void cancelJob(SimpleJob *job)
        { self()->_cancelJob(job); }

        static void jobFinished(KIO::SimpleJob *job, KIO::Slave *slave)
        { self()->_jobFinished(job, slave); }

        static void putSlaveOnHold(KIO::SimpleJob *job, const KURL &url)
        { self()->_putSlaveOnHold(job, url); }

        static void removeSlaveOnHold()
        { self()->_removeSlaveOnHold(); }

        /**
         * Request a slave for use in connection-oriented mode.
         *
         * @param url This defines the username,password,host & port to
         *            connect with.
         * @param config Configuration data for the slave.
         *
         * @return A pointer to a connected slave or 0 if an error occured.
         * @see assignJobToSlave
         * @see disconnectSlave
         */
        static KIO::Slave *getConnectedSlave(const KURL &url, const KIO::MetaData &config = MetaData() )
        { return self()->_getConnectedSlave(url, config); }

        /*
         * Use @p slave to do @p job.
         *
         * @param slave The slave to use. The slave must have been obtained
         *              with a call to @ref getConnectedSlave and must not
         *              be currently assigned to any other job.
         * @param job The job to do.
         *
         * @return true is successfull, false otherwise.
         *
         * This function should be called immediately after creating a Job.
         *
         * @see getConnectedSlave
         * @see disconnectSlave
         * @see slaveConnected
         * @see slaveError
         */
        static bool assignJobToSlave(KIO::Slave *slave, KIO::SimpleJob *job)
        { return self()->_assignJobToSlave(slave, job); }

        /*
         * Disconnect @p slave.
         *
         * @param slave The slave to disconnect. The slave must have been
         *              obtained with a call to @ref getConnectedSlave
         *              and must not be assigned to any job.
         *
         * @return true is successfull, false otherwise.
         *
         * @see getConnectedSlave
         * @see assignJobToSlave
         */
        static bool disconnectSlave(KIO::Slave *slave)
        { return self()->_disconnectSlave(slave); }

        /**
         * Function to connect signals emitted by the scheduler.
         *
         * @see slaveConnected
         * @see slaveError
         */

        static bool connect( const char *signal, const QObject *receiver,
                             const char *member)
        { return QObject::connect(self(), signal, receiver, member); }

        static bool connect( const QObject* sender, const char* signal,
                             const QObject* receiver, const char* member )
        { return QObject::connect(sender, signal, receiver, member); }

        static bool disconnect( const QObject* sender, const char* signal,
                                const QObject* receiver, const char* member )
        { return QObject::disconnect(sender, signal, receiver, member); }

        bool connect( const QObject *sender, const char *signal,
                      const char *member )
        { return QObject::connect(sender, signal, member); }

        void debug_info();

        virtual bool process(const QCString &fun, const QByteArray &data,
                             QCString& replyType, QByteArray &replyData);

        virtual QCStringList functions();

    public slots:
        void slotSlaveDied(KIO::Slave *slave);
        void slotSlaveStatus(pid_t pid, const QCString &protocol,
                             const QString &host, bool connected);
    signals:
        void slaveConnected(KIO::Slave *slave);
        void slaveError(KIO::Slave *slave, int error, const QString &errorMsg);

    protected:
        bool startJobScheduled(ProtocolInfo *protInfo);
        bool startJobDirect();
        Scheduler();

    protected slots:
        void startStep();
        void slotCleanIdleSlaves();
        void slotSlaveConnected();
        void slotSlaveError(int error, const QString &errorMsg);
        void slotScheduleCoSlave();

    private:
        class ProtocolInfoDict;
        class ExtraJobData;

        Scheduler(const Scheduler&);
        static Scheduler *self();
        static Scheduler *instance;
        void _doJob(SimpleJob *job);
        void _scheduleJob(SimpleJob *job);
        void _cancelJob(SimpleJob *job);
        void _jobFinished(KIO::SimpleJob *job, KIO::Slave *slave);
        void _scheduleCleanup();
        void _putSlaveOnHold(KIO::SimpleJob *job, const KURL &url);
        void _removeSlaveOnHold();
        Slave *_getConnectedSlave(const KURL &url, const KIO::MetaData &metaData );
        bool _assignJobToSlave(KIO::Slave *slave, KIO::SimpleJob *job);
        bool _disconnectSlave(KIO::Slave *slave);

        Slave *findIdleSlave(ProtocolInfo *protInfo, SimpleJob *job, bool &exact);
        Slave *createSlave(ProtocolInfo *protInfo, SimpleJob *job, const KURL &url);

        QTimer slaveTimer;
        QTimer coSlaveTimer;
        QTimer cleanupTimer;
        bool busy;

        SlaveList *slaveList;
        SlaveList *idleSlaves;
        SlaveList *coIdleSlaves;

        ProtocolInfoDict *protInfoDict;
        Slave *slaveOnHold;
        KURL urlOnHold;
        JobList newJobs;

        QPtrDict<JobList> coSlaves;
        ExtraJobData *extraJobData;
        SlaveConfig *slaveConfig;
        SessionData *sessionData;
};

};
#endif

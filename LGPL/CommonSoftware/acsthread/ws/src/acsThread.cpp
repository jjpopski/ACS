/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* (c) European Southern Observatory, 2005 
* 
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id: acsThread.cpp,v 1.30 2006/03/10 12:16:55 gchiozzi Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* bjeram  2005-02-15  created 
*/

#include "vltPort.h"

static char *rcsId="@(#) $Id: acsThread.cpp,v 1.30 2006/03/10 12:16:55 gchiozzi Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include "acsThread.h"
#include "acsThreadManager.h"

using namespace ACS;
// static
ACE_TSS<ThreadManager::ThreadManagerTSS> ThreadManager::threadManagerTSS;

Thread::Thread(const ACE_CString & name,
	       const bool suspended,
	       const TimeInterval& responseTime, 
	       const TimeInterval& sleepTime,
	       bool del
		     ) : 
    ThreadBase(name, 
	       ACE_Thread_Manager::instance(), 
	       (void*)Thread::threadSvc, 
	       (void*)this,
	       responseTime,
	       sleepTime,
	       suspended,
	       false /* super class shall not create a thread */),
    logger_mp(0), thrMgr_mp(0), delete_m(del)
{
    ACS_TRACE("ACS::Thread::Thread");
    thrMgr_mp = ACS::ThreadManager::threadManagerTSS->getThreadManager(true);
    if (thrMgr_mp != NULL)
	{
	thrMgr_mp->add2map(name, static_cast<ACS::Thread*>(this));
	}
    else
	{
        /* 
	 * If the thread is not created by the thread manager,
	 * it has to be created suspended.
	 * Otherwise we risk that the thread service function starts
	 * to execute before the creation of the the thread object
	 * itself is completed. 
	 * (problem with virtual functions: run/runLoop)
	 */
	suspend();
	}
    /*
     * We create a Kernel Thread and the thread is:
     * - DETACHED if it has to delete the Thread object itself upon exiting
     * - JOINABLE in the opposite case.
     * But we have to be careful: if we have a JOINABLE thread,
     * we HAVE TO JOIN to it to release resources!!!!
     */
    if (!create(del ? (THR_NEW_LWP | THR_DETACHED): (THR_NEW_LWP | THR_JOINABLE)))
	{
	if (thrMgr_mp != NULL)  thrMgr_mp->removeFromMap(name);
	acsthreadErrType::CanNotSpawnThreadExImpl ex(__FILE__, __LINE__, "ACS::Thread::Thread");
	ex.setThreadName(getName());
	throw ex;
	}//if
}

Thread::Thread(const ACE_CString & name,
	       const bool suspended,
	       const TimeInterval& responseTime, 
	       const TimeInterval& sleepTime,
	       bool del,
	       const long _thrFlags
		     ) : 
    ThreadBase(name, 
	       ACE_Thread_Manager::instance(), 
	       (void*)Thread::threadSvc, 
	       (void*)this,
	       responseTime,
	       sleepTime,
	       suspended,
	       false /* super class shall not create a thread */),
    logger_mp(0), thrMgr_mp(0), delete_m(del)
{
    ACS_TRACE("ACS::Thread::Thread");
    thrMgr_mp = ACS::ThreadManager::threadManagerTSS->getThreadManager(true);
    if (thrMgr_mp != NULL)
	{
	thrMgr_mp->add2map(name, static_cast<ACS::Thread*>(this));
	}
    else
	{
        /* 
	 * If the thread is not created by the thread manager,
	 * it has to be created suspended.
	 * Otherwise we risk that the thread service function starts
	 * to execute before the creation of the the thread object
	 * itself is completed. 
	 * (problem with virtual functions: run/runLoop)
	 */
	suspend();
	}
    /*
     * We create a Kernel Thread and the thread is:
     * - DETACHED if it has to delete the Thread object itself upon exiting
     * - JOINABLE in the opposite case.
     * But we have to be careful: if we have a JOINABLE thread,
     * we HAVE TO JOIN to it to release resources!!!!
     */
    if (!create(_thrFlags))
	{
	if (thrMgr_mp != NULL)  thrMgr_mp->removeFromMap(name);
	acsthreadErrType::CanNotSpawnThreadExImpl ex(__FILE__, __LINE__, "ACS::Thread::Thread");
	ex.setThreadName(getName());
	throw ex;
	}//if
}

Thread::~Thread()
{
    ACS_TRACE("ACS::Thread::~Thread");

   /*
    * Here we call terminte to make sure the thread
    * function is completed BEFORE the class
    * members and virtual tables are destroyed.
    * It could be that the thread function is using
    * some object resources and if this happens after 
    * the object has been destroyed we can easily get
    * memory access faults.
    */
    terminate();
    if (thrMgr_mp != 0)
	{
	thrMgr_mp->removeFromMap(this->getName());
	}
}

ACS::ThreadManager*  Thread::getThreadManager()
{
    return thrMgr_mp;
}

Logging::Logger::LoggerSmartPtr Thread::getLogger()
{
    if (getThreadManager()!=0)
	{
	return getThreadManager()->getLogger();
	}
    else
	{
	ACS_CHECK_LOGGER;
	return ::getLogger();
	}
}//getLogger

void Thread::commonStart()
{ 
    logger_mp = new LoggingProxy(0, 0, 31);
    LoggingProxy::init(logger_mp);
    LoggingProxy::ThreadName(getName().c_str());
    ACS_TRACE("ACS::Thread::commonStart"); // we can not put before initalization of logging system
    onStart();
}

void Thread::commonStop()
{
    ACS_TRACE("ACS::Thread::commonStop");
    onStop();
    // logger can be deleted only if delete is not called from SVC function 
    if (!delete_m) delete logger_mp; // we can delete the logger w/o  problem because of fix in the logging system
}

/*
 * Debugging logs are commented for performance reasons
 * Uncomment them if needed for debugging.
 */ 
void Thread::run()
{
    ACS_TRACE("ACS::Thread::run");
    while (check())
    {
       setRunning();

       /*
	* This check if isSuspended should not be necessary and is just
	* a precaution.
	* Now sleep() takes care of hanlding suspension.
	* At the first iteration the sleep is performed in the
	* threadSvc, so that we do not even come here.
	* At the following iterations is done here below, followed
	* by the check() here above in the while condition.
	*/
       if (!isSuspended())
	   {
	   runLoop();
	   }
       // else
       //   {
       //   ACS_TRACE("ACS::Thread::run susp");
       //   }
       resetRunning();

       /*
	* GCH 2006-01-26
	* Here we can now call directly sleep.
	* We do not need any more any waitForResume(), since 
	* everything is now implemented inside the sleep itself.
	*/
       /*
	* With the current implementation of sleep()
	* the run will stop here also in case of suspend().
	* until released or interrupted.
	* Immediatly after it will go back to check()
	* ensuring that we verify if we have to perform another iteration
	* or exit the thread.
	* After a sleep() always do a check()!
	*/
       sleep();
    }
} //run

/*
 * We do not put logs here to ensure max performance
 */
void Thread::threadSvc(void *param)
{
    Thread* thrObj_p = (Thread*)(((ThreadBaseParameter*) param)->getParameter());
    
    /*
     * If the thread is born suspended,
     * call the thread sleep() that will wait until released
     * or interrupted.
     * After that always perform a check() so that we can exit cleanly
     * in case this war the reason for interrupting the sleep.
     */
    if( thrObj_p->isSuspended() )
	{
	thrObj_p->sleep();
	}

    if( thrObj_p->check() )
	{
	thrObj_p->commonStart(); // Init logger. Should be this executed before while ?
	thrObj_p->run();
	thrObj_p->commonStop(); // This deletes the logger. Mo more logs after this!
	}

    thrObj_p->setStopped();

    delete (ThreadBaseParameter*) param;

    /** 
     * @todo GCH IMPORTANT
     * Look at the code after this comment.
     * This needs to be changed.
     * If we here delete the object, other threads using it might
     * get into big troubles.
     * In particular we have problems with calls to stop() from another thread.
     * stop() monitors thread variables to know if the object managed
     * to stop(), but if the object has been also deleted, that variables are 
     * not valid any more and we get a segfault().
     */
    if (thrObj_p->delete_m) 
	{
	delete thrObj_p; // deleting thread object
	}
} // threadSvc

/*___oOo___*/

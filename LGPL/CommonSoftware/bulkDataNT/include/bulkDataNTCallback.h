#ifndef _BULKDATA_NT_CALLBACK_H
#define _BULKDATA_NT_CALLBACK_H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <Message_Block.h>

/*
#include "orbsvcs/AV/AVStreams_i.h"
#include "orbsvcs/AV/Endpoint_Strategy.h"
#include "orbsvcs/AV/Protocol_Factory.h"
#include "orbsvcs/AV/Flows_T.h"
#include "orbsvcs/AV/Transport.h"
#include "orbsvcs/AV/Policy.h"

#include <baci.h>

#include "ACSBulkDataError.h"
#include "ACSBulkDataStatus.h"

#include "bulkDataReceiver.h"
*/

#include <SString.h>

class BulkDataCallback
//: public TAO_AV_Callback
{

  public:
/*
    enum Cb_State
    {
	CB_UNS,
	CB_SEND_PARAM,
	CB_SEND_DATA
    };

    enum Cb_SubState
    {
	CB_SUB_UNS,
	CB_SUB_INIT
    };


    BulkDataCallback();
*/
    virtual ~BulkDataCallback(){};
/*
    virtual int handle_start(void);

    virtual int handle_stop (void);

    virtual int handle_destroy (void);

    virtual int receive_frame (ACE_Message_Block *frame, TAO_AV_frame_info *frame_info, const ACE_Addr &);

*/
    virtual void setFlowName (const char* name) { flowName_m =name; }

    virtual void setStreamName (const char* name) { streamName_m =name; }

    virtual void setReceiverName(ACE_CString &name) { recvName_m=name; }
/*
    virtual void setSleepTime(ACE_Time_Value locWaitPeriod);

    virtual void setSafeTimeout(CORBA::ULong locLoop);

    virtual CORBA::Boolean isTimeout();
    virtual CORBA::Boolean isWorking();
    virtual CORBA::Boolean isError();

    virtual CompletionImpl *getErrorCompletion();

    virtual void setFlowTimeout(CORBA::ULong timeout);

    virtual void closePeer();

    template<class TCallback>
    void setReceiver(AcsBulkdata::BulkDataReceiver<TCallback> *recv)
	{
	    ACE_UNUSED_ARG(recv);
	    //to be defined by the user
	}

    ACE_HANDLE getHandle();

    CORBA::Boolean isFepAlive()
	{
	    return isFepAlive_m;
	}

    virtual void setCbTimeout(ACE_Time_Value cbTimeout)
	{
	    // empty
	}

    void fwdData2UserCB(CORBA::Boolean enable);
*/
    /********************* methods to be implemented by the user *****************/

    //Depreciated !!
    virtual int cbStart(ACE_Message_Block * userParam_p = 0){return 0;}
    virtual int cbStart(unsigned char* userParam_p = 0, unsigned  int size=0);//=0;

    //Depreciated !!
    virtual int cbReceive(ACE_Message_Block * frame_p) {return 0;}
    virtual int cbReceive(unsigned char * frame_p, unsigned  int size);//=0;

    virtual int cbStop() = 0;

  protected:

    std::string flowName_m;
    std::string streamName_m;

    //unsigned int flowNumber_m;

    ACE_CString recvName_m;
/*
    bool fwdData2UserCB_m;

  private:

    void cleanRecvBuffer();

    void checkFlowTimeout();

    ACE_Time_Value waitPeriod_m;

    CORBA::ULong loop_m;

    Cb_State state_m;
    Cb_SubState substate_m;

    CORBA::Long dim_m;

    CORBA::Long count_m;

    CORBA::Long frameCount_m;

    ACE_Message_Block *bufParam_p;

    CORBA::Boolean timeout_m;
    CORBA::Boolean working_m;
    CORBA::Boolean error_m;

    ACSBulkDataStatus::AVCbErrorCompletion *errComp_p;

    CORBA::ULong flowTimeout_m;

    ACE_Time_Value startTime_m;    

    CORBA::Boolean isFepAlive_m;
    */
};


#endif /*!_BULKDATA_CALLBACK_H*/
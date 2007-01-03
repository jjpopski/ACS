/*
 * @@COPYRIGHT@@
 */

package com.cosylab.acs.maci.plug;

import org.omg.CORBA.ORB;

import alma.acs.util.ACSPorts;
import alma.acsdaemon.DaemonHelper;

import com.cosylab.acs.maci.Daemon;
import com.cosylab.acs.maci.Transport;

/**
 * CORBA implementation of transport.
 * @author msekoranja
 */
public class CORBATransport implements Transport {

	ORB orb;
	String managerIOR;
	
	/**
	 * Constructor.
	 * @param orb	CORBA ORB.
	 * @param managerIOR	manager IOR.
	 */
	public CORBATransport(ORB orb, String managerIOR)
	{
		this.orb = orb;
		this.managerIOR = managerIOR;
	}
	
	/* (non-Javadoc)
	 * @see com.cosylab.acs.maci.Transport#getDaemon(java.lang.String)
	 */
	public Daemon getDaemon(String host) {
		
		String daemonCORBALOC = "corbaloc::" + host + ":" + ACSPorts.getDaemonPort() + "/ACSDaemon";

		org.omg.CORBA.Object obj = orb.string_to_object(daemonCORBALOC);
		alma.acsdaemon.Daemon daemon = DaemonHelper.narrow(obj);
		if (daemon == null)
		{
			throw new RuntimeException("Failed to resolve daemon reference.");
		}
		
		// return proxy
		return new DaemonProxy(daemon);
	}

	/* (non-Javadoc)
	 * @see com.cosylab.acs.maci.Transport#getManagerReference()
	 */
	public String getManagerReference() {
		return managerIOR;
	}
}

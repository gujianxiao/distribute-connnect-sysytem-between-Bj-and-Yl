/*
 * Part of the NDNx Java Library.
 *
 * Portions Copyright (C) 2013 Regents of the University of California.
 * 
 * Based on the CCNx C Library by PARC.
 * Copyright (C) 2011, 2013 Palo Alto Research Center, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation. 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details. You should have received
 * a copy of the GNU Lesser General Public License along with this library;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 */
package org.ndnx.ndn.profiles.versioning;

import java.util.Arrays;
import java.util.Random;

import junit.framework.Assert;

import org.ndnx.ndn.NDNHandle;
import org.ndnx.ndn.impl.NDNFlowControl.SaveType;
import org.ndnx.ndn.io.content.NDNStringObject;
import org.ndnx.ndn.io.content.Link;
import org.ndnx.ndn.io.content.Link.LinkObject;
import org.ndnx.ndn.profiles.versioning.VersioningInterest;
import org.ndnx.ndn.protocol.ContentName;
import org.ndnx.ndn.protocol.ContentObject;
import org.ndnx.ndn.profiles.versioning.VersioningHelper.ReceivedData;
import org.ndnx.ndn.profiles.versioning.VersioningHelper.TestListener;
import org.junit.Test;

public class QueueOfLinksTestRepo {
	Random rnd = new Random();
	
	@Test
	public void testLinkQueue() throws Exception {
		String prefix = String.format("/repotest/test_%016X", rnd.nextLong());
		String queuenamestring = prefix + "/queue";
		String objnamestring = prefix + "/obj";
		ContentName queuename = ContentName.fromNative(queuenamestring);
		ContentName objname = ContentName.fromNative(objnamestring);
		int objsize = 1024*600;
		NDNHandle recvhandle = NDNHandle.getHandle();
		NDNHandle sendhandle = NDNHandle.open(recvhandle.keyManager());
		
		char [] buf = new char[objsize];
		Arrays.fill(buf, 'x');
		String objfill = String.valueOf(buf);
		
		VersioningInterest vi = new VersioningInterest(recvhandle);
		TestListener listener = new TestListener();
		vi.expressInterest(queuename, listener);
		
		Thread.sleep(1000);
		
		NDNStringObject so = new NDNStringObject(objname, objfill, SaveType.LOCALREPOSITORY, sendhandle);
		so.save();
		so.close();
		
		Link link = new Link(so.getVersionedName());
		LinkObject lo = new LinkObject(queuename, link, SaveType.LOCALREPOSITORY, sendhandle);
		lo.save();
		lo.close();
		
		sendhandle.close();
		
		// now see if we got it in the TestListener
		
		Assert.assertTrue(listener.cl.waitForValue(1, 60000));
		
		ReceivedData rd = listener.received.get(0);
		
		ContentObject co = rd.object;
		
		NDNStringObject so2 = new NDNStringObject(co.name(), recvhandle);
		
		Assert.assertEquals(so, so2);
	}
}

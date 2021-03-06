/*
 * NDNx Android Services
 *
 * Portions Copyright (C) 2013 Regents of the University of California.
 * 
 * Based on the CCNx C Library by PARC.
 * Copyright (C) 2010 Palo Alto Research Center, Inc.
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

package org.ndnx.android.ndnlib;

/**
 * This is a list of possible states a service can be in.
 * 
 * TODO: The enum and public finals should be unified at some point
 */
public class NDNxServiceStatus {
	
	public enum SERVICE_STATUS {
		START_ALL_INITIALIZING,
		START_ALL_NDND_DONE,
		START_ALL_REPO_DONE,
		START_ALL_DONE,
		START_ALL_ERROR,
		STOP_ALL_DONE,
		SERVICE_OFF,
		SERVICE_INITIALIZING,
		SERVICE_SETUP,
		SERVICE_RUNNING,
		SERVICE_TEARING_DOWN,
		SERVICE_FINISHED,
		SERVICE_ERROR,
		NDND_INITIALIZING,
		NDND_RUNNING,
		NDND_TEARING_DOWN,
		NDND_OFF,
		REPO_INITIALIZING,
		REPO_RUNNING,
		REPO_TEARING_DOWN,
		REPO_OFF,
		UNKNOWN;
		
		/**
		 * Convert from ordinal to a SERVICE_STATUS. This is the reverse of the ordinal() function.
		 * This method is NOT OPTIMIZED.
		 * 
		 * TODO: Optimize this method
		 * 
		 * @param i Ordinal to convert
		 * @return SERVICE_STATUS object or null if not found
		 */
		public static SERVICE_STATUS fromOrdinal(int i){
			for(SERVICE_STATUS st : SERVICE_STATUS.values()){
				if(st.ordinal() == i){
					return st;
				}
			}
			return UNKNOWN;
		}
	};
}

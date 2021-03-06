/*
 * NDIS Error mappings
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _ndiserrmap_h_
#define _ndiserrmap_h_

extern int ndisstatus2bcmerror(NDIS_STATUS status);
extern NDIS_STATUS bcmerror2ndisstatus(int bcmerror);

#endif	/* _ndiserrmap_h_ */

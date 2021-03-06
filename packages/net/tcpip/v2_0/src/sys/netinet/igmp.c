//==========================================================================
//
//      sys/netinet/igmp.c
//
//     
//
//==========================================================================
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


/*	$OpenBSD: igmp.c,v 1.6 1999/12/08 06:50:19 itojun Exp $	*/
/*	$NetBSD: igmp.c,v 1.15 1996/02/13 23:41:25 christos Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Internet Group Management Protocol (IGMP) routines.
 *
 * Written by Steve Deering, Stanford, May 1988.
 * Modified by Rosen Sharma, Stanford, Aug 1994.
 * Modified by Bill Fenner, Xerox PARC, Feb 1995.
 *
 * MULTICAST Revision: 1.3
 */

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/protosw.h>
#ifndef __ECOS
#include <sys/systm.h>
#endif

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/igmp.h>
#include <netinet/igmp_var.h>
#ifndef __ECOS
#include <dev/rndvar.h>
#endif

#include <machine/stdarg.h>

#define IP_MULTICASTOPTS	0

int		igmp_timers_are_running;
static struct router_info *rti_head;

void igmp_sendpkt __P((struct in_multi *, int));
static int rti_fill __P((struct in_multi *));
static struct router_info * rti_find __P((struct ifnet *));
struct igmpstat igmpstat;  //add by K.M
void
igmp_init()
{

	/*
	 * To avoid byte-swapping the same value over and over again.
	 */
	igmp_timers_are_running = 0;
	rti_head = 0;
}

static int
rti_fill(inm)
	struct in_multi *inm;
{
	register struct router_info *rti;

	for (rti = rti_head; rti != 0; rti = rti->rti_next) {
		if (rti->rti_ifp == inm->inm_ifp) {
			inm->inm_rti = rti;
			if (rti->rti_type == IGMP_v1_ROUTER)
				return (IGMP_v1_HOST_MEMBERSHIP_REPORT);
			else
				return (IGMP_v2_HOST_MEMBERSHIP_REPORT);
		}
	}

	rti = (struct router_info *)malloc(sizeof(struct router_info),
					   M_MRTABLE, M_NOWAIT);
	rti->rti_ifp = inm->inm_ifp;
	rti->rti_type = IGMP_v2_ROUTER;
	rti->rti_next = rti_head;
	rti_head = rti;
	inm->inm_rti = rti;
	return (IGMP_v2_HOST_MEMBERSHIP_REPORT);
}

static struct router_info *
rti_find(ifp)
	struct ifnet *ifp;
{
	register struct router_info *rti;

	for (rti = rti_head; rti != 0; rti = rti->rti_next) {
		if (rti->rti_ifp == ifp)
			return (rti);
	}

	rti = (struct router_info *)malloc(sizeof(struct router_info),
					   M_MRTABLE, M_NOWAIT);
	rti->rti_ifp = ifp;
	rti->rti_type = IGMP_v2_ROUTER;
	rti->rti_next = rti_head;
	rti_head = rti;
	return (rti);
}

void
rti_delete(ifp)
	struct ifnet *ifp;
{
	struct router_info *rti, **prti = &rti_head;

	for (rti = rti_head; rti != 0; rti = rti->rti_next) {
		if (rti->rti_ifp == ifp) {
			*prti = rti->rti_next;
			free(rti, M_MRTABLE);
			break;
		}
		prti = &rti->rti_next;
	}
}

void
#if __STDC__
igmp_input(struct mbuf *m, ...)
#else
igmp_input(m, va_alist)
	struct mbuf *m;
	va_dcl
#endif
{
	int proto;
	register int iphlen;
	register struct ifnet *ifp = m->m_pkthdr.rcvif;
	register struct ip *ip = mtod(m, struct ip *);
	register struct igmp *igmp;
	register int igmplen;
	register int minlen;
	struct in_multi *inm;
	struct in_multistep step;
	struct router_info *rti;
	register struct in_ifaddr *ia;
	int timer;
	va_list ap;

	va_start(ap, m);
	iphlen = va_arg(ap, int);
	proto = va_arg(ap, int);
	va_end(ap);

	++igmpstat.igps_rcv_total;

	igmplen = ip->ip_len;

	/*
	 * Validate lengths
	 */
	if (igmplen < IGMP_MINLEN) {
		++igmpstat.igps_rcv_tooshort;
		m_freem(m);
		return;
	}
	minlen = iphlen + IGMP_MINLEN;
	if ((m->m_flags & M_EXT || m->m_len < minlen) &&
	    (m = m_pullup(m, minlen)) == 0) {
		++igmpstat.igps_rcv_tooshort;
		return;
	}

	/*
	 * Validate checksum
	 */
	m->m_data += iphlen;
	m->m_len -= iphlen;
	igmp = mtod(m, struct igmp *);
	if (in_cksum(m, igmplen)) {
		++igmpstat.igps_rcv_badsum;
		m_freem(m);
		return;
	}
	m->m_data -= iphlen;
	m->m_len += iphlen;
	ip = mtod(m, struct ip *);

	switch (igmp->igmp_type) {

	case IGMP_HOST_MEMBERSHIP_QUERY:
		++igmpstat.igps_rcv_queries;

		if (ifp->if_flags & IFF_LOOPBACK)
			break;

		if (igmp->igmp_code == 0) {
			rti = rti_find(ifp);
			rti->rti_type = IGMP_v1_ROUTER;
			rti->rti_age = 0;

			if (ip->ip_dst.s_addr != INADDR_ALLHOSTS_GROUP) {
				++igmpstat.igps_rcv_badqueries;
				m_freem(m);
				return;
			}

			/*
			 * Start the timers in all of our membership records
			 * for the interface on which the query arrived,
			 * except those that are already running and those
			 * that belong to a "local" group (224.0.0.X).
			 */
			IN_FIRST_MULTI(step, inm);
			while (inm != NULL) {
				if (inm->inm_ifp == ifp &&
				    inm->inm_timer == 0 &&
				    !IN_LOCAL_GROUP(inm->inm_addr.s_addr)) {
					inm->inm_state = IGMP_DELAYING_MEMBER;
					inm->inm_timer = IGMP_RANDOM_DELAY(
					    IGMP_MAX_HOST_REPORT_DELAY * PR_FASTHZ);
					igmp_timers_are_running = 1;
				}
				IN_NEXT_MULTI(step, inm);
			}
		} else {
			if (!IN_MULTICAST(ip->ip_dst.s_addr)) {
				++igmpstat.igps_rcv_badqueries;
				m_freem(m);
				return;
			}

			timer = igmp->igmp_code * PR_FASTHZ / IGMP_TIMER_SCALE;

			/*
			 * Start the timers in all of our membership records
			 * for the interface on which the query arrived,
			 * except those that are already running and those
			 * that belong to a "local" group (224.0.0.X).  For
			 * timers already running, check if they need to be
			 * reset.
			 */
			IN_FIRST_MULTI(step, inm);
			while (inm != NULL) {
				if (inm->inm_ifp == ifp &&
				    !IN_LOCAL_GROUP(inm->inm_addr.s_addr) &&
				    (ip->ip_dst.s_addr == INADDR_ALLHOSTS_GROUP ||
				     ip->ip_dst.s_addr == inm->inm_addr.s_addr)) {
					switch (inm->inm_state) {
					case IGMP_DELAYING_MEMBER:
						if (inm->inm_timer <= timer)
							break;
						/* FALLTHROUGH */
					case IGMP_IDLE_MEMBER:
					case IGMP_LAZY_MEMBER:
					case IGMP_AWAKENING_MEMBER:
						inm->inm_state =
						    IGMP_DELAYING_MEMBER;
						inm->inm_timer =
						    IGMP_RANDOM_DELAY(timer);
						igmp_timers_are_running = 1;
						break;
					case IGMP_SLEEPING_MEMBER:
						inm->inm_state =
						    IGMP_AWAKENING_MEMBER;
						break;
					}
				}
				IN_NEXT_MULTI(step, inm);
			}
		}

		break;

	case IGMP_v1_HOST_MEMBERSHIP_REPORT:
		++igmpstat.igps_rcv_reports;

		if (ifp->if_flags & IFF_LOOPBACK)
			break;

		if (!IN_MULTICAST(igmp->igmp_group.s_addr) ||
		    igmp->igmp_group.s_addr != ip->ip_dst.s_addr) {
			++igmpstat.igps_rcv_badreports;
			m_freem(m);
			return;
		}

		/*
		 * KLUDGE: if the IP source address of the report has an
		 * unspecified (i.e., zero) subnet number, as is allowed for
		 * a booting host, replace it with the correct subnet number
		 * so that a process-level multicast routing daemon can
		 * determine which subnet it arrived from.  This is necessary
		 * to compensate for the lack of any way for a process to
		 * determine the arrival interface of an incoming packet.
		 */
		if ((ip->ip_src.s_addr & IN_CLASSA_NET) == 0) {
			IFP_TO_IA(ifp, ia);
			if (ia)
				ip->ip_src.s_addr = ia->ia_subnet;
		}

		/*
		 * If we belong to the group being reported, stop
		 * our timer for that group.
		 */
		IN_LOOKUP_MULTI(igmp->igmp_group, ifp, inm);
		if (inm != NULL) {
			inm->inm_timer = 0;
			++igmpstat.igps_rcv_ourreports;

			switch (inm->inm_state) {
			case IGMP_IDLE_MEMBER:
			case IGMP_LAZY_MEMBER:
			case IGMP_AWAKENING_MEMBER:
			case IGMP_SLEEPING_MEMBER:
				inm->inm_state = IGMP_SLEEPING_MEMBER;
				break;
			case IGMP_DELAYING_MEMBER:
				if (inm->inm_rti->rti_type == IGMP_v1_ROUTER)
					inm->inm_state = IGMP_LAZY_MEMBER;
				else
					inm->inm_state = IGMP_SLEEPING_MEMBER;
				break;
			}
		}

		break;

	case IGMP_v2_HOST_MEMBERSHIP_REPORT:
#ifdef MROUTING
		/*
		 * Make sure we don't hear our own membership report.  Fast
		 * leave requires knowing that we are the only member of a
		 * group.
		 */
		IFP_TO_IA(ifp, ia);
		if (ia && ip->ip_src.s_addr == ia->ia_addr.sin_addr.s_addr)
			break;
#endif

		++igmpstat.igps_rcv_reports;

		if (ifp->if_flags & IFF_LOOPBACK)
			break;

		if (!IN_MULTICAST(igmp->igmp_group.s_addr) ||
		    igmp->igmp_group.s_addr != ip->ip_dst.s_addr) {
			++igmpstat.igps_rcv_badreports;
			m_freem(m);
			return;
		}

		/*
		 * KLUDGE: if the IP source address of the report has an
		 * unspecified (i.e., zero) subnet number, as is allowed for
		 * a booting host, replace it with the correct subnet number
		 * so that a process-level multicast routing daemon can
		 * determine which subnet it arrived from.  This is necessary
		 * to compensate for the lack of any way for a process to
		 * determine the arrival interface of an incoming packet.
		 */
		if ((ip->ip_src.s_addr & IN_CLASSA_NET) == 0) {
#ifndef MROUTING
			IFP_TO_IA(ifp, ia);
#endif
			if (ia)
				ip->ip_src.s_addr = ia->ia_subnet;
		}

		/*
		 * If we belong to the group being reported, stop
		 * our timer for that group.
		 */
		IN_LOOKUP_MULTI(igmp->igmp_group, ifp, inm);
		if (inm != NULL) {
			inm->inm_timer = 0;
			++igmpstat.igps_rcv_ourreports;

			switch (inm->inm_state) {
			case IGMP_DELAYING_MEMBER:
			case IGMP_IDLE_MEMBER:
			case IGMP_AWAKENING_MEMBER:
				inm->inm_state = IGMP_LAZY_MEMBER;
				break;
			case IGMP_LAZY_MEMBER:
			case IGMP_SLEEPING_MEMBER:
				break;
			}
		}

		break;

	}

	/*
	 * Pass all valid IGMP packets up to any process(es) listening
	 * on a raw IGMP socket.
	 */
	rip_input(m, iphlen, proto);
	return;
}

void
igmp_joingroup(inm)
	struct in_multi *inm;
{
	int s = splsoftnet();

	inm->inm_state = IGMP_IDLE_MEMBER;

	if (!IN_LOCAL_GROUP(inm->inm_addr.s_addr) &&
	    (inm->inm_ifp->if_flags & IFF_LOOPBACK) == 0) {
		igmp_sendpkt(inm, rti_fill(inm));
		inm->inm_state = IGMP_DELAYING_MEMBER;
		inm->inm_timer = IGMP_RANDOM_DELAY(
		    IGMP_MAX_HOST_REPORT_DELAY * PR_FASTHZ);
		igmp_timers_are_running = 1;
	} else
		inm->inm_timer = 0;
	splx(s);
}

void
igmp_leavegroup(inm)
	struct in_multi *inm;
{

	switch (inm->inm_state) {
	case IGMP_DELAYING_MEMBER:
	case IGMP_IDLE_MEMBER:
		if (!IN_LOCAL_GROUP(inm->inm_addr.s_addr) &&
		    (inm->inm_ifp->if_flags & IFF_LOOPBACK) == 0)
			if (inm->inm_rti->rti_type != IGMP_v1_ROUTER)
				igmp_sendpkt(inm, IGMP_HOST_LEAVE_MESSAGE);
		break;
	case IGMP_LAZY_MEMBER:
	case IGMP_AWAKENING_MEMBER:
	case IGMP_SLEEPING_MEMBER:
		break;
	}
}

void
igmp_fasttimo()
{
	register struct in_multi *inm;
	struct in_multistep step;
	int s;

	/*
	 * Quick check to see if any work needs to be done, in order
	 * to minimize the overhead of fasttimo processing.
	 */
	if (!igmp_timers_are_running)
		return;

	s = splsoftnet();
	igmp_timers_are_running = 0;
	IN_FIRST_MULTI(step, inm);
	while (inm != NULL) {
		if (inm->inm_timer == 0) {
			/* do nothing */
		} else if (--inm->inm_timer == 0) {
			if (inm->inm_state == IGMP_DELAYING_MEMBER) {
				if (inm->inm_rti->rti_type == IGMP_v1_ROUTER)
					igmp_sendpkt(inm,
					    IGMP_v1_HOST_MEMBERSHIP_REPORT);
				else
					igmp_sendpkt(inm,
					    IGMP_v2_HOST_MEMBERSHIP_REPORT);
				inm->inm_state = IGMP_IDLE_MEMBER;
			}
		} else {
			igmp_timers_are_running = 1;
		}
		IN_NEXT_MULTI(step, inm);
	}
	splx(s);
}

void
igmp_slowtimo()
{
	register struct router_info *rti;
	int s;

	s = splsoftnet();
	for (rti = rti_head; rti != 0; rti = rti->rti_next) {
		if (rti->rti_type == IGMP_v1_ROUTER &&
		    ++rti->rti_age >= IGMP_AGE_THRESHOLD) {
			rti->rti_type = IGMP_v2_ROUTER;
		}
	}
	splx(s);
}

void
igmp_sendpkt(inm, type)
	struct in_multi *inm;
	int type;
{
	struct mbuf *m;
	struct igmp *igmp;
	struct ip *ip;
	struct ip_moptions imo;
#ifdef MROUTING
	extern struct socket *ip_mrouter;
#endif /* MROUTING */

	MGETHDR(m, M_DONTWAIT, MT_HEADER);
	if (m == NULL)
		return;
	/*
	 * Assume max_linkhdr + sizeof(struct ip) + IGMP_MINLEN
	 * is smaller than mbuf size returned by MGETHDR.
	 */
	m->m_data += max_linkhdr;
	m->m_len = sizeof(struct ip) + IGMP_MINLEN;
	m->m_pkthdr.len = sizeof(struct ip) + IGMP_MINLEN;

	ip = mtod(m, struct ip *);
	ip->ip_tos = 0;
	ip->ip_len = sizeof(struct ip) + IGMP_MINLEN;
	ip->ip_off = 0;
	ip->ip_p = IPPROTO_IGMP;
	ip->ip_src.s_addr = INADDR_ANY;
	ip->ip_dst = inm->inm_addr;

	m->m_data += sizeof(struct ip);
	m->m_len -= sizeof(struct ip);
	igmp = mtod(m, struct igmp *);
	igmp->igmp_type = type;
	igmp->igmp_code = 0;
	igmp->igmp_group = inm->inm_addr;
	igmp->igmp_cksum = 0;
	igmp->igmp_cksum = in_cksum(m, IGMP_MINLEN);
	m->m_data -= sizeof(struct ip);
	m->m_len += sizeof(struct ip);

	imo.imo_multicast_ifp = inm->inm_ifp;
	imo.imo_multicast_ttl = 1;
#ifdef RSVP_ISI
	imo.imo_multicast_vif = -1;
#endif
	/*
	 * Request loopback of the report if we are acting as a multicast
	 * router, so that the process-level routing demon can hear it.
	 */
#ifdef MROUTING
	imo.imo_multicast_loop = (ip_mrouter != NULL);
#else
	imo.imo_multicast_loop = 0;
#endif /* MROUTING */

#if 0 /*KAME IPSEC*/
	m->m_pkthdr.rcvif = NULL;
#endif /*IPSEC*/
	ip_output(m, (struct mbuf *)0, (struct route *)0, IP_MULTICASTOPTS,
	    &imo, NULL);

	++igmpstat.igps_snd_reports;
}

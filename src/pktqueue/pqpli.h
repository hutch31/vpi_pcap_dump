/*! \file pqpli.h
 * C-compiliant function prototypes for packet queue PLI calls.  This file
 * creates the VPI code needed to interface with the simulator to the
 * generic pktqueue code.
 */
/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "vpi_user.h"
#include "cv_vpi_user.h"
#include <stdio.h>

/*! \brief Create a queue
 * 
 * Usage: $pq_request_queue (out qid)
 * 
 * This is the initial call to the pktqueue PLI.  Each queue needs to be
 * created by calling pq_request_queue.  The simulation needs to keep track
 * of the returned qid, although the pktqueue creates qids in the order that
 * it is called.  First qid is qid 0.  Once created queues exist for the
 * lifetime of the simulation.
 */
PLI_INT32 pq_request_queue_call ();
/*! \brief Get number of queues
 * 
 * Usage: $pq_num_queues (out num_queues)
 * 
 * Return the number of queues created.
 */
PLI_INT32 pq_num_queues_call ();
/*! \brief Insert a packet in a queue
 * 
 * Usage: $pq_insert_packet (qid, len, buffer)
 * 
 * Insert a packet in a queue.  The verilog buffer must be declared as an
 * 8-bit wide array.  The len parameter represents the length of the packet
 * in bytes.
 */
PLI_INT32 pq_insert_packet_call ();
/*! \brief Get depth of queue
 * 
 * Usage: $pq_queue_size (qid, out size)
 * 
 * Returns the number of packets currently residing in the queue.
 */
PLI_INT32 pq_queue_size_call ();
/*! \brief Remove packet from a queue
 * 
 * Usage: $pq_remove_packet (qid, out len, out buffer)
 * 
 * Dequeues a packet from the listed queue and places it in the provided
 * buffer.  Updates the len variable to indicate how many bytes were in the
 * packet.
 * 
 * buffer should be an 8-bit wide array deep enough to contain any possible
 * packet.  The len parameter should be an integer or 32-bit reg.  The
 * simulation should call $pq_queue_size() prior to calling $pq_remove_packet()
 * to determine if there is a packet available to be dequeued.
 */
PLI_INT32 pq_remove_packet_call ();


/* -*- c++ -*- */
/* 
 * Copyright 2012 University of Applied Sciences Luebeck.
 * authors:
 * Thomas Schmid, Marius Ciepluch
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_COGRA_QPSK_H
#define INCLUDED_COGRA_QPSK_H

#include <cogra_api.h>
#include <gr_sync_interpolator.h>
#include <gr_types.h>
#include <gr_io_signature.h>


class cogra_qpsk;
typedef boost::shared_ptr<cogra_qpsk> cogra_qpsk_sptr;

COGRA_API cogra_qpsk_sptr cogra_make_qpsk ();

/*!
 * \brief a QPSK modulator
 *
 */
class COGRA_API cogra_qpsk : public gr_sync_interpolator
{
	friend COGRA_API cogra_qpsk_sptr cogra_make_qpsk ();

	cogra_qpsk ();

 public:
	~cogra_qpsk ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COGRA_QPSK_H */


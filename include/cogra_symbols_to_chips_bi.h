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

#ifndef INCLUDED_COGRA_SYMBOLS_TO_CHIPS_BI_H
#define INCLUDED_COGRA_SYMBOLS_TO_CHIPS_BI_H

#include <cogra_api.h>
#include <gr_sync_interpolator.h>


class cogra_symbols_to_chips_bi;
typedef boost::shared_ptr<cogra_symbols_to_chips_bi> cogra_symbols_to_chips_bi_sptr;
COGRA_API cogra_symbols_to_chips_bi_sptr cogra_make_symbols_to_chips_bi();

/*!
 * \brief applies IEEE 802.15.4 DSSS
 *
 */
class COGRA_API cogra_symbols_to_chips_bi: public gr_sync_interpolator {
	friend COGRA_API cogra_symbols_to_chips_bi_sptr cogra_make_symbols_to_chips_bi();

	cogra_symbols_to_chips_bi();

public:
	~cogra_symbols_to_chips_bi();

	int work(int noutput_items, gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COGRA_SYMBOLS_TO_CHIPS_BI_H */


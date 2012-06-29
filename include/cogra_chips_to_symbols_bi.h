/* -*- c++ -*- */
/* 
 * Copyright 2012 University of Applied Sciences Lübeck.
 * authors:
 * Marius Ciepluch
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

#ifndef INCLUDED_COGRA_CHIPS_TO_SYMBOLS_BI_H
#define INCLUDED_COGRA_CHIPS_TO_SYMBOLS_BI_H

#include <cogra_api.h>
#include <gr_sync_decimator.h>


class cogra_chips_to_symbols_bi;
typedef boost::shared_ptr<cogra_chips_to_symbols_bi> cogra_chips_to_symbols_bi_sptr;

COGRA_API cogra_chips_to_symbols_bi_sptr cogra_make_chips_to_symbols_bi ();

/*!
 * \brief maps Chips back to symbols
 * \ingroup decimator
 *
 */
class COGRA_API cogra_chips_to_symbols_bi : public gr_sync_decimator
{
	friend COGRA_API cogra_chips_to_symbols_bi_sptr cogra_make_chips_to_symbols_bi ();

	cogra_chips_to_symbols_bi ();

 public:
	~cogra_chips_to_symbols_bi ();


	int work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COGRA_CHIPS_TO_SYMBOLS_BI_H */


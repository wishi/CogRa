/* -*- c++ -*- */
/* 
 * Copyright 2012 University of Applied Sciences Lübeck.
 * authors:
 * Marius Ciepluch, Thomas Schmid
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <cogra_chips_to_symbols_bi.h>

// this is the MSK table used for oQPSK demodulation if a FM Demod is used
static const unsigned int CHIP_MAPPING[] =
  { 1618456172, 1309113062, 1826650030, 1724778362, 778887287, 2061946375,
      2007919840, 125494990, 529027475, 838370585, 320833617, 422705285,
      1368596360, 85537272, 139563807, 2021988657 };

cogra_chips_to_symbols_bi_sptr
cogra_make_chips_to_symbols_bi()
{
  return cogra_chips_to_symbols_bi_sptr(new cogra_chips_to_symbols_bi());
}

cogra_chips_to_symbols_bi::cogra_chips_to_symbols_bi() :
    gr_sync_decimator("chips_to_symbols_bi",
        gr_make_io_signature(1, -1, sizeof(unsigned int)),
        gr_make_io_signature(1, -1, sizeof(unsigned char)), 2)
{
}

cogra_chips_to_symbols_bi::~cogra_chips_to_symbols_bi()
{
}

int
cogra_chips_to_symbols_bi::work(int noutput_items,
    gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
  // \brief decimation happens here due to the extraction
  // of the PN sequences into the original symbols

  assert(input_items.size() == output_items.size());
  int nstreams = input_items.size();

  for (int m = 0; m < nstreams; m++)
    {
    const unsigned int *in = (unsigned int *) input_items[m];
    unsigned char *out = (unsigned char *) output_items[m];

    // per stream processing
    for (int i = 0; i < noutput_items; i += 2)
      {
      //fprintf(stderr, "%x %x, ", in[i/2]&0xF, (in[i/2]>>4)&0xF), fflush(stderr);

      // The LSBlock is sent first (802.15.4 standard)
      memcpy(&out[i + 1],
          &CHIP_MAPPING[(unsigned int) ((in[i / 2] >> 4) & 0xF)],
          sizeof(unsigned int));
      memcpy(&out[i], &CHIP_MAPPING[(unsigned int) (in[i / 2] & 0xF)],
          sizeof(unsigned int));
    }
  // end of per stream processing

}
return noutput_items;
}


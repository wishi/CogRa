/* -*- c++ -*- */
/* 
 * Copyright 2012 University of Applied Sciences Luebeck.
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
#include <cogra_qpsk.h>
#include <assert.h>
#include <gr_sync_interpolator.h>

#define MAX_IN 	1
#define MAX_OUT 1

static const int SAMPLES_PER_SYMBOL = 4;

cogra_qpsk_sptr
cogra_make_qpsk()
{
  return cogra_qpsk_sptr(new cogra_qpsk());
}

cogra_qpsk::cogra_qpsk() :
    gr_sync_interpolator("qpsk",
        gr_make_io_signature(MAX_IN, MAX_OUT, sizeof(gr_complex)),
        gr_make_io_signature(MAX_IN, MAX_OUT, sizeof(gr_complex)),
        SAMPLES_PER_SYMBOL)
{
}

cogra_qpsk::~cogra_qpsk()
{
}

int
cogra_qpsk::work(int noutput_items, gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{

  // complex in, complex out
  const gr_complex *in = (gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];

  // keep the sync
  assert(noutput_items % SAMPLES_PER_SYMBOL == 0);

  // 4 samples per symbol - so iterate over 4 samples at once
  for (int i = 0; i < noutput_items / SAMPLES_PER_SYMBOL; i++)
    {

    // IQ into 2 float streams
    float iphase = real(in[i]);
    float qphase = imag(in[i]);
    // this Debug message can show all vectors in stdout (slows down)
    //fprintf(stderr, "%.0f %.0f ", iphase, qphase), fflush(stderr);

    // apply pulse shaping with Sinus-based constants
    *out++ = gr_complex(0.0, 0.0);
    *out++ = gr_complex(iphase * 0.70710678, qphase * 0.70710678);
    *out++ = gr_complex(iphase, qphase);
    *out++ = gr_complex(iphase * 0.70710678, qphase * 0.70710678);
  }

return noutput_items;
}


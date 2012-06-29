/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_COGRA_BLUETOOTH_MULTI_BLOCK_H
#define INCLUDED_COGRA_BLUETOOTH_MULTI_BLOCK_H

#include <cogra_api.h>
#include <gr_sync_block.h>

#include <gr_sync_block.h>
#include <stdint.h>
#include <gr_pwr_squelch_cc.h>
#include <gr_freq_xlating_fir_filter_ccf.h>
#include <gr_quadrature_demod_cf.h>
// #include <gr_clock_recovery_mm_ff.h>
// #include <gr_binary_slicer_fb.h>
#include <gri_mmse_fir_interpolator.h>

class cogra_bluetooth_multi_block;
typedef boost::shared_ptr<cogra_bluetooth_multi_block> cogra_bluetooth_multi_block_sptr;
COGRA_API cogra_bluetooth_multi_block_sptr
cogra_make_bluetooth_multi_block(double sample_rate, double center_freq,
    double squelch_threshold);

/*!
 * \brief <+description+>
 *
 */
class COGRA_API cogra_bluetooth_multi_block : public gr_sync_block
{

  /*
  friend COGRA_API cogra_bluetooth_multi_block_sptr
  cogra_make_bluetooth_multi_block(double sample_rate, double center_freq,
      double squelch_threshold);

  cogra_bluetooth_multi_block(double sample_rate, double center_freq,
      double squelch_threshold);
  */

protected:
  /* constructor */
  cogra_bluetooth_multi_block(double sample_rate, double center_freq, double squelch_threshold);

  /* symbols per second */
  static const int SYMBOL_RATE = 1000000;

  /* length of time slot in symbols */
  static const int SYMBOLS_PER_SLOT = 625;

  /* channel 0 in Hz */
  static const uint32_t BASE_FREQUENCY = 2402000000UL;

  /* channel width in Hz */
  static const int CHANNEL_WIDTH = 1000000;

  /* total number of samples elapsed */
  uint64_t d_cumulative_count;

  /* sample rate of raw input stream */
  double d_sample_rate;

  /* number of raw samples per symbol */
  double d_samples_per_symbol;

  /* number of raw samples per time slot (625 microseconds) */
  double d_samples_per_slot;

  /* center frequency of input stream */
  double d_center_freq;

  /* lowest channel (0-78) we can decode */
  int d_low_channel;

  /* highest channel (0-78) we can decode */
  int d_high_channel;

  /* power squelch threshold normalized for comparison in channel_symbols() */
  double d_squelch_threshold;

  /* decimation rate of digital downconverter */
  int d_ddc_decimation_rate;

  /* mm_cr variables */
  float d_gain_mu; // gain for adjusting mu
  float d_mu; // fractional sample position [0.0, 1.0]
  float d_omega_relative_limit; // used to compute min and max omega
  float d_omega; // nominal frequency
  float d_gain_omega; // gain for adjusting omega
  float d_omega_mid; // average omega
  float d_last_sample;

  /* channel filter coefficients for digital downconverter */
  std::vector<float> d_channel_filter;

  /* quadrature frequency demodulator sensitivity */
  float d_demod_gain;

  /* interpolator M&M clock recovery block */
  gri_mmse_fir_interpolator *d_interp;

  /* M&M clock recovery, adapted from gr_clock_recovery_mm_ff */
  int
  mm_cr(const float *in, int ninput_items, float *out, int noutput_items);

  /* fm demodulation, taken from gr_quadrature_demod_cf */
  void
  demod(const gr_complex *in, float *out, int noutput_items);

  /* binary slicer, similar to gr_binary_slicer_fb */
  void
  slicer(const float *in, char *out, int noutput_items);

  /* produce symbols stream for a particular channel pulled out of the raw samples */
  int
  channel_symbols(int channel, gr_vector_const_void_star &in, char *out,
      int ninput_items);

  /* add some number of symbols to the block's history requirement */
  void
  set_symbol_history(int num_symbols);

  /* set available channels based on d_center_freq and d_sample_rate */
  void
  set_channels();

  /* returns relative (with respect to d_center_freq) frequency in Hz of given channel */
  double
  channel_freq(int channel);

public:
  ~cogra_bluetooth_multi_block();

  int
  work(int noutput_items, gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COGRA_BLUETOOTH_MULTI_BLOCK_H */


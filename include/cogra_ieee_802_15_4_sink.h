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

#ifndef INCLUDED_COGRA_IEEE_802_15_4_SINK_H
#define INCLUDED_COGRA_IEEE_802_15_4_SINK_H

#include <cogra_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>
#include <boost/thread.hpp>

class cogra_ieee_802_15_4_sink;
typedef boost::shared_ptr<cogra_ieee_802_15_4_sink> cogra_ieee_802_15_4_sink_sptr;
COGRA_API cogra_ieee_802_15_4_sink_sptr
cogra_make_ieee_802_15_4_sink(int threshold, char *filename);

/*!
 * \brief <+description+>
 *
 */
class COGRA_API cogra_ieee_802_15_4_sink : public gr_sync_block
{
  friend COGRA_API cogra_ieee_802_15_4_sink_sptr
  cogra_make_ieee_802_15_4_sink(int threshold, char *filename);

  // cogra_ieee_802_15_4_sink (int threshold, const char *filename);

private:
  enum state_t
  {
    STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER
  };

  static const int MAX_PKT_LEN = 128 - 1; // remove header and CRC
  static const int MAX_LQI_SAMPLES = 8; // Number of chip correlation samples to take

  //static const unsigned int CHIP_MAPPING[];     // will hold the mapping between chip and symbols

  // gr_msg_queue_sptr d_target_queue; // where to send the packet when received
  unsigned int d_sync_vector; // 802.15.4 standard is 4x 0 bytes and 1x0xA7
  unsigned int d_threshold; // how many bits may be wrong in sync vector

  state_t d_state;

  unsigned int d_shift_reg; // used to look for sync_vector
  int d_preamble_cnt; // count on where we are in preamble
  int d_chip_cnt; // counts the chips collected

  unsigned int d_header; // header bits
  int d_headerbitlen_cnt; // how many so far

  unsigned char d_packet[MAX_PKT_LEN]; // assembled payload
  unsigned char d_packet_byte; // byte being assembled
  int d_packet_byte_index; // which bit of d_packet_byte we're working on
  int d_packetlen; // length of packet
  int d_packetlen_cnt; // how many so far
  int d_payload_cnt; // how many bytes in payload
  int d_processed;
  char* d_filename;

  unsigned int d_lqi; // Link Quality Information
  unsigned int d_lqi_sample_count;

protected:
  cogra_ieee_802_15_4_sink(int threshold, char* filename);

  FILE *d_fp; // current FILE pointer
  FILE *d_new_fp; // new FILE pointer
  bool d_updated; // is there a new FILE pointer?
  boost::mutex d_mutex;
  bool d_is_binary;

  void
  enter_search();
  void
  enter_have_sync();
  void
  enter_have_header(int payload_len);
  unsigned char
  decode_chips(unsigned int chips);
  // for file handling
  void
  write_file(const char* filename);
  bool
  open(const char* filename);
  void
  close();

  int
  slice(float x)
  {
    return x > 0 ? 1 : 0;
  }

  bool
  header_ok()
  {
    // might do some checks on the header later...
    return 1;
  }

public:
  ~cogra_ieee_802_15_4_sink();

  int
  work(int noutput_items, gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
};

#endif /* INCLUDED_COGRA_IEEE_802_15_4_SINK_H */


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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include <cogra_ieee_802_15_4_sink.h>

#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <cstring>
#include <gr_count_bits.h>
#include <gruel/thread.h>

#include <stdexcept>

#ifdef O_LARGEFILE
#define OUR_O_LARGEFILE O_LARGEFILE
#else
#define OUR_O_LARGEFILE 0
#endif

#define VERBOSE         0
#define VERBOSE2        0

// this is the mapping between chips and symbols if we do
// a fm demodulation of the O-QPSK signal. Note that this
// is different than the O-QPSK chip sequence from the
// 802.15.4 standard since there there is a translation
// happening.
// See "CMOS RFIC Architectures for IEEE 802.15.4 Networks",
// John Notor, Anthony Caviglia, Gary Levy, for more details.

// mostly here it's MSK == QPSK
static const unsigned int CHIP_MAPPING[] =
  { 1618456172, 1309113062, 1826650030, 1724778362, 778887287, 2061946375,
      2007919840, 125494990, 529027475, 838370585, 320833617, 422705285,
      1368596360, 85537272, 139563807, 2021988657 };

// state machine enter
inline void
cogra_ieee_802_15_4_sink::enter_search()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_search\n");

  d_state = STATE_SYNC_SEARCH;
  d_shift_reg = 0;
  d_preamble_cnt = 0;
  d_chip_cnt = 0;
  d_packet_byte = 0;
}

// set state for sync
inline void
cogra_ieee_802_15_4_sink::enter_have_sync()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_have_sync\n");

  d_state = STATE_HAVE_SYNC;
  d_packetlen_cnt = 0;
  d_packet_byte = 0;
  d_packet_byte_index = 0;

  // Link Quality Information
  d_lqi = 0;
  d_lqi_sample_count = 0;
}

// header complete
inline void
cogra_ieee_802_15_4_sink::enter_have_header(int payload_len)
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_have_header (payload_len = %d)\n", payload_len);

  d_state = STATE_HAVE_HEADER;
  d_packetlen = payload_len;
  d_payload_cnt = 0;
  d_packet_byte = 0;
  d_packet_byte_index = 0;
}

inline bool
cogra_ieee_802_15_4_sink::open(const char *filename)
{
  gruel::scoped_lock guard(d_mutex); // hold mutex for duration of this function

  // we use the open system call to get access to the O_LARGEFILE flag.
  int fd;
  if ((fd = ::open(filename, O_WRONLY | O_CREAT | O_APPEND | OUR_O_LARGEFILE,
      0664)) < 0)
    {
      perror(filename);
      return false;
    }

  if (d_new_fp)
    { // if we've already got a new one open, close it
      fclose(d_new_fp);
      d_new_fp = 0;
    }

  if ((d_new_fp = fdopen(fd, d_is_binary ? "wba" : "wa")) == NULL)
    {
      perror(filename);
      ::close(fd);
    }

  d_updated = true;
  return d_new_fp != 0;
}

inline void
cogra_ieee_802_15_4_sink::close()
{
  gruel::scoped_lock guard(d_mutex); // hold mutex for duration of this function

  if (d_new_fp)
    {
      fclose(d_new_fp);
      d_new_fp = 0;
    }
  d_updated = true;
}

inline void
cogra_ieee_802_15_4_sink::write_file(const char* filename)
{
  if (this->open(filename))
    {

      std::ostringstream payload;
      std::ostringstream crc;
      payload << std::hex;
      crc << std::hex;

      // last two bytes are CRC
      for (int i = 0; i < this->d_packetlen - 2; i++)
        {
          payload << "0x" << static_cast<unsigned>(this->d_packet[i]) << " ";
          for (int j = i; j < this->d_packetlen; j++)
            {
              crc << static_cast<unsigned>(this->d_packet[j]);
            }
        }

      std::string payload_result = payload.str();
      std::string crc_result = crc.str();

      fprintf(d_new_fp, "%s, %s, %d, %d\n", payload_result.c_str(),
          crc_result.c_str(), this->d_lqi, this->d_lqi_sample_count);

    }
  this->close();
}

// decoding with the chips to the original bits
inline unsigned char
cogra_ieee_802_15_4_sink::decode_chips(unsigned int chips)
{
  int i;
  int best_match = 0xFF;
  int min_threshold = 33; // Matching to 32 chips, could never have a error of 33 chips

  for (i = 0; i < 16; i++)
    {
      // FIXME: we can store the last chip
      // ignore the first and last chip since it depends on the last chip.
      unsigned int threshold = gr_count_bits32(
          (chips & 0x7FFFFFFE) ^ (CHIP_MAPPING[i] & 0x7FFFFFFE));

      if (threshold < min_threshold)
        {
          best_match = i;
          min_threshold = threshold;
        }
    }

  if (min_threshold < d_threshold)
    {
      if (VERBOSE)
        fprintf(stderr, "Found sequence with %d errors at 0x%x\n",
            min_threshold,
            (chips & 0x7FFFFFFE) ^ (CHIP_MAPPING[best_match] & 0x7FFFFFFE)), fflush(
            stderr);
      // LQI: Average number of chips correct * MAX_LQI_SAMPLES
      if (d_lqi_sample_count < MAX_LQI_SAMPLES)
        {
          d_lqi += 32 - min_threshold;
          d_lqi_sample_count++;
        }

      return (char) best_match & 0xF;
    }

  return 0xFF;
}

cogra_ieee_802_15_4_sink_sptr
cogra_make_ieee_802_15_4_sink(int threshold, char *filename)
{
  return cogra_ieee_802_15_4_sink_sptr(
      new cogra_ieee_802_15_4_sink(threshold, filename));
}

cogra_ieee_802_15_4_sink::cogra_ieee_802_15_4_sink(int threshold,
    char *filename) :
    gr_sync_block("ieee_802_15_4_sink",
        gr_make_io_signature(1, 1, sizeof(float)),
        gr_make_io_signature(0, 0, 0))
{
  // apply defined threshold and filename
  d_threshold = threshold;
  d_filename = filename;
  this->d_is_binary = false; // log text not binary

  // check file
  if (!open(this->d_filename))
    {
      throw std::runtime_error("can't open file");
    }

  // initialize for 802.15.4 MAC sublayer
  d_sync_vector = 0xA7;
  d_processed = 0;

  // Link Quality Information
  d_lqi = 0;
  d_lqi_sample_count = 0;

  // if(VERBOSE)
  fprintf(stderr, "syncvec: %x, threshold: %d\n", d_sync_vector, d_threshold), fflush(
      stderr);
  enter_search();

}

cogra_ieee_802_15_4_sink::~cogra_ieee_802_15_4_sink()
{
  this->close();
}

int
cogra_ieee_802_15_4_sink::work(int noutput_items,
    gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
{
  float *inbuf = (float *) input_items[0];
  int count = 0;

  // apply basic carrier sensing to keep the state machine stream synced
  // if(d_state != STATE_SYNC_SEARCH) {

  // fprintf(stderr, ">>> Entering state machine\n"), fflush(stderr);
  d_processed += noutput_items;

  while (count < noutput_items)
    {
      switch (d_state)
        {

      case STATE_SYNC_SEARCH: // Look for sync vector
        if (VERBOSE)
          fprintf(stderr, "SYNC Search, noutput=%d syncvec=%x\n", noutput_items,
              d_sync_vector), fflush(stderr);

        while (count < noutput_items)
          {

            //if(inbuf[count++] == 0.0)
            //  continue;

            if (slice(inbuf[count++]))
              d_shift_reg = (d_shift_reg << 1) | 1;
            else
              d_shift_reg = d_shift_reg << 1;

            if (d_preamble_cnt > 0)
              {
                d_chip_cnt = d_chip_cnt + 1;
              }

            // The first if block syncronizes to chip sequences.
            if (d_preamble_cnt == 0)
              {
                unsigned int threshold;
                threshold = gr_count_bits32(
                    (d_shift_reg & 0x7FFFFFFE)
                        ^ (CHIP_MAPPING[0] & 0x7FFFFFFE));
                if (threshold < d_threshold)
                  {
                    //  fprintf(stderr, "Threshold %d d_preamble_cnt: %d\n", threshold, d_preamble_cnt);
                    //if ((d_shift_reg&0xFFFFFE) == (CHIP_MAPPING[0]&0xFFFFFE)) {
                    if (VERBOSE2)
                      {
                        fprintf(stderr, "Found 0 in chip sequence\n"), fflush(
                            stderr);
                      }
                    // we found a 0 in the chip sequence
                    d_preamble_cnt += 1;
                    //fprintf(stderr, "Threshold %d d_preamble_cnt: %d\n", threshold, d_preamble_cnt);
                  }
              }
            else
              {
                // we found the first 0, thus we only have to do the calculation every 32 chips
                if (d_chip_cnt == 32)
                  {
                    d_chip_cnt = 0;

                    if (d_packet_byte == 0)
                      {
                        if (gr_count_bits32(
                            (d_shift_reg & 0x7FFFFFFE)
                                ^ (CHIP_MAPPING[0] & 0xFFFFFFFE))
                            <= d_threshold)
                          {
                            if (VERBOSE2)
                              fprintf(stderr, "Found %d 0 in chip sequence\n",
                                  d_preamble_cnt), fflush(stderr);
                            // we found an other 0 in the chip sequence
                            d_packet_byte = 0;
                            d_preamble_cnt++;
                          }
                        else if (gr_count_bits32(
                            (d_shift_reg & 0x7FFFFFFE)
                                ^ (CHIP_MAPPING[7] & 0xFFFFFFFE))
                            <= d_threshold)
                          {
                            if (VERBOSE2)
                              fprintf(stderr, "Found first SFD\n",
                                  d_preamble_cnt), fflush(stderr);
                            d_packet_byte = 7 << 4;
                          }
                        else
                          {
                            // we are not in the synchronization header
                            if (VERBOSE2)
                              fprintf(stderr, "Wrong first byte of SFD. %u\n",
                                  d_shift_reg), fflush(stderr);
                            enter_search();
                            break;
                          }

                      }
                    else
                      {
                        if (gr_count_bits32(
                            (d_shift_reg & 0x7FFFFFFE)
                                ^ (CHIP_MAPPING[10] & 0xFFFFFFFE))
                            <= d_threshold)
                          {
                            d_packet_byte |= 0xA;
                            if (VERBOSE2)
                              fprintf(stderr, "Found sync, 0x%x\n",
                                  d_packet_byte), fflush(stderr);
                            // found SDF
                            // setup for header decode
                            enter_have_sync();
                            break;
                          }
                        else
                          {
                            if (VERBOSE)
                              fprintf(stderr, "Wrong second byte of SFD. %u\n",
                                  d_shift_reg), fflush(stderr);
                            enter_search();
                            break;
                          }
                      }
                  }
              }
          }
        break;

      case STATE_HAVE_SYNC:
        if (VERBOSE2)
          fprintf(stderr, "Header Search bitcnt=%d, header=0x%08x\n",
              d_headerbitlen_cnt, d_header), fflush(stderr);

        while (count < noutput_items)
          { // Decode the bytes one after another.
            if (slice(inbuf[count++]))
              d_shift_reg = (d_shift_reg << 1) | 1;
            else
              d_shift_reg = d_shift_reg << 1;

            d_chip_cnt = d_chip_cnt + 1;

            if (d_chip_cnt == 32)
              {
                d_chip_cnt = 0;
                unsigned char c = decode_chips(d_shift_reg);
                if (c == 0xFF)
                  {
                    // something is wrong. restart the search for a sync
                    // if (VERBOSE2)
                    fprintf(stderr, "Found a not valid chip sequence! %u\n",
                        d_shift_reg), fflush(stderr);

                    enter_search();
                    break;
                  }

                if (d_packet_byte_index == 0)
                  {
                    d_packet_byte = c;
                  }
                else
                  {
                    // c is always < 15
                    d_packet_byte |= c << 4;
                  }
                d_packet_byte_index = d_packet_byte_index + 1;
                if (d_packet_byte_index % 2 == 0)
                  {
                    // we have a complete byte which represents the frame length.
                    int frame_len = d_packet_byte;
                    if (frame_len <= MAX_PKT_LEN)
                      {
                        enter_have_header(frame_len);
                      }
                    else
                      {
                        enter_search();
                      }
                    break;
                  }
              }
          }
        break;

      case STATE_HAVE_HEADER:
        fprintf(stderr,
            "Packet Build count=%d, noutput_items=%d, packet_len=%d\n", count,
            noutput_items, d_packetlen), fflush(stderr);

        while (count < noutput_items)
          { // shift bits into bytes of packet one at a time
            if (slice(inbuf[count++]))
              d_shift_reg = (d_shift_reg << 1) | 1;
            else
              d_shift_reg = d_shift_reg << 1;

            d_chip_cnt = (d_chip_cnt + 1) % 32;

            if (d_chip_cnt == 0)
              {
                unsigned char c = decode_chips(d_shift_reg);
                if (c == 0xff)
                  {
                    // something is wrong. restart the search for a sync
                    if (VERBOSE)
                      fprintf(stderr, "Found a not valid chip sequence! %u\n",
                          d_shift_reg), fflush(stderr);

                    enter_search();
                    break;
                  }
                // the first symbol represents the first part of the byte.
                if (d_packet_byte_index == 0)
                  {
                    d_packet_byte = c;
                  }
                else
                  {
                    // c is always < 15
                    d_packet_byte |= c << 4;
                  }
                if (VERBOSE)
                  {
                    fprintf(stderr, "%d: 0x%x\n", d_packet_byte_index, c);
                    d_packet_byte_index = d_packet_byte_index + 1;
                  }
                if (d_packet_byte_index % 2 == 0)
                  {
                    // we have a complete byte
                    if (VERBOSE)
                      {
                        fprintf(stderr,
                            "packetcnt: %d, payloadcnt: %d, payload 0x%x, d_packet_byte_index: %d\n",
                            d_packetlen_cnt, d_payload_cnt, d_packet_byte,
                            d_packet_byte_index), fflush(stderr);
                      }

                    d_packet[d_packetlen_cnt++] = d_packet_byte;
                    d_payload_cnt++;
                    d_packet_byte_index = 0;

                    if (d_payload_cnt >= d_packetlen)
                      { // packet is filled, including CRC. might do check later in here
                        unsigned int scaled_lqi = (d_lqi / MAX_LQI_SAMPLES)
                            << 3;
                        unsigned char lqi = (
                            scaled_lqi >= 256 ? 255 : scaled_lqi);

                        // build an output with LQI
                        // Message will be:
                        // |-----------------|--------------------------|
                        // |    LQI (1 Byte) |   MPDU (variable length) |
                        // |-----------------|--------------------------|
                        //
                        // gr_message_sptr msg = gr_make_message(0, 0, 0,
                        // d_packetlen_cnt + sizeof(unsigned char));

                        //memcpy(msg->msg(), &lqi, sizeof(unsigned char));
                        //memcpy(msg->msg() + sizeof(unsigned char), d_packet,
                        // d_packetlen_cnt);

                        // d_target_queue->insert_tail(msg); // send it

                        // fprintf(stderr, "Adding message of size %d to queue\n",
                        //      d_packetlen_cnt);
                        // output of the packet here
                        // std::string result = msg->to_string();
                        // msg.reset(); // free it up
                        this->write_file(this->d_filename);
                        fprintf(stderr, "Packet payload: %x\n", d_packet), fflush(
                            stderr);
                        enter_search();
                        break;
                      }
                  }
              }
          }
        break;

      default:
        assert(0);
        break; // fixed (Marius)

        } // switch

    } // while
  // } // sensed

  if (VERBOSE2)
    fprintf(stderr, "Samples Processed: %d\n", d_processed), fflush(stderr);

  return noutput_items;
}


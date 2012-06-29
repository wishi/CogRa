#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Loopback 802.15.1
# Author: Marius Ciepluch
# Description: 802.15.1 loopback
# Generated: Tue Jun 12 19:36:20 2012
##################################################

execfile("/home/wishi_t/.grc_gnuradio/quadrature_demod.py")
from cogra import ieee_802_15_1_gfsk_mod
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import scopesink2
from grc_gnuradio import blks2 as grc_blks2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import SimpleXMLRPCServer
import threading
import wx

class IEEE_802_15_1_Loopback(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="Loopback 802.15.1")

		##################################################
		# Variables
		##################################################
		self.usrp_samp_rate = usrp_samp_rate = 1e6
		self.samp_rate = samp_rate = 10000
		self.rf_freq = rf_freq = 2441e6
		self.payload = payload = 0, 0, 0, 1, 1, 1
		self.gain = gain = 30
		self.freq_error = freq_error = 0
		self.freq = freq = 500
		self.fm_sensitivity = fm_sensitivity = 1.0
		self.baud = baud = 8

		##################################################
		# Blocks
		##################################################
		self.xmlrpc_server_0 = SimpleXMLRPCServer.SimpleXMLRPCServer(("localhost", 8080), allow_none=True)
		self.xmlrpc_server_0.register_instance(self)
		threading.Thread(target=self.xmlrpc_server_0.serve_forever).start()
		self.wxgui_scopesink2_0 = scopesink2.scope_sink_f(
			self.GetWin(),
			title="Scope Plot",
			sample_rate=samp_rate,
			v_scale=0,
			v_offset=0,
			t_scale=0,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
			trig_mode=gr.gr_TRIG_MODE_AUTO,
			y_axis_label="Counts",
		)
		self.Add(self.wxgui_scopesink2_0.win)
		self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
			self.GetWin(),
			baseband_freq=0,
			y_per_div=10,
			y_divs=10,
			ref_level=0,
			ref_scale=2.0,
			sample_rate=samp_rate,
			fft_size=1024,
			fft_rate=15,
			average=False,
			avg_alpha=None,
			title="FFT Plot",
			peak_hold=False,
		)
		self.Add(self.wxgui_fftsink2_0.win)
		self.quadrature_demod_0 = quadrature_demod()
		self.ieee_802_15_4_gfsk_mod_0 = ieee_802_15_1_gfsk_mod.gfsk_mod(
				samples_per_symbol=baud,
				sensitivity=1.0,
				bt=0.35,
				verbose=False,
				log=False,
				)
			
		self.gr_vector_source_x_0 = gr.vector_source_f((payload), True, 1)
		self.gr_throttle_0_0 = gr.throttle(gr.sizeof_float*1, 10000)
		self.gr_multiply_xx_0 = gr.multiply_vff(1)
		self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(baud*(1+freq_error), 0.25*0.175, 0.5, 0.175, 0.005)
		self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
		self.const_source_x_0 = gr.sig_source_f(0, gr.GR_CONST_WAVE, 0, 0, fm_sensitivity)
		self.blks2_packet_encoder_0 = grc_blks2.packet_mod_f(grc_blks2.packet_encoder(
				samples_per_symbol=2,
				bits_per_symbol=1,
				access_code="",
				pad_for_usrp=True,
			),
			payload_length=0,
		)
		self.blks2_packet_decoder_0_0 = grc_blks2.packet_demod_f(grc_blks2.packet_decoder(
				access_code="",
				threshold=-1,
				callback=lambda ok, payload: self.blks2_packet_decoder_0_0.recv_pkt(ok, payload),
			),
		)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_throttle_0_0, 0), (self.blks2_packet_encoder_0, 0))
		self.connect((self.blks2_packet_encoder_0, 0), (self.ieee_802_15_4_gfsk_mod_0, 0))
		self.connect((self.ieee_802_15_4_gfsk_mod_0, 0), (self.quadrature_demod_0, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
		self.connect((self.digital_binary_slicer_fb_0, 0), (self.blks2_packet_decoder_0_0, 0))
		self.connect((self.blks2_packet_decoder_0_0, 0), (self.wxgui_scopesink2_0, 0))
		self.connect((self.gr_vector_source_x_0, 0), (self.gr_throttle_0_0, 0))
		self.connect((self.quadrature_demod_0, 0), (self.gr_multiply_xx_0, 0))
		self.connect((self.const_source_x_0, 0), (self.gr_multiply_xx_0, 1))
		self.connect((self.gr_multiply_xx_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
		self.connect((self.ieee_802_15_4_gfsk_mod_0, 0), (self.wxgui_fftsink2_0, 0))

	def get_usrp_samp_rate(self):
		return self.usrp_samp_rate

	def set_usrp_samp_rate(self, usrp_samp_rate):
		self.usrp_samp_rate = usrp_samp_rate

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.wxgui_scopesink2_0.set_sample_rate(self.samp_rate)
		self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate)

	def get_rf_freq(self):
		return self.rf_freq

	def set_rf_freq(self, rf_freq):
		self.rf_freq = rf_freq

	def get_payload(self):
		return self.payload

	def set_payload(self, payload):
		self.payload = payload

	def get_gain(self):
		return self.gain

	def set_gain(self, gain):
		self.gain = gain

	def get_freq_error(self):
		return self.freq_error

	def set_freq_error(self, freq_error):
		self.freq_error = freq_error
		self.digital_clock_recovery_mm_xx_0.set_omega(self.baud*(1+self.freq_error))

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq

	def get_fm_sensitivity(self):
		return self.fm_sensitivity

	def set_fm_sensitivity(self, fm_sensitivity):
		self.fm_sensitivity = fm_sensitivity
		self.const_source_x_0.set_offset(self.fm_sensitivity)

	def get_baud(self):
		return self.baud

	def set_baud(self, baud):
		self.baud = baud
		self.digital_clock_recovery_mm_xx_0.set_omega(self.baud*(1+self.freq_error))

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = IEEE_802_15_1_Loopback()
	tb.Run(True)


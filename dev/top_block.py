#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Tue Jun 12 14:35:24 2012
##################################################

execfile("/home/wishi_t/.grc_gnuradio/quadrature_demod.py")
from cogra import ieee_802_15_1_gfsk_mod
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import scopesink2
from grc_gnuradio import blks2 as grc_blks2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import wx

class top_block(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="Top Block")

		##################################################
		# Variables
		##################################################
		self.sps = sps = 8
		self.samp_rate = samp_rate = 10000
		self.freq_error = freq_error = 0
		self.freq = freq = 500
		self.fm_sensitivity = fm_sensitivity = 1.0

		##################################################
		# Blocks
		##################################################
		self.wxgui_scopesink2_0_0 = scopesink2.scope_sink_f(
			self.GetWin(),
			title="GFSK Demod Spectrum",
			sample_rate=samp_rate,
			v_scale=0,
			v_offset=0,
			t_scale=1./freq,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
			trig_mode=gr.gr_TRIG_MODE_AUTO,
			y_axis_label="Counts",
		)
		self.Add(self.wxgui_scopesink2_0_0.win)
		self.quadrature_demod_0 = quadrature_demod()
		self.ieee_802_15_4_gfsk_mod_0 = ieee_802_15_1_gfsk_mod.gfsk_mod(
				samples_per_symbol=8,
				sensitivity=1.0,
				bt=0.35,
				verbose=False,
				log=False,
				)
			
		self.gr_vector_source_x_0 = gr.vector_source_f((0, 0, 0, 0, 1, 1, 1), True, 1)
		self.gr_throttle_0_0 = gr.throttle(gr.sizeof_float*1, samp_rate)
		self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(sps*(1+freq_error), 0.25*0.175, 0.5, 0.175, 0.005)
		self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
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
		self.connect((self.quadrature_demod_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
		self.connect((self.digital_binary_slicer_fb_0, 0), (self.blks2_packet_decoder_0_0, 0))
		self.connect((self.blks2_packet_decoder_0_0, 0), (self.wxgui_scopesink2_0_0, 0))
		self.connect((self.gr_vector_source_x_0, 0), (self.gr_throttle_0_0, 0))

	def get_sps(self):
		return self.sps

	def set_sps(self, sps):
		self.sps = sps
		self.digital_clock_recovery_mm_xx_0.set_omega(self.sps*(1+self.freq_error))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.wxgui_scopesink2_0_0.set_sample_rate(self.samp_rate)

	def get_freq_error(self):
		return self.freq_error

	def set_freq_error(self, freq_error):
		self.freq_error = freq_error
		self.digital_clock_recovery_mm_xx_0.set_omega(self.sps*(1+self.freq_error))

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq

	def get_fm_sensitivity(self):
		return self.fm_sensitivity

	def set_fm_sensitivity(self, fm_sensitivity):
		self.fm_sensitivity = fm_sensitivity

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = top_block()
	tb.Run(True)


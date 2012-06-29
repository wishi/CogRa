#
# @summary: GFSK modulation and demodulation.  
# @author: Marius Ciepluch et. al.
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

# See gnuradio-examples/python/digital for examples

from gnuradio import gr

# specific imports from the modulation toolbox
import gnuradio.digital.modulation_utils as modulation_utils
import gnuradio.digital.digital_swig as digital

from math import pi
import numpy
from pprint import pprint
import inspect




class gfsk_demod(gr.hier_block2):
    
    # default values (used in __init__ and add_options)
    # these remain unchanged
    _def_samples_per_symbol = 2
    _def_sensitivity = 1
    _def_bt = 0.35
    _def_verbose = False
    _def_log = False
    
    _def_gain_mu = None
    _def_mu = 0.5
    _def_freq_error = 0.0
    _def_omega_relative_limit = 0.005

   
    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 sensitivity=_def_sensitivity,
                 gain_mu=_def_gain_mu,
                 mu=_def_mu,
                 omega_relative_limit=_def_omega_relative_limit,
                 freq_error=_def_freq_error,
                 verbose=_def_verbose,
                 log=_def_log):
        """
        Hierarchical block for Gaussian Minimum Shift Key (GFSK)
        demodulation.
    
        The input is the complex modulated signal at baseband.
        The output is a stream of bits packed 1 bit per byte (the LSB)
    
        @param samples_per_symbol: samples per baud
        @type samples_per_symbol: integer
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Print modualtion data to files?
        @type log: bool 

        Clock recovery parameters.  These all have reasonble defaults.
        
        @param gain_mu: controls rate of mu adjustment
        @type gain_mu: float
        @param mu: fractional delay [0.0, 1.0]
        @type mu: float
        @param omega_relative_limit: sets max variation in omega
        @type omega_relative_limit: float, typically 0.000200 (200 ppm)
        @param freq_error: bit rate error as a fraction
        @param float
        """

        gr.hier_block2.__init__(self, "gfsk_demod",
                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                gr.io_signature(1, 1, gr.sizeof_char))       # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._gain_mu = gain_mu
        self._mu = mu
        self._omega_relative_limit = omega_relative_limit
        self._freq_error = freq_error
        self._differential = False
        

        if samples_per_symbol < 2:
            raise TypeError, "samples_per_symbol >= 2, is %f" % samples_per_symbol

        self._omega = samples_per_symbol*(1+self._freq_error)

        if not self._gain_mu:
            self._gain_mu = 0.175
            
        self._gain_omega = .25 * self._gain_mu * self._gain_mu        # critically damped
    
        # Demodulate FM
        #sensitivity = (pi / 2) / samples_per_symbol
        self.fmdemod = gr.quadrature_demod_cf(1.0 / sensitivity)
    
        # the clock recovery block tracks the symbol clock and resamples as needed.
        # the output of the block is a stream of soft symbols (float)
        self.clock_recovery = digital.clock_recovery_mm_ff(self._omega, self._gain_omega,
                                                               self._mu, self._gain_mu,
                                                               self._omega_relative_limit)

        # slice the floats at 0, outputting 1 bit (the LSB of the output byte) per sample
        self.slicer = digital.binary_slicer_fb()

        if verbose:
            self._print_verbage()
         
        if log:
            self._setup_logging()

        # Connect & Initialize base class
        self.connect(self, self.fmdemod, self.clock_recovery, self.slicer, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 1
        bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.


    def _print_verbage(self):
        print "bits per symbol = %d" % self.bits_per_symbol()
        print "M&M clock recovery omega = %f" % self._omega
        print "M&M clock recovery gain mu = %f" % self._gain_mu
        print "M&M clock recovery mu = %f" % self._mu
        print "M&M clock recovery omega rel. limit = %f" % self._omega_relative_limit
        print "frequency error = %f" % self._freq_error


    def _setup_logging(self):
        print "Demodulation logging turned on."
        self.connect(self.fmdemod,
                    gr.file_sink(gr.sizeof_float, "fmdemod.dat"))
        self.connect(self.clock_recovery,
                    gr.file_sink(gr.sizeof_float, "clock_recovery.dat"))
        self.connect(self.slicer,
                    gr.file_sink(gr.sizeof_char, "slicer.dat"))

    def add_options(parser):
        """
         Adds GFSK demodulation-specific options to the standard parser
        """
        parser.add_option("", "--gain-mu", type="float", default=_def_gain_mu,
                          help="M&M clock recovery gain mu [default=%default] (GFSK/PSK)")
        parser.add_option("", "--mu", type="float", default=_def_mu,
                          help="M&M clock recovery mu [default=%default] (GFSK/PSK)")
        parser.add_option("", "--omega-relative-limit", type="float", default=_def_omega_relative_limit,
                          help="M&M clock recovery omega relative limit [default=%default] (GFSK/PSK)")
        parser.add_option("", "--freq-error", type="float", default=_def_freq_error,
                          help="M&M clock recovery frequency error [default=%default] (GFSK)")
        add_options=staticmethod(add_options)

    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(gfsk_demod.__init__,
                                                            ('self',), options)
        extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)

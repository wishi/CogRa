/* -*- c++ -*- */

#define COGRA_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "cogra_swig_doc.i"


%{
#include "cogra_qpsk.h"
#include "cogra_symbols_to_chips_bi.h"
#include "cogra_chips_to_symbols_bi.h"
#include "cogra_ieee_802_15_4_simple_sink.h"
#include "cogra_ieee_802_15_4_sink.h"
%}


GR_SWIG_BLOCK_MAGIC(cogra,qpsk);
%include "cogra_qpsk.h"

GR_SWIG_BLOCK_MAGIC(cogra,symbols_to_chips_bi);
%include "cogra_symbols_to_chips_bi.h"
GR_SWIG_BLOCK_MAGIC(cogra,chips_to_symbols_bi);
%include "cogra_chips_to_symbols_bi.h"

GR_SWIG_BLOCK_MAGIC(cogra,ieee_802_15_4_simple_sink);
%include "cogra_ieee_802_15_4_simple_sink.h"

GR_SWIG_BLOCK_MAGIC(cogra,ieee_802_15_4_sink);
%include "cogra_ieee_802_15_4_sink.h"

## @summary:  define a map of channels according to the standard

class chan_802_15_4:
    """ @summary: a very simple channel map to lookup in GRC 
    simply put from cogra import ieee_802_15_4_channelmap in the import block
    and ieee_802_15_4_channelmap.chan_802_15_4.chan_map.get(11) into the block
    config. Ensure that you tune into the middle of the channel (+2,5 MHz)
    """
      
    chan_map = { 11 : 2405e6,
        12 : 2410e6,
        13 : 2415e6,
        14 : 2420e6,
        15 : 2425e6,
        16 : 2430e6,
        17 : 2435e6,
        18 : 2440e6,
        19 : 2445e6,
        20 : 2450e6,
        21 : 2455e6,
        22 : 2460e6,
        23 : 2465e6,
        24 : 2470e6,
        25 : 2475e6,
        26 : 2480e6}
    

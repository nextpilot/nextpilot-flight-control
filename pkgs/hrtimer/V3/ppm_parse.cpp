/*
 * Specific registers and bits used by PPM sub-functions
 */
#ifdef HRT_PPM_CHANNEL
/*
 * If the timer hardware doesn't support GTIM_CCER_CCxNP, then we will work around it.
 *
 * Note that we assume that M3 means STM32F1 (since we don't really care about the F2).
 */
#   ifdef CONFIG_ARCH_CORTEXM3
#      undef GTIM_CCER_CC1NP
#      undef GTIM_CCER_CC2NP
#      undef GTIM_CCER_CC3NP
#      undef GTIM_CCER_CC4NP
#      define GTIM_CCER_CC1NP 0
#      define GTIM_CCER_CC2NP 0
#      define GTIM_CCER_CC3NP 0
#      define GTIM_CCER_CC4NP 0
#      define PPM_EDGE_FLIP
#   endif
/* FIXME! There is an interaction in the CCMR registers that prevents using Chan 1 as the timer and chan 2 as the PPM*/
#   if HRT_PPM_CHANNEL == 1
#      define rCCR_PPM      rCCR1                                               /* capture register for PPM */
#      define DIER_PPM      GTIM_DIER_CC1IE                                     /* capture interrupt (non-DMA mode) */
#      define SR_INT_PPM    GTIM_SR_CC1IF                                       /* capture interrupt (non-DMA mode) */
#      define SR_OVF_PPM    GTIM_SR_CC1OF                                       /* capture overflow (non-DMA mode) */
#      define CCMR1_PPM     1                                                   /* not on TI1/TI2 */
#      define CCMR2_PPM     0                                                   /* on TI3, not on TI4 */
#      define CCER_PPM      (GTIM_CCER_CC1E | GTIM_CCER_CC1P | GTIM_CCER_CC1NP) /* CC1, both edges */
#      define CCER_PPM_FLIP GTIM_CCER_CC1P
#   elif HRT_PPM_CHANNEL == 2
#      define rCCR_PPM      rCCR2                                               /* capture register for PPM */
#      define DIER_PPM      GTIM_DIER_CC2IE                                     /* capture interrupt (non-DMA mode) */
#      define SR_INT_PPM    GTIM_SR_CC2IF                                       /* capture interrupt (non-DMA mode) */
#      define SR_OVF_PPM    GTIM_SR_CC2OF                                       /* capture overflow (non-DMA mode) */
#      define CCMR1_PPM     2                                                   /* not on TI1/TI2 */
#      define CCMR2_PPM     0                                                   /* on TI3, not on TI4 */
#      define CCER_PPM      (GTIM_CCER_CC2E | GTIM_CCER_CC2P | GTIM_CCER_CC2NP) /* CC2, both edges */
#      define CCER_PPM_FLIP GTIM_CCER_CC2P
#   elif HRT_PPM_CHANNEL == 3
#      define rCCR_PPM      rCCR3                                               /* capture register for PPM */
#      define DIER_PPM      GTIM_DIER_CC3IE                                     /* capture interrupt (non-DMA mode) */
#      define SR_INT_PPM    GTIM_SR_CC3IF                                       /* capture interrupt (non-DMA mode) */
#      define SR_OVF_PPM    GTIM_SR_CC3OF                                       /* capture overflow (non-DMA mode) */
#      define CCMR1_PPM     0                                                   /* not on TI1/TI2 */
#      define CCMR2_PPM     1                                                   /* on TI3, not on TI4 */
#      define CCER_PPM      (GTIM_CCER_CC3E | GTIM_CCER_CC3P | GTIM_CCER_CC3NP) /* CC3, both edges */
#      define CCER_PPM_FLIP GTIM_CCER_CC3P
#   elif HRT_PPM_CHANNEL == 4
#      define rCCR_PPM      rCCR4                                               /* capture register for PPM */
#      define DIER_PPM      GTIM_DIER_CC4IE                                     /* capture interrupt (non-DMA mode) */
#      define SR_INT_PPM    GTIM_SR_CC4IF                                       /* capture interrupt (non-DMA mode) */
#      define SR_OVF_PPM    GTIM_SR_CC4OF                                       /* capture overflow (non-DMA mode) */
#      define CCMR1_PPM     0                                                   /* not on TI1/TI2 */
#      define CCMR2_PPM     2                                                   /* on TI3, not on TI4 */
#      define CCER_PPM      (GTIM_CCER_CC4E | GTIM_CCER_CC4P | GTIM_CCER_CC4NP) /* CC4, both edges */
#      define CCER_PPM_FLIP GTIM_CCER_CC4P
#   else
#      error HRT_PPM_CHANNEL must be a value between 1 and 4
#   endif

/*
 * PPM decoder tuning parameters
 */
#   define PPM_MIN_PULSE_WIDTH   200  /**< minimum width of a valid first pulse */
#   define PPM_MAX_PULSE_WIDTH   600  /**< maximum width of a valid first pulse */
#   define PPM_MIN_CHANNEL_VALUE 800  /**< shortest valid channel signal */
#   define PPM_MAX_CHANNEL_VALUE 2200 /**< longest valid channel signal */
#   define PPM_MIN_START         2300 /**< shortest valid start gap (only 2nd part of pulse) */

/* decoded PPM buffer */
#   define PPM_MIN_CHANNELS 5
#   define PPM_MAX_CHANNELS 20

/** Number of same-sized frames required to 'lock' */
#   define PPM_CHANNEL_LOCK 4 /**< should be less than the input timeout */

__EXPORT uint16_t ppm_buffer[PPM_MAX_CHANNELS];
__EXPORT uint16_t ppm_frame_length      = 0;
__EXPORT unsigned ppm_decoded_channels  = 0;
__EXPORT uint64_t ppm_last_valid_decode = 0;

#   define PPM_DEBUG 0

#   if PPM_DEBUG
/* PPM edge history */
__EXPORT uint16_t ppm_edge_history[32];
unsigned          ppm_edge_next;

/* PPM pulse history */
__EXPORT uint16_t ppm_pulse_history[32];
unsigned          ppm_pulse_next;
#   endif

static uint16_t ppm_temp_buffer[PPM_MAX_CHANNELS];

/** PPM decoder state machine */
struct {
    uint16_t last_edge;    /**< last capture time */
    uint16_t last_mark;    /**< last significant edge */
    uint16_t frame_start;  /**< the frame width */
    unsigned next_channel; /**< next channel index */

    enum {
        UNSYNCH = 0,
        ARM,
        ACTIVE,
        INACTIVE
    } phase;
} ppm;

static void hrt_ppm_decode(uint32_t status);

#else
/* disable the PPM configuration */
#   define rCCR_PPM   0
#   define DIER_PPM   0
#   define SR_INT_PPM 0
#   define SR_OVF_PPM 0
#   define CCMR1_PPM  0
#   define CCMR2_PPM  0
#   define CCER_PPM   0
#endif /* HRT_PPM_CHANNEL */

#ifdef HRT_PPM_CHANNEL
/**
 * Handle the PPM decoder state machine.
 */
static void hrt_ppm_decode(uint32_t status) {
    uint16_t count = rCCR_PPM;
    uint16_t width;
    uint16_t interval;
    unsigned i;

    /* if we missed an edge, we have to give up */
    if (status & SR_OVF_PPM) {
        goto error;
    }

    /* how long since the last edge? - this handles counter wrapping implicitly. */
    width = count - ppm.last_edge;

#   if PPM_DEBUG
    ppm_edge_history[ppm_edge_next++] = width;

    if (ppm_edge_next >= 32) {
        ppm_edge_next = 0;
    }

#   endif

    /*
	 * if this looks like a start pulse, then push the last set of values
	 * and reset the state machine
	 */
    if (width >= PPM_MIN_START) {
        /*
		 * If the number of channels changes unexpectedly, we don't want
		 * to just immediately jump on the new count as it may be a result
		 * of noise or dropped edges.  Instead, take a few frames to settle.
		 */
        if (ppm.next_channel != ppm_decoded_channels) {
            static unsigned new_channel_count;
            static unsigned new_channel_holdoff;

            if (new_channel_count != ppm.next_channel) {
                /* start the lock counter for the new channel count */
                new_channel_count   = ppm.next_channel;
                new_channel_holdoff = PPM_CHANNEL_LOCK;

            } else if (new_channel_holdoff > 0) {
                /* this frame matched the last one, decrement the lock counter */
                new_channel_holdoff--;

            } else {
                /* we have seen PPM_CHANNEL_LOCK frames with the new count, accept it */
                ppm_decoded_channels = new_channel_count;
                new_channel_count    = 0;
            }

        } else {
            /* frame channel count matches expected, let's use it */
            if (ppm.next_channel >= PPM_MIN_CHANNELS) {
                for (i = 0; i < ppm.next_channel; i++) {
                    ppm_buffer[i] = ppm_temp_buffer[i];
                }

                ppm_last_valid_decode = hrt_absolute_time();
            }
        }

        /* reset for the next frame */
        ppm.next_channel = 0;

        /* next edge is the reference for the first channel */
        ppm.phase = ARM;

        ppm.last_edge = count;
        return;
    }

    switch (ppm.phase) {
    case UNSYNCH:
        /* we are waiting for a start pulse - nothing useful to do here */
        break;

    case ARM:

        /* we expect a pulse giving us the first mark */
        if (width < PPM_MIN_PULSE_WIDTH || width > PPM_MAX_PULSE_WIDTH) {
            goto error; /* pulse was too short or too long */
        }

        /* record the mark timing, expect an inactive edge */
        ppm.last_mark = ppm.last_edge;

        /* frame length is everything including the start gap */
        ppm_frame_length = (uint16_t)(ppm.last_edge - ppm.frame_start);
        ppm.frame_start  = ppm.last_edge;
        ppm.phase        = ACTIVE;
        break;

    case INACTIVE:

        /* we expect a short pulse */
        if (width < PPM_MIN_PULSE_WIDTH || width > PPM_MAX_PULSE_WIDTH) {
            goto error; /* pulse was too short or too long */
        }

        /* this edge is not interesting, but now we are ready for the next mark */
        ppm.phase = ACTIVE;
        break;

    case ACTIVE:
        /* determine the interval from the last mark */
        interval      = count - ppm.last_mark;
        ppm.last_mark = count;

#   if PPM_DEBUG
        ppm_pulse_history[ppm_pulse_next++] = interval;

        if (ppm_pulse_next >= 32) {
            ppm_pulse_next = 0;
        }

#   endif

        /* if the mark-mark timing is out of bounds, abandon the frame */
        if ((interval < PPM_MIN_CHANNEL_VALUE) || (interval > PPM_MAX_CHANNEL_VALUE)) {
            goto error;
        }

        /* if we have room to store the value, do so */
        if (ppm.next_channel < PPM_MAX_CHANNELS) {
            ppm_temp_buffer[ppm.next_channel++] = interval;
        }

        ppm.phase = INACTIVE;
        break;
    }

    ppm.last_edge = count;
    return;

    /* the state machine is corrupted; reset it */

error:
    /* we don't like the state of the decoder, reset it and try again */
    ppm.phase            = UNSYNCH;
    ppm_decoded_channels = 0;
}
#endif /* HRT_PPM_CHANNEL */

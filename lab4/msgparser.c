/*
 * msgparser.c -- Simple binary message format parser
 *
 * Message wire format (all integers little-endian):
 *
 *   Offset  Size  Field
 *   ------  ----  -----
 *      0      4   magic ("MSG\0")
 *      4      1   version
 *      5      1   flags
 *      6      2   num_records (uint16)
 *      8      *   records (variable)
 *
 * Each record:
 *   Offset  Size  Field
 *   ------  ----  -----
 *      0      2   type        (uint16)
 *      2      2   payload_len (uint16)
 *      4      *   payload     (payload_len bytes)
 */

#include "msgparser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

static inline uint16_t le16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

/* ------------------------------------------------------------------ */
/* count_payload_bytes                                                  */
/*                                                                      */
/* First pass: validate the record region and sum all payload lengths.  */
/* Returns the total number of payload bytes, or (size_t)-1 on error.  */
/*                                                                      */
/* NOTE: the accumulator `total' is a uint16_t.  When the sum of all   */
/* payload_len fields exceeds 65535 the value wraps silently and the    */
/* returned size is smaller than the actual data needed.               */
/* ------------------------------------------------------------------ */
static size_t count_payload_bytes(const uint8_t *p,
                                   const uint8_t *end,
                                   uint16_t        nrec)
{
    uint16_t total = 0;         /* BUG: should be size_t */

    for (uint16_t i = 0; i < nrec; i++) {
        if (p + 4 > end)
            return (size_t)-1;

        uint16_t plen = le16(p + 2);
        total += plen;          /* silent uint16_t wraparound possible */

        p += 4 + plen;
        if (p > end)
            return (size_t)-1;
    }
    return (size_t)total;
}

/* ------------------------------------------------------------------ */
/* populate_records                                                     */
/*                                                                      */
/* Second pass: fill the records[] array and copy each payload into     */
/* pool.  pool must be at least count_payload_bytes() bytes.           */
/* ------------------------------------------------------------------ */
static int populate_records(msg_record_t   *records,
                             uint16_t        nrec,
                             const uint8_t  *p,
                             const uint8_t  *end,
                             uint8_t        *pool)
{
    uint8_t *dst = pool;

    for (uint16_t i = 0; i < nrec; i++) {
        if (p + 4 > end)
            return -1;

        records[i].type        = le16(p);
        records[i].payload_len = le16(p + 2);
        records[i].payload     = dst;

        uint16_t plen = records[i].payload_len;
        if (p + 4 + (size_t)plen > end)
            return -1;

        /* When pool is undersized (due to the overflow in the first pass),
         * dst eventually goes past the end of the allocation and this
         * memcpy triggers a heap-buffer-overflow. */
        memcpy(dst, p + 4, plen);
        dst += plen;
        p   += 4 + plen;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int msg_parse(const uint8_t *data, size_t size, msg_t *out)
{
    if (!data || !out || size < 8)
        return -1;

    if (memcmp(data, "MSG\0", 4) != 0)
        return -1;

    out->version     = data[4];
    out->flags       = data[5];
    out->num_records = le16(data + 6);
    out->records     = NULL;
    out->data_pool   = NULL;

    if (out->num_records == 0)
        return 0;

    const uint8_t *rec_start = data + 8;
    const uint8_t *rec_end   = data + size;

    /* First pass: sum payload lengths.
     * pool_size may be smaller than reality if the sum wrapped. */
    size_t pool_size = count_payload_bytes(rec_start, rec_end,
                                           out->num_records);
    if (pool_size == (size_t)-1)
        return -1;

    out->records = (msg_record_t *)calloc(out->num_records, sizeof(msg_record_t));
    if (!out->records)
        return -1;

    /* Allocate the payload pool. May be too small if pool_size wrapped. */
    out->data_pool = (uint8_t *)malloc(pool_size + 1);
    if (!out->data_pool) {
        free(out->records);
        out->records = NULL;
        return -1;
    }

    /* Second pass: copy payloads. Overflows here if pool is undersized. */
    if (populate_records(out->records, out->num_records,
                          rec_start, rec_end, out->data_pool) < 0) {
        msg_free(out);
        return -1;
    }
    return 0;
}

int msg_process(const msg_t *msg)
{
    if (!msg || !msg->records)
        return -1;

    for (uint16_t i = 0; i < msg->num_records; i++) {
        const msg_record_t *r = &msg->records[i];
        if (!r->payload || r->payload_len == 0)
            continue;

        uint32_t sum = 0;
        for (uint16_t j = 0; j < r->payload_len; j++)
            sum += r->payload[j];

        // fprintf(stderr, "record[%u]: type=0x%04x  len=%-5u  chksum=0x%08x\n",
        //         i, r->type, r->payload_len, sum);
    }
    return 0;
}

void msg_free(msg_t *msg)
{
    if (!msg)
        return;
    free(msg->records);
    free(msg->data_pool);
    msg->records   = NULL;
    msg->data_pool = NULL;
}

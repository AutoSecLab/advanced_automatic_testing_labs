/*
 * msgparser.h -- Public API for the simple binary message format parser
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

#ifndef MSGPARSER_H
#define MSGPARSER_H

#include <stddef.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Data structures                                                      */
/* ------------------------------------------------------------------ */

/*
 * msg_record_t -- a single record extracted from the message body.
 *
 * `payload' points into the msg_t data_pool; it is valid for the
 * lifetime of the owning msg_t and must not be freed independently.
 */
typedef struct {
    uint16_t  type;         /* record type tag                        */
    uint16_t  payload_len;  /* number of bytes in payload             */
    uint8_t  *payload;      /* pointer into the parent msg_t data_pool */
} msg_record_t;

/*
 * msg_t -- a fully parsed message.
 *
 * After a successful msg_parse() call all fields are populated.
 * Call msg_free() to release the internally allocated memory.
 */
typedef struct {
    uint8_t       version;      /* protocol version byte              */
    uint8_t       flags;        /* flags byte                         */
    uint16_t      num_records;  /* number of records in `records'     */
    msg_record_t *records;      /* heap-allocated array of records    */
    uint8_t      *data_pool;    /* heap-allocated payload storage     */
} msg_t;

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

/*
 * msg_parse -- parse a raw byte buffer into a msg_t.
 *
 * On success returns 0 and populates *out.  The caller is responsible
 * for calling msg_free(out) when done.
 * Returns -1 on any parse error or allocation failure; *out is left in
 * a defined-but-empty state and need not be freed.
 */
int  msg_parse(const uint8_t *data, size_t size, msg_t *out);

/*
 * msg_process -- iterate over the records and print a checksum summary
 * to stderr.
 *
 * Returns 0 on success, -1 if msg or msg->records is NULL.
 */
int  msg_process(const msg_t *msg);

/*
 * msg_free -- release all memory owned by a msg_t.
 *
 * Safe to call on a zero-initialised or partially-initialised msg_t.
 * Does NOT free the msg_t struct itself.
 */
void msg_free(msg_t *msg);

#endif /* MSGPARSER_H */

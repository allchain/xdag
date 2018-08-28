/* block processing, T13.654-T14.390 $DVS:time$ */

#ifndef XDAG_BLOCK_H
#define XDAG_BLOCK_H

#include <stdint.h>
#include <stdio.h>
#include "hash.h"
#include "system.h"
#include "types.h"

enum xdag_field_type {
	XDAG_FIELD_NONCE,        //0
	XDAG_FIELD_HEAD,         //1
	XDAG_FIELD_IN,           //2
	XDAG_FIELD_OUT,          //3
	XDAG_FIELD_SIGN_IN,      //4
	XDAG_FIELD_SIGN_OUT,     //5
	XDAG_FIELD_PUBLIC_KEY_0, //6
	XDAG_FIELD_PUBLIC_KEY_1, //7
	XDAG_FIELD_HEAD_TEST,    //8
	XDAG_FIELD_REMARK,       //9
	XDAG_FIELD_RESERVE1,     //A
	XDAG_FIELD_RESERVE2,     //B
	XDAG_FIELD_RESERVE3,     //C
	XDAG_FIELD_RESERVE4,     //D
	XDAG_FIELD_RESERVE5,     //E
	XDAG_FIELD_RESERVE6      //F
};

enum xdag_message_type {
	XDAG_MESSAGE_BLOCKS_REQUEST,
	XDAG_MESSAGE_BLOCKS_REPLY,
	XDAG_MESSAGE_SUMS_REQUEST,
	XDAG_MESSAGE_SUMS_REPLY,
	XDAG_MESSAGE_BLOCKEXT_REQUEST,
	XDAG_MESSAGE_BLOCKEXT_REPLY,
	XDAG_MESSAGE_BLOCK_REQUEST,
};

enum bi_flags {
	BI_MAIN       = 0x01,
	BI_MAIN_CHAIN = 0x02,
	BI_APPLIED    = 0x04,
	BI_MAIN_REF   = 0x08,
	BI_REF        = 0x10,
	BI_OURS       = 0x20,
	BI_EXTRA      = 0x40,
};

#define XDAG_BLOCK_FIELDS 16

typedef char xdag_remark[32];

struct xdag_field {
	union {
		struct {
			union {
				struct {
					uint64_t transport_header;
					uint64_t type;
					xdag_time_t time;
				};
				xdag_hashlow_t hash;
			};
			union {
				xdag_amount_t amount;
				xdag_time_t end_time;
			};
		};
		xdag_hash_t data;
		xdag_remark remark;
	};
};

struct xdag_block {
	struct xdag_field field[XDAG_BLOCK_FIELDS];
};

#define xdag_type(b, n) ((b)->field[0].type >> ((n) << 2) & 0xf)

#ifdef __cplusplus
extern "C" {
#endif
	
// convert cheato to xdag
extern long double amount2xdags(xdag_amount_t amount);

// contert xdag to cheato
extern xdag_amount_t xdags2amount(const char *str);

// start of regular block processing
extern int xdag_blocks_start(int is_pool, int mining_threads_count, int miner_address);

// checks and adds block to the storage. Returns non-zero value in case of error.
extern int xdag_add_block(struct xdag_block *b);

// returns our first block. If there is no blocks yet - the first block is created.
extern int xdag_get_our_block(xdag_hash_t hash);

// calls callback for each own block
extern int xdag_traverse_our_blocks(void *data,
	int (*callback)(void*, xdag_hash_t, xdag_amount_t, xdag_time_t, int));

// calls callback for each block
extern int xdag_traverse_all_blocks(void *data, int (*callback)(void *data, xdag_hash_t hash,
	xdag_amount_t amount, xdag_time_t time));

// create and publish a block
extern int xdag_create_block(struct xdag_field *fields, int inputsCount, int outputsCount, int hasRemark, xdag_amount_t fee, 
	xdag_time_t send_time, xdag_hash_t newBlockHashResult);

// returns current balance for specified address or balance for all addresses if hash == 0
extern xdag_amount_t xdag_get_balance(xdag_hash_t hash);

// sets current balance for the specified address
extern int xdag_set_balance(xdag_hash_t hash, xdag_amount_t balance);

// calculates current supply by specified count of main blocks
extern xdag_amount_t xdag_get_supply(uint64_t nmain);

// returns position and time of block by hash; if block is extra and block != 0 also returns the whole block
extern int64_t xdag_get_block_pos(const xdag_hash_t hash, xdag_time_t *time, struct xdag_block *block);

// return state info string
extern const char* xdag_get_block_state_info(uint8_t flag);

// returns a number of the current period, period is 64 seconds
extern xdag_time_t xdag_main_time(void);

// returns the number of the time period corresponding to the start of the network
extern xdag_time_t xdag_start_main_time(void);

// returns a number of key by hash of block or -1 if block is not ours
extern int xdag_get_key(xdag_hash_t hash);

// reinitialization of block processing
extern int xdag_blocks_reset(void);

// prints detailed information about block
extern int xdag_print_block_info(xdag_hash_t hash, FILE *out);

// prints list of N last main blocks
extern void xdag_list_main_blocks(int count, int print_only_addresses, FILE *out);

// prints list of N last blocks mined by current pool
extern void xdag_list_mined_blocks(int count, int include_non_payed, FILE *out);

// calculate difficulty from hash
xdag_diff_t xdag_hash_difficulty(xdag_hash_t hash);

// get all transactions of specified address, and return total number of transactions
extern int xdag_get_transactions(xdag_hash_t hash, void *data, int (*callback)(void*, int, int, xdag_hash_t, xdag_amount_t, xdag_time_t, xdag_remark));

// print orphan blocks
void xdag_list_orphan_blocks(int, FILE*);

// completes work with the blocks
void xdag_block_finish(void);
	
// get block info of specified address
extern int xdag_get_block_info(xdag_hash_t hash, void *data, int (*callback)(void*, int, xdag_hash_t, xdag_amount_t, xdag_time_t, xdag_remark));

#ifdef __cplusplus
};
#endif

#endif

#define F_IA64
#define NAPI_EXPERIMENTAL
#include <node_api.h>
#include "f_nano_crypto_util.h"

#define F_BUF_CHAR (size_t)512
static char _buf[F_BUF_CHAR];

#define CANT_PARSE_JAVASCRIPT_ARGS "Can't parse arguments"
#define ERROR_PARSE_CONST "Unable to INIT constant"
#define ERROR_CONST_NUM "200"
#define PARSE_ERROR "101"
#define ERROR_MISSING_ARGS "Missing arguments"
#define ERROR_DIGITS_A_TOO_LONG "Digits in Nano big number A too long"
#define ERROR_DIGITS_B_TOO_LONG "Digits in Nano big number B too long"
#define ERROR_TOO_MANY_ARGUMENTS "Too many arguments"
#define ERROR_ADD_SUB_BIG_NUMBERS "Error when ADD/SUB Nano big numbers"
#define ERROR_UNABLE_TO_CREATE_ATTRIBUTE "Unable to create attribute"
#define ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK "Can't add attribute to Nano in JSON block"
#define NANOJS_NAPI_INIT_ERROR "myNanoJS internal error in C function \"%s\" %s"
#define ERROR_CANT_READ_NANO_BLOCK "Can't read Nano block"
#define ERROR_CANT_READ_NANO_BLOCK_NUMBER "162"
#define WRONG_NANO_BLOCK_SZ "Wrong Nano block size"
#define WRONG_NANO_BLOCK_SZ_ERR "163"
#define ERROR_INVALID_NANO_BLK "Invalid Nano Block"
#define ERROR_INVALID_NANO_BLK_NUMBER "164"

#define VALUE_TO_SEND F_NANO_SUB_A_B
#define VALUE_TO_RECEIVE F_NANO_ADD_A_B
#define BALANCE_RAW_128 F_NANO_A_RAW_128
#define BALANCE_REAL_STRING F_NANO_A_REAL_STRING
#define BALANCE_RAW_STRING F_NANO_A_RAW_STRING
#define VALUE_SEND_RECEIVE_RAW_128 F_NANO_B_RAW_128
#define VALUE_SEND_RECEIVE_REAL_STRING F_NANO_B_REAL_STRING
#define VALUE_SEND_RECEIVE_RAW_STRING F_NANO_B_RAW_STRING
#define REAL_TO_RAW (int)(1<<8)
#define RAW_TO_REAL (int)(1<<13)
#define REAL_TO_HEX (int)(1<<9)
#define HEX_TO_REAL (int)(1<<10)
#define RAW_TO_HEX (int)(1<<11)
#define HEX_TO_RAW (int)(1<<12)

typedef int (*attr_fn)(napi_env, napi_value, void *);
typedef napi_value (*my_nano_fn)(napi_env, napi_callback_info);

typedef struct my_nano_js_fn_call_t {
   const char *function_name;
   my_nano_fn fn;
} MY_NANO_JS_FUNCTION;

typedef struct constant_char_t { const char *constant_name, *constant; } MY_NANO_JS_CONST_CHAR;

typedef struct constant_uint32_t_t {
   const char *constant_name;
   uint32_t constant;
} MY_NANO_JS_CONST_UINT32_T;

typedef struct constant_uint64_t_t {
   const char *constant_name;
   uint64_t constant;
} MY_NANO_JS_CONST_UINT64_T;

void gen_rand_no_entropy(void *, size_t);
int extract_public_key_from_wallet_or_hex_str_util(int *, uint8_t *, char *, size_t);
int mynanojs_add_nano_function_util(napi_env, napi_value, void *);
int mynanojs_add_char_constant_util(napi_env, napi_value, void *);
int mynanojs_add_uint32_constant_util(napi_env, napi_value, void *);
int mynanojs_add_uint64_constant_util(napi_env, napi_value, void *);
int mynanojs_add_init_property(const char *, napi_env, napi_value, attr_fn, void *);


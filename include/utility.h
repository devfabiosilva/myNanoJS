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
#define ERROR_FATAL_MALLOC "Unable to alloc memory"
#define ERROR_EMPTY_ARRAY_BUFFER_MSG "Empty ArrayBuffer"
#define ERROR_EMPTY_ARRAY_BUFFER_ERR "185"
#define ERROR_FILE_PATH_TOO_LONG_MSG "Dictionary filepath/name too long"
#define ERROR_FILE_PATH_LONG "196"
#define ERROR_PARSE_FILE_AND_PATH_MSG "Can't parse dictionary file and path to C string"
#define ERROR_PARSE_FILE_AND_PATH "195"
#define ERROR_WRONG_NANO_SEED_SIZE_MSG "Wrong Nano SEED size"
#define ERROR_WRONG_NANO_SEED_SIZE "106"
#define ERROR_CANT_CONVERT_NANO_SEED_TO_BINARY "Can't convert Nano SEED to binary"
#define ERROR_CANT_GENERATE_ENTROPY "Can't generate Nano SEED given entropy %s"
#define ERROR_CANT_PARSE_PASSWORD_MSG "Can't parse Password to C string"
#define ERROR_CANT_PARSE_PASSWORD "501"
#define ERROR_CANT_PARSE_PRIVATE_KEY "578"
#define ERROR_CANT_PARSE_PRIVATE_KEY_MSG "Can't parse private key to sign message"
#define NO_ENTROPY_FOUND_MSG "No entropy found"
#define NO_ENTROPY_FOUND "515"
#define ERROR_EMPTY_PASSWORD_MSG "Empty password"
#define ERROR_EMPTY_PASSWORD "508"
#define ERROR_PASSWORD_TOO_LONG_MSG "Password is too long"
#define ERROR_PASSWORD_TOO_LONG "509"
#define ERROR_CANT_PARSE_BIP39_TO_C_STR_MSG "Can't parse Bip39 to C string"
#define ERROR_CANT_PARSE_BIP39_TO_C_STR "193"
#define ERROR_BIP39_TOO_LONG_MSG "Bip39 too long"
#define ERROR_BIP39_TOO_LONG "194"
#define ERROR_CANT_PARSE_BIP39_TO_SEED "Can't parse Bip39 to Nano SEED %s"
#define ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK_MSG "Can't create array buffer to store encrypted Nano Block"
#define ERROR_CANT_CREATE_ARRAY_BUFFER_ENC_BLOCK "513"
#define ERROR_CANT_EXPORT_WALLET_NUMBER_MSG "Can't export wallet number"
#define ERROR_CANT_EXPORT_WALLET_NUMBER "112"
#define ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER_MSG "Can't copy array buffer to store encrypted Nano Block in Javascript ArrayBuffer"
#define ERROR_CANT_PARSE_ENCRIPTED_STREAM_TO_ARRAY_BUFFER "514"
#define ERROR_INTERNAL_C_FUNCTION_CREATE_OBJECT_MSG "Error in internal C function 'create_object_keypair_util' %s"
#define ERROR_CANT_READ_ENCRYPTED_STREAM "Internal error. Can't read encrypted stream %s"
#define ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM_MSG "Unable to parse ArrayBuffer to open encrypted stream"
#define ERROR_UNABLE_TO_PARSE_ARRAY_BUFFER_TO_ENCRTYPTED_STREAM "199"
#define ERROR_WRONG_ENCRYPTED_STREAM_SIZE_MSG "Wrong encrypted stream size"
#define ERROR_WRONG_ENCRYPTED_STREAM_SIZE "500"
#define ERROR_CANT_PARSE_NANO_XRB_PREFIX_MSG "Can't parse Nano/Xrb prefix"
#define ERROR_CANT_PARSE_NANO_XRB_PREFIX "516"
#define ERROR_WRONG_PREFIX_SIZE_MSG "Wrong prefix size"
#define ERROR_WRONG_PREFIX_SIZE "517"
#define ERROR_CREATING_OBJECT_MSG "myNanoEmbedded C error on creating object"
#define ERROR_CREATING_OBJECT "518"
#define ERROR_PARSING_BASE_DIFFICULTY_MSG "Error when parsing base diffuculty to big int"
#define ERROR_PARSING_BASE_DIFFICULTY "543"
#define ERROR_PRECISION_LOSS_DIFFICULTY_MSG "Precision loss in base difficulty big int"
#define ERROR_PRECISION_LOSS_DIFFICULTY "544"
#define WRONG_PRIVATE_KEY_SZ_MSG "Wrong private key size"
#define WRONG_PRIVATE_KEY_SZ "175"
#define ERROR_CANT_PARSE_PRIVATE_KEY_TO_BINARY "Can't parse private key to binary hex %s"
#define ERROR_NANO_WALLET_PK_TOO_LONG_MSG "Public key/Nano Wallet too long"
#define ERROR_NANO_WALLET_PK_TOO_LONG "187"
#define ERROR_CANT_WRITE_REP_WALLET_PK_TO_BLOCK_MSG "Can't write Representative Nano Wallet/Public key to block %s"
#define ERROR_CANT_WRITE_LINK_TO_BLOCK_MSG "Can't write link to block %s"
#define ERROR_CANT_CALCULATE_HASH_BLOCK_MSG "Can't calculate block hash %s"
#define ERROR_COULD_PERFORM_BIG_NUMBER_COMPARE_MSG "Could not perform a Big number compare %s"
#define ERROR_CANT_PARSE_P2POW_BLOCK "553"
#define ERROR_CANT_PARSE_P2POW_BLOCK_MSG "Can't parse P2PoW block"
#define ERROR_WRONG_P2POW_SIZE "554"
#define ERROR_WRONG_P2POW_SIZE_MSG "Wrong P2PoW size"
#define ERROR_INVALID_P2POW_BLOCK "555"
#define ERROR_INVALID_P2POW_BLOCK_MSG "Invalid P2PoW block"
#define NANO_STATE "state"
#define NANO_ACCOUNT "account"
#define NANO_PREVIOUS "previous"
#define NANO_REPRESENTATIVE "representative"
#define NANO_BALANCE "balance"
#define NANO_LINK "link"
#define NANO_LINK_AS_ACCOUNT "link_as_account"
#define NANO_SIGNATURE "signature"
#define P2POW_USER_TRANSACTION "user_block"
#define P2POW_WORKER_TRANSACTION "reward_block"
#define ERROR_THRESHOLD_BIG_INT_MSG "Precision loss in threshold big int"
#define ERROR_THRESHOLD_BIG_INT "125"
#define ERROR_INTERNAL_ERROR_MSG "Internal error in myNanoEmbedded C function 'f_nano_pow'"
#define ERROR_PARSING_NO_OF_THREADS "126"
#define ERROR_PARSING_NO_OF_THREADS_MSG "Error when parsing CPU number of threads"
#define ERROR_CANT_STORE_NANO_BLOCK "155"
#define ERROR_CANT_STORE_NANO_BLOCK_MSG "Can't create array buffer to store Nano Block"
#define ERROR_CANT_COPY_NANO_BLOCK "156"
#define ERROR_CANT_COPY_NANO_BLOCK_MSG "Can't copy to array buffer to store Nano Block in Javascript"

#define MIN_PASSWORD_SZ (size_t)18
#define PASS_MUST_HAVE (int)(F_PASS_MUST_HAVE_AT_LEAST_ONE_NUMBER|F_PASS_MUST_HAVE_AT_LEAST_ONE_SYMBOL|F_PASS_MUST_HAVE_AT_LEAST_ONE_UPPER_CASE|F_PASS_MUST_HAVE_AT_LEAST_ONE_LOWER_CASE)

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
void memory_flush(void);
char *verify_password_util(char *, int);
int filter_no_entropy_util(uint32_t);
int seed2keypair_util(char *, char **, char **, uint32_t, const char *, int);
int create_object_keypair_util(napi_env, napi_value, char *, char *, uint32_t, char *);
int extract_public_key_from_wallet_or_hex_str_util(int *, uint8_t *, char *, size_t);
int parse_and_adjust_big_number_hex128_string_balance_util(uint8_t *, char *, size_t);
int p2pow_block_json_util(napi_env, napi_value, char *, size_t, const char *, F_BLOCK_TRANSFER *);
int mynanojs_add_nano_function_util(napi_env, napi_value, void *);
int mynanojs_add_char_constant_util(napi_env, napi_value, void *);
int mynanojs_add_uint32_constant_util(napi_env, napi_value, void *);
int mynanojs_add_uint64_constant_util(napi_env, napi_value, void *);
int mynanojs_add_init_property(const char *, napi_env, napi_value, attr_fn, void *);


// Bitcoin utility
napi_value bitcoin_private_key_to_wif(napi_env env, napi_callback_info info);
napi_value bitcoin_wif_to_private_key(napi_env env, napi_callback_info info);



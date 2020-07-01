#define F_IA64
#define NAPI_EXPERIMENTAL
#include <node_api.h>
#include "f_nano_crypto_util.h"

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
//27 de junho de 2019 15:43

#define VALUE_TO_SEND F_NANO_SUB_A_B
#define VALUE_TO_RECEIVE F_NANO_ADD_A_B
#define BALANCE_RAW_128 F_NANO_A_RAW_128
#define BALANCE_REAL_STRING F_NANO_A_REAL_STRING
#define BALANCE_RAW_STRING F_NANO_A_RAW_STRING
#define VALUE_SEND_RECEIVE_RAW_128 F_NANO_B_RAW_128
#define VALUE_SEND_RECEIVE_REAL_STRING F_NANO_B_REAL_STRING
#define VALUE_SEND_RECEIVE_RAW_STRING F_NANO_B_RAW_STRING
#define REAL_TO_RAW (int)(1<<8)
#define RAW_TO_REAL F_RAW_TO_STR_STRING
#define REAL_TO_HEX (int)(1<<9)
#define HEX_TO_REAL (int)(1<<10)
#define RAW_TO_HEX (int)(1<<11)
#define HEX_TO_RAW (int)(1<<12)

#define F_BUF_CHAR (size_t)512
static char _buf[F_BUF_CHAR];

void gen_rand_no_entropy(void *output, size_t output_len)
{
   FILE *f;
   size_t rnd_sz, left;

   if (!(f=fopen("/dev/urandom", "r")))
      return;

   rnd_sz=0;
   left=output_len;

   while ((rnd_sz+=fread(output+rnd_sz, 1, left, f))<output_len)
      left-=rnd_sz;

   fclose(f);

   return;

}

void memory_flush() {
   memset(_buf, 0, sizeof(_buf));
}

int extract_public_key_from_wallet_or_hex_str_util(int *prefix, uint8_t *dest, char *wallet_or_pk, size_t wallet_or_pk_sz)
{
// if prefix!=NULL then prefix is parsed
// Internal use only !!!
// *wallet_or_pk is manipulated as well
   int err;
   uint8_t msg[PUB_KEY_EXTENDED_MAX_LEN];

   if (!wallet_or_pk_sz)
      return 50;

   if (wallet_or_pk_sz>(MAX_STR_NANO_CHAR-1))
      return 51;

   wallet_or_pk[wallet_or_pk_sz]=0;

   if (prefix)
      *prefix=is_nano_prefix((const char *)wallet_or_pk, XRB_PREFIX);

   if ((err=nano_base_32_2_hex(msg, (char *)wallet_or_pk))) {

      if (wallet_or_pk_sz!=64)
         return err;

      return f_str_to_hex(dest, (char *)wallet_or_pk);

   }

   memcpy(dest, msg, 32);
   return 0;
}

napi_value nanojs_license(napi_env env, napi_callback_info info)
{
   napi_value license, argv;
   size_t argc=1;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)LICENSE, sizeof(LICENSE)-1, &license)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, "Fail on parse LICENSE");
      return NULL;
   }

   return license;

}

napi_value nanojs_wallet_to_public_key(napi_env env, napi_callback_info info) 
{
   int err;
   size_t nano_wallet_sz, argc=1;
   napi_value argv, res;
   char nano_wallet[MAX_STR_NANO_CHAR];

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>1) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, "2", "Missing Nano/Xrb Wallet");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv, nano_wallet, sizeof(nano_wallet)+1, &nano_wallet_sz)!=napi_ok) {
      napi_throw_error(env, "102", "Can't parse public key to myNanoEmbedded C library");
      return NULL;
   }

   if (nano_wallet_sz==MAX_STR_NANO_CHAR) {
      napi_throw_error(env, "123", "String excess greater than MAX_STR_NANO_CHAR");
      return NULL;
   }

   nano_wallet[nano_wallet_sz]=0;

   if ((err=nano_base_32_2_hex((uint8_t *)(_buf+128), nano_wallet))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Can't convert Nano wallet to binary");
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)(_buf+128)), 64, &res)!=napi_ok) {
      napi_throw_error(env, "100", "Failed when parse binary to string");
      return NULL;
   }

   return res;
}

napi_value nanojs_seed_to_nano_wallet(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[2], res, wn;
   size_t argc=2, sz_tmp;
   uint32_t wallet_number;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc!=2) {
      napi_throw_error(env, "107", "Required: Nano SEED and Wallet number");
      return NULL;
   }

   if (napi_get_value_uint32(env, argv[1], &wallet_number)!=napi_ok) {
      napi_throw_error(env, "108", "Wrong wallet number");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "103", "Can't parse Nano SEED to myNanoEmbedded C library");
      return NULL;
   }

   if (sz_tmp!=64) {
      napi_throw_error(env, "106", "Wrong Nano SEED size");
      return NULL;
   }

   _buf[64]=0;

   if ((err=f_str_to_hex((uint8_t *)(_buf+128), _buf))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Can't convert Nano SEED to binary");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if ((err=f_seed_to_nano_wallet((uint8_t *)_buf, (uint8_t *)(_buf+64), (uint8_t *)(_buf+128), wallet_number))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Can't extract wallet number");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str((_buf+128), (unsigned char *)_buf), 64, &argv[0])!=napi_ok) {
      napi_throw_error(env, "110", "Can't export private key from Nano SEED");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str((_buf+128), (unsigned char *)(_buf+64)), 64, &argv[1])!=napi_ok) {
      napi_throw_error(env, "111", "Can't export private key from Nano SEED");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_create_uint32(env, wallet_number, &wn)!=napi_ok) {
      napi_throw_error(env, "112", "Can't export wallet number");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "104", "myNanoEmbedded C error. Can't create object in 'nanojs_seed_to_nano_wallet'");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "walletNumber", wn)!=napi_ok) {
      napi_throw_error(env, "113", "myNanoEmbedded C error. Can't set walletNumber property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "privateKey", argv[0])!=napi_ok) {
      napi_throw_error(env, "105", "myNanoEmbedded C error. Can't set privateKey property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "publicKey", argv[1])!=napi_ok) {
      napi_throw_error(env, "109", "myNanoEmbedded C error. Can't set publicKey property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_seed_to_nano_wallet_EXIT1;
   }

   memory_flush();
   return res;

nanojs_seed_to_nano_wallet_EXIT1:
   memory_flush();
   return NULL;

}

napi_value nanojs_add_sub(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint32_t add_sub_type;
   char *A, *B, *Result, *Tmp1, *Tmp2;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, "114", ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, "1", ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==3) {
      if (napi_get_value_uint32(env, argv[2], &add_sub_type)!=napi_ok) {
         napi_throw_error(env, "115", "Wrong Nano Big number ADD/SUB mode");
         return NULL;
      }
   } else
      add_sub_type=F_NANO_RES_REAL_STRING|F_NANO_ADD_A_B|F_NANO_A_REAL_STRING|F_NANO_B_REAL_STRING;

   if (napi_get_value_string_utf8(env, argv[0], A=_buf, F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "116", "Can't parse Nano Big Number A value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "117", ERROR_DIGITS_A_TOO_LONG);
      return NULL;
   }

   A[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], B=(A+F_RAW_STR_MAX_SZ), F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "118", "Can't parse Nano Big Number B value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "119", ERROR_DIGITS_B_TOO_LONG);
      return NULL;
   }

   B[sz_tmp]=0;

   Result=(B+F_RAW_STR_MAX_SZ);
   Tmp1=(Result+F_RAW_STR_MAX_SZ);
   Tmp2=(Tmp1+F_RAW_STR_MAX_SZ);

   if (add_sub_type&F_NANO_A_RAW_128) {
      if ((err=f_str_to_hex((uint8_t *)Tmp1, A))) {
         napi_throw_error(env, "120", "Can't parse Nano Big Number A to binary");
         return NULL;
      }

      A=Tmp1;
   }

   if (add_sub_type&F_NANO_B_RAW_128) {
      if ((err=f_str_to_hex((uint8_t *)Tmp2, B))) {
         napi_throw_error(env, "121", "Can't parse Nano Big Number B to binary");
         return NULL;
      }

      B=Tmp2;
   }

   if ((add_sub_type&(F_NANO_RES_RAW_128|F_NANO_RES_RAW_STRING|F_NANO_RES_REAL_STRING))==0)
      add_sub_type|=F_NANO_RES_REAL_STRING;

   if ((err=f_nano_add_sub(Result, A, B, add_sub_type))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, ERROR_ADD_SUB_BIG_NUMBERS);
      return NULL;
   }

   if (add_sub_type&F_NANO_RES_RAW_128)
      Result=fhex2strv2(A, (const void *)Result, sizeof(f_uint128_t), 0);

   if (napi_create_string_utf8(env, (const char *)Result, NAPI_AUTO_LENGTH, &res)!=napi_ok) {
      napi_throw_error(env, "122", "Can't perform result operation");
      return NULL;
   }

   return res;

}

napi_value nanojs_pow(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint64_t threshold;
   int32_t n_thr;
   bool lossless;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==2)
      threshold=F_DEFAULT_THRESHOLD;
   else {
      if (napi_get_value_bigint_uint64(env, argv[2], &threshold, &lossless)!=napi_ok) {
         napi_throw_error(env, "124", "Error when parsing PoW threshold");
         return NULL;
      }

      if (!lossless) {
         napi_throw_error(env, "125", "Precision loss in threshold big int");
         return NULL;
      }
   }

   if (napi_get_value_int32(env, argv[1], &n_thr)!=napi_ok) {
      napi_throw_error(env, "126", "Error when parsing CPU number of threads");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "127", "Can't parse Nano Big Number B value");
      return NULL;
   }

   if (sz_tmp!=64) {
      sprintf(_buf, "Wrong hash size %lu to calculate PoW", (unsigned long int)sz_tmp);
      napi_throw_error(env, "128", _buf);
      return NULL;
   }

   _buf[64]=0;

   if ((err=f_str_to_hex((uint8_t *)(_buf+128), _buf))) {
      sprintf((_buf+128), "Can't convert hash \"%s\" to binary", _buf);
      napi_throw_error(env, "129", (_buf+128));
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy);

   if ((err=f_nano_pow((uint64_t *)_buf, (unsigned char *)(_buf+128), (const uint64_t)threshold, (int)n_thr))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Internal error in myNanoEmbedded C function 'f_nano_pow'");
   }

   f_random_detach();

   if (err)
      return NULL;

   if (napi_create_bigint_uint64(env, (uint64_t)*((uint64_t *)_buf), &res)!=napi_ok) {
      napi_throw_error(env, "130", "Can't parse result threshold JavaScript environment");
      return NULL;
   }

   return res;
}

napi_value nanojs_extract_seed_from_brainwallet(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint32_t allow_mode;
   char *warning_msg, *brainwallet, *salt, *seed;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==3) {

      if (napi_get_value_uint32(env, argv[2], &allow_mode)!=napi_ok) {
         napi_throw_error(env, "131", "Can't get Brainwallet allow mode");
         return NULL;
      }

   } else
      allow_mode=F_BRAIN_WALLET_PERFECT;

   if (napi_get_value_string_utf8(env, argv[0], brainwallet=_buf, (sizeof(_buf)>>1)+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "132", "Can't parse Brainwallet text");
      return NULL;
   }

   if (sz_tmp==(sizeof(_buf)>>1)) {
      napi_throw_error(env, "134", "Brainwallet text too long");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   brainwallet[sz_tmp]=0;

   if (napi_get_value_string_utf8(env, argv[1], salt=(_buf+(sizeof(_buf)>>1)), (sizeof(_buf)>>1)+1-32, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "135", "Can't parse Salt text");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (sz_tmp==((sizeof(_buf)>>1)-32)) {
      napi_throw_error(env, "136", "Salt text is too long");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   salt[sz_tmp]=0;

   if ((err=f_extract_seed_from_brainwallet((uint8_t *)(seed=(_buf+sizeof(_buf)-32)), &warning_msg, allow_mode, (const char *)brainwallet, (const char *)salt))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, 
         "Error in myNanoEmbedded function 'f_extract_seed_from_brainwallet' %s. Can't extract Nano SEED from brainwallet. Warning message: %s", _buf, warning_msg);
      napi_throw_error(env, _buf, _buf+128);
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "137", "myNanoEmbedded C error. Can't create object in 'nanojs_extract_seed_from_brainwallet'");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)seed), 64, &argv[0])!=napi_ok) {
      napi_throw_error(env, "138", "Can't export NANO SEED from Brainwallet");
      goto  nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "seed", argv[0])!=napi_ok) {
      napi_throw_error(env, "139", "myNanoEmbedded C error. Can't set 'seed' property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_create_string_utf8(env, (const char *)warning_msg, NAPI_AUTO_LENGTH, &argv[1])!=napi_ok) {
      napi_throw_error(env, "140", "Can't export warning message");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   if (napi_set_named_property(env, res, "warningMessage", argv[1])!=napi_ok) {
      napi_throw_error(env, "141", "myNanoEmbedded C error. Can't set 'warningMessage' property to 'nanojs_seed_to_nano_wallet'");
      goto nanojs_extract_seed_from_brainwallet_EXIT1;
   }

   memory_flush();
   return res;

nanojs_extract_seed_from_brainwallet_EXIT1:
   memory_flush();
   return NULL;
}

//account 0
//previous 1
//representative 2
//balance 3
//balance_type 4
//value_to_send_or_receive 5
//value_to_send_or_receive_type 6
//link or destination address 7
//direction 8
napi_value nanojs_create_block(napi_env env, napi_callback_info info)
{
   int err, xrb_prefix;
   napi_value argv[9], res;
   size_t argc=9, sz_tmp;
   uint8_t *p;
   char *Balance, *Value_To_Send_Rec;
   uint32_t type, type_tmp;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>9) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc<9) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "142", "Can't parse account to Nano block");
      return NULL;
   }

   memset(&nano_block, 0, sizeof(nano_block));

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.account, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write Nano Wallet/Public key to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes=SENDER_XRB;

   if (napi_get_value_string_utf8(env, argv[1], _buf+128, (sizeof(_buf)-128), &sz_tmp)==napi_ok) {
      if (sz_tmp==64) {
         if ((err=f_str_to_hex((uint8_t *)_buf, _buf+128))) {
            napi_throw_error(env, "144", "Error when parsing previous block to binary");
            return NULL;
         }

         p=(uint8_t *)_buf;
      } else if (sz_tmp) {
         napi_throw_error(env, "143", "Invalid previous block size");
         return NULL;
      } else
         p=nano_block.account;

   } else {
      if (napi_get_null(env, &argv[1])!=napi_ok) {
         napi_throw_error(env, "145", "Error when parsing previous account to block");
         return NULL;
      }

      p=nano_block.account;
   }

   memcpy(nano_block.previous, p, 32);

   if (napi_get_value_string_utf8(env, argv[2], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "146", "Can't parse representative to Nano block");
      return NULL;
   }

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.representative, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write Representative Nano Wallet/Public key to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes|=REP_XRB;

   if (napi_get_value_string_utf8(env, argv[3], Balance=_buf, F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "147", "Can't parse Balance Number value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "148", "Balance digits are too long");
      return NULL;
   }

   Balance[sz_tmp]=0;

   if (napi_get_value_uint32(env, argv[4], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "149", "Can't determine Balance Big number type");
      return NULL;
   }

   if (type_tmp&BALANCE_RAW_128) {
      if ((err=f_str_to_hex(p=(uint8_t *)(_buf+2*F_RAW_STR_MAX_SZ), Balance))) {
         napi_throw_error(env, "157", "Can't parse Balance to binary");
         return NULL;
      }

      Balance=(char *)p;
   }

   type=type_tmp;

   if (napi_get_value_string_utf8(env, argv[5], Value_To_Send_Rec=(_buf+F_RAW_STR_MAX_SZ), F_RAW_STR_MAX_SZ+1, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "150", "Can't parse Value to send/Receive Big Number value");
      return NULL;
   }

   if (sz_tmp==F_RAW_STR_MAX_SZ) {
      napi_throw_error(env, "151", "Value to send/receive digits are too long");
      return NULL;
   }

   Value_To_Send_Rec[sz_tmp]=0;

   if (napi_get_value_uint32(env, argv[6], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "152", "Can't determine Value to send/receive Big number type");
      return NULL;
   }

   if (type_tmp&VALUE_SEND_RECEIVE_RAW_128) {
      if ((err=f_str_to_hex(p=(uint8_t *)(_buf+3*F_RAW_STR_MAX_SZ), Value_To_Send_Rec))) {
         napi_throw_error(env, "157", "Can't parse Value to Send/Receive to binary");
         return NULL;
      }

      Value_To_Send_Rec=(char *)p;
   }

   type|=type_tmp;
   type_tmp|=F_NANO_A_RAW_128;

   if ((err=f_nano_value_compare_value(memset(_buf+256, 0, sizeof(f_uint128_t)), Value_To_Send_Rec, &type_tmp))) {
      napi_throw_error(env, "158", "Can't compare Value to send big numbers");
      return NULL;
   }

   if (type_tmp&F_NANO_COMPARE_EQ) {
      napi_throw_error(env, "159", "You cannot send or receive 0 amount");
      return NULL;
   }

   if (napi_get_value_uint32(env, argv[8], &type_tmp)!=napi_ok) {
      napi_throw_error(env, "153", "Can't determine direction (Send or Receive)");
      return NULL;
   }

   type|=type_tmp;

   if (memcmp(nano_block.account, nano_block.previous, 32)==0) {
      if (type_tmp&VALUE_TO_SEND) {
         napi_throw_error(env, "160", "You cannot send amount in genesis block");
         return NULL;
      }

      type_tmp=((type&(BALANCE_RAW_128|BALANCE_REAL_STRING|BALANCE_RAW_STRING))|F_NANO_B_RAW_128);

      if ((err=f_nano_value_compare_value(Balance, _buf+256, &type_tmp))) {
         napi_throw_error(env, "158", "Can't compare Balance big number");
         return NULL;
      }

      if (type_tmp&F_NANO_COMPARE_GT) {
         napi_throw_error(env, "161", "Genesis block should have 0 amount");
         return NULL;
      }
   }

   if ((err=f_nano_add_sub(nano_block.balance, Balance, Value_To_Send_Rec, F_NANO_RES_RAW_128|type))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, ERROR_ADD_SUB_BIG_NUMBERS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[7], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "154", "Can't parse link to Nano block");
      return NULL;
   }

   if ((err=extract_public_key_from_wallet_or_hex_str_util(&xrb_prefix, nano_block.link, _buf, sz_tmp))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't write link to block %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (xrb_prefix)
      nano_block.prefixes|=DEST_XRB;

   nano_block.preamble[31]=0x06;

   if (napi_create_arraybuffer(env, sizeof(nano_block), (void **)&p, &res)!=napi_ok) {
      napi_throw_error(env, "155", "Can't create array buffer to store Nano Block");
      return NULL;
   }

   memcpy(p, &nano_block, sizeof(nano_block));

   if (napi_create_external_arraybuffer(env, p, sizeof(nano_block), NULL, NULL, &res)!=napi_ok) {
      napi_throw_error(env, "156", "Can't copy array buffer to store Nano Block in Javascript");
      return NULL;
   }

   return res;
}

napi_value nanojs_block_to_JSON(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv, block, res;
   size_t argc=1, sz_tmp;
   void *buffer;
   F_BLOCK_TRANSFER nano_block;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (napi_get_arraybuffer_info(env, argv, &buffer, &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "162", "Can't read Nano block");
      return NULL;
   }

   if (sz_tmp!=sizeof(F_BLOCK_TRANSFER)) {
      napi_throw_error(env, "163", "Wrong Nano block size");
      return NULL;
   }

   if (!f_nano_is_valid_block((F_BLOCK_TRANSFER *)buffer)) {
      napi_throw_error(env, "164", "Invalid Nano Block");
      return NULL;
   }

   memcpy(&nano_block, buffer, sizeof(nano_block));

   if (napi_create_object(env, &block)!=napi_ok) {
      napi_throw_error(env, "165", "Can't create JSON block");
      return NULL;
   }

   if (napi_create_string_utf8(env, "state", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "type", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&SENDER_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.account, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert public key account to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "account", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)nano_block.previous), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "previous", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&REP_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.representative, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert public key representative to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "representative", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=f_nano_balance_to_str(_buf, sizeof(_buf), &sz_tmp, nano_block.balance))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't parse Nano RAW to string %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, sz_tmp, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "balance", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(_buf, (unsigned char *)nano_block.link), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "link", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if ((err=pk_to_wallet(_buf, (nano_block.prefixes&DEST_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(_buf+128, nano_block.link, 32)))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, "Can't convert link to Base 32 Nano Wallet %s", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "link_as_account", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, (const char *)fhex2strv2(_buf, (unsigned char *)nano_block.signature, 64, 1), NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "signature", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   sprintf(_buf, "%016llx", (unsigned long long int)nano_block.work);

   if (napi_create_string_utf8(env, (const char *)_buf, NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, block, "work", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }
//
   if (napi_create_object(env, &res)!=napi_ok) {
      napi_throw_error(env, "166", "Unable to create Nano Block to parse to JavaScript");
      return NULL;
   }

   if (napi_create_string_utf8(env, "process", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, res, "action", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_create_string_utf8(env, "true", NAPI_AUTO_LENGTH, &argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_UNABLE_TO_CREATE_ATTRIBUTE);
      return NULL;
   }

   if (napi_set_named_property(env, res, "json_block", argv)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   if (napi_set_named_property(env, res, "block", block)!=napi_ok) {
      napi_throw_error(env, NULL, ERROR_CANT_ADD_ATTRIBUTE_TO_NANO_JSON_BLOCK);
      return NULL;
   }

   return res;
}

napi_value nanojs_convert_balance(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv, res;
   size_t argc=1, sz_tmp;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   return res;
}

typedef int (*attr_fn)(napi_env, napi_value, void *);
typedef napi_value (*my_nano_fn)(napi_env, napi_callback_info);
typedef struct my_nano_js_fn_call_t {
   const char *function_name;
   my_nano_fn fn;
} MY_NANO_JS_FUNCTION;

typedef struct constant_uint32_t_t {
   const char *constant_name;
   uint32_t constant;
} MY_NANO_JS_CONST_UINT32_T;

typedef struct constant_uint64_t_t {
   const char *constant_name;
   uint64_t constant;
} MY_NANO_JS_CONST_UINT64_T;

MY_NANO_JS_FUNCTION NANO_JS_FUNCTIONS[] = {

   {"nanojs_license", nanojs_license},
   {"nanojs_wallet_to_public_key", nanojs_wallet_to_public_key},
   {"nanojs_seed_to_nano_wallet", nanojs_seed_to_nano_wallet},
   {"nanojs_add_sub", nanojs_add_sub},
   {"nanojs_pow", nanojs_pow},
   {"nanojs_extract_seed_from_brainwallet", nanojs_extract_seed_from_brainwallet},
   {"nanojs_create_block", nanojs_create_block},
   {"nanojs_block_to_JSON", nanojs_block_to_JSON},
   {NULL, NULL}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_CONST[] = {

   {"VALUE_TO_SEND", VALUE_TO_SEND},
   {"VALUE_TO_RECEIVE", VALUE_TO_RECEIVE},
   {"BALANCE_RAW_128", BALANCE_RAW_128},
   {"BALANCE_REAL_STRING", BALANCE_REAL_STRING},
   {"BALANCE_RAW_STRING", BALANCE_RAW_STRING},
   {"VALUE_SEND_RECEIVE_RAW_128", VALUE_SEND_RECEIVE_RAW_128},
   {"VALUE_SEND_RECEIVE_REAL_STRING", VALUE_SEND_RECEIVE_REAL_STRING},
   {"VALUE_SEND_RECEIVE_RAW_STRING", VALUE_SEND_RECEIVE_RAW_STRING},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_BIG_NUMBER_CONST[] = {

   {"NANO_ADD_A_B", F_NANO_ADD_A_B},
   {"NANO_SUB_A_B", F_NANO_SUB_A_B},
   {"NANO_RES_RAW_128", F_NANO_RES_RAW_128},
   {"NANO_RES_RAW_STRING", F_NANO_RES_RAW_STRING},
   {"NANO_RES_REAL_STRING", F_NANO_RES_REAL_STRING},
   {"NANO_A_RAW_128", F_NANO_A_RAW_128},
   {"NANO_A_RAW_STRING", F_NANO_A_RAW_STRING},
   {"NANO_A_REAL_STRING", F_NANO_A_REAL_STRING},
   {"NANO_B_RAW_128", F_NANO_B_RAW_128},
   {"NANO_B_RAW_STRING", F_NANO_B_RAW_STRING},
   {"NANO_B_REAL_STRING", F_NANO_B_REAL_STRING},
   {"REAL_TO_RAW", REAL_TO_RAW},
   {"RAW_TO_REAL", RAW_TO_REAL},
   {"REAL_TO_HEX", REAL_TO_HEX},
   {"HEX_TO_REAL", HEX_TO_REAL},
   {"RAW_TO_HEX", RAW_TO_HEX},
   {"HEX_TO_RAW", HEX_TO_RAW},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_BRAINWALLET_CONST[] = {

   {"BRAIN_WALLET_VERY_POOR", F_BRAIN_WALLET_VERY_POOR},
   {"BRAIN_WALLET_POOR", F_BRAIN_WALLET_POOR},
   {"BRAIN_WALLET_VERY_BAD", F_BRAIN_WALLET_VERY_BAD},
   {"BRAIN_WALLET_BAD", F_BRAIN_WALLET_BAD},
   {"BRAIN_WALLET_VERY_WEAK", F_BRAIN_WALLET_VERY_WEAK},
   {"BRAIN_WALLET_WEAK", F_BRAIN_WALLET_WEAK},
   {"BRAIN_WALLET_STILL_WEAK", F_BRAIN_WALLET_STILL_WEAK},
   {"BRAIN_WALLET_MAYBE_GOOD", F_BRAIN_WALLET_MAYBE_GOOD},
   {"BRAIN_WALLET_GOOD", F_BRAIN_WALLET_GOOD},
   {"BRAIN_WALLET_VERY_GOOD", F_BRAIN_WALLET_VERY_GOOD},
   {"BRAIN_WALLET_NICE", F_BRAIN_WALLET_NICE},
   {"BRAIN_WALLET_PERFECT", F_BRAIN_WALLET_PERFECT},
   {NULL, 0}

};

MY_NANO_JS_CONST_UINT64_T NANO_CONST_UINT64[] = {

   {"DEFAULT_THRESHOLD", F_DEFAULT_THRESHOLD},
   {NULL, 0}

};

int mynanojs_add_nano_function_util(napi_env env, napi_value exports, void *handle)
{

   napi_value fn;
   MY_NANO_JS_FUNCTION *function=(MY_NANO_JS_FUNCTION *)handle;

   while (function->function_name) {

      if (napi_create_function(env, NULL, 0, function->fn, NULL, &fn)!=napi_ok)
         return 300;

      if (napi_set_named_property(env, exports, function->function_name, fn)!=napi_ok)
         return 301;

      function++;

   }

   return 0;

}

int mynanojs_add_uint32_constant_util(napi_env env, napi_value exports, void *handle) 
{

   napi_value const_value;
   MY_NANO_JS_CONST_UINT32_T *uint32_t_constant=(MY_NANO_JS_CONST_UINT32_T *)handle;

   while (uint32_t_constant->constant_name) {

      if (napi_create_uint32(env, uint32_t_constant->constant, &const_value)!=napi_ok)
         return 200;

      if (napi_set_named_property(env, exports, uint32_t_constant->constant_name, const_value)!=napi_ok)
         return 201;

      uint32_t_constant++;

   }
   return 0;
}

int mynanojs_add_uint64_constant_util(napi_env env, napi_value exports, void *handle)
{

   napi_value const_value;
   MY_NANO_JS_CONST_UINT64_T *uint64_t_constant=(MY_NANO_JS_CONST_UINT64_T *)handle;

   while (uint64_t_constant->constant_name) {

      if (napi_create_bigint_uint64(env, uint64_t_constant->constant, &const_value)!=napi_ok)
         return 400;

      if (napi_set_named_property(env, exports, uint64_t_constant->constant_name, const_value)!=napi_ok)
         return 401;

      uint64_t_constant++;

   }
   return 0;

}

int mynanojs_add_init_property(const char *property_name, napi_env env, napi_value exports, attr_fn fn, void *handle_parameter)
{
   int err;
   napi_value property;

   if (napi_create_object(env, &property)!=napi_ok)
      return 10;

   if ((err=fn(env, property, handle_parameter)))
      return err;

   if (napi_set_named_property(env, exports, property_name, property)!=napi_ok)
      return 11;

   return 0;
}

#define NANOJS_NAPI_INIT_ERROR "myNanoJS internal error in C function \"%s\" %s"

napi_value Init(napi_env env, napi_value exports)
{
   int err;

   if ((err=mynanojs_add_nano_function_util(env, exports, NANO_JS_FUNCTIONS))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_nano_function_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_uint32_constant_util(env, exports, NANO_UINT32_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_uint32_constant_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_uint64_constant_util(env, exports, NANO_CONST_UINT64))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_uint64_constant_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("NANO_BIG_NUMBER_TYPE", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_BIG_NUMBER_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ NANO_BIG_NUMBER_TYPE", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   if ((err=mynanojs_add_init_property("BRAINWALLET_TYPE", env, exports, mynanojs_add_uint32_constant_util, (void *)NANO_UINT32_BRAINWALLET_CONST))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_init_property @ BRAINWALLET_TYPE", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)


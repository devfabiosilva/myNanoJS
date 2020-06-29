#define F_IA64
#define NAPI_VERSION 3
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
//27 de junho de 2019 15:43

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
      napi_throw_error(env, _buf, "Error when ADD/SUB Nano big numbers");
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
   char *hash;
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
         napi_throw_error(env, "125", "Precision lossing threshold in big int");
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

   if (napi_create_string_utf8(env, (const char *)warning_msg, strlen(warning_msg), &argv[1])!=napi_ok) {
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
   {NULL, NULL}

};

MY_NANO_JS_CONST_UINT32_T NANO_UINT32_CONST[] = {

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

int add_nano_function_util(napi_env env, napi_value exports, MY_NANO_JS_FUNCTION *function)
{

   napi_value fn;

   while (function->function_name) {

      if (napi_create_function(env, NULL, 0, function->fn, NULL, &fn)!=napi_ok) {
         sprintf(_buf, "Unable to wrap native function \"%s\"", function->function_name);
         napi_throw_error(env, "300", _buf);
         return 300;
      }

      if (napi_set_named_property(env, exports, function->function_name, fn)!=napi_ok) {
         sprintf(_buf, "Unable to populate constant \"%s\"", function->function_name);
         napi_throw_error(env, "301", (const char *)_buf);
         return 301;
      }

      function++;

   }

   return 0;

}

int add_uint32_constant_util(napi_env env, napi_value exports, MY_NANO_JS_CONST_UINT32_T *uint32_t_constant) 
{

   napi_value const_value;

   while (uint32_t_constant->constant_name) {

      if (napi_create_uint32(env, uint32_t_constant->constant, &const_value)!=napi_ok) {
         napi_throw_error(env, ERROR_CONST_NUM, ERROR_PARSE_CONST);
         return 200;
      }

      if (napi_set_named_property(env, exports, uint32_t_constant->constant_name, const_value)!=napi_ok) {
         sprintf(_buf, "Unable to populate constant \"%s\"", uint32_t_constant->constant_name);
         napi_throw_error(env, "201", (const char *)_buf);
         return 201;
      }

      uint32_t_constant++;

   }
   return 0;
}

int add_uint64_constant_util(napi_env env, napi_value exports, MY_NANO_JS_CONST_UINT64_T *uint64_t_constant)
{

   napi_value const_value;

   while (uint64_t_constant->constant_name) {

      if (napi_create_bigint_uint64(env, uint64_t_constant->constant, &const_value)!=napi_ok) {
         napi_throw_error(env, ERROR_CONST_NUM, ERROR_PARSE_CONST);
         return 400;
      }

      if (napi_set_named_property(env, exports, uint64_t_constant->constant_name, const_value)!=napi_ok) {
         sprintf(_buf, "Unable to populate uint64 constant \"%s\"", uint64_t_constant->constant_name);
         napi_throw_error(env, "401", (const char *)_buf);
         return 401;
      }

      uint64_t_constant++;

   }
   return 0;

}

napi_value Init(napi_env env, napi_value exports)
{
   napi_value fn;

   if (add_nano_function_util(env, exports, NANO_JS_FUNCTIONS))
      return NULL;

   if (add_uint32_constant_util(env, exports, NANO_UINT32_CONST))
      return NULL;

   if (add_uint64_constant_util(env, exports, NANO_CONST_UINT64))
      return NULL;

   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)


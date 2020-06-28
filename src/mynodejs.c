#define F_IA64
#define NAPI_VERSION 3
#include <node_api.h>
#include "f_nano_crypto_util.h"

#define CANT_PARSE_JAVASCRIPT_ARGS "Can't parse arguments"
#define ERROR_PARSE_CONST "Unable to INIT constant"
#define ERROR_CONST_NUM "200"
#define PARSE_ERROR "101"
//27 de junho de 2019 15:43

#define F_BUF_CHAR (size_t)512
static char _buf[F_BUF_CHAR];

void memory_flush() {
   memset(_buf, 0, sizeof(_buf));
}

napi_value nanojs_license(napi_env env, napi_callback_info info)
{
   napi_value license;

   if (napi_create_string_utf8(env, (const char *)LICENSE, sizeof(LICENSE)-1, &license)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, "Fail on parse LICENSE");
      return NULL;
   }

   return license;

}

napi_value nanojs_wallet_to_public_key(napi_env env, napi_callback_info info) 
{
   int err;
   size_t argc=1;
   napi_value argv, res;
   char nano_wallet[MAX_STR_NANO_CHAR];
   size_t nano_wallet_sz;

   if (napi_get_cb_info(env, info, &argc, &argv, NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv, nano_wallet, sizeof(nano_wallet), &nano_wallet_sz)!=napi_ok) {
      napi_throw_error(env, "102", "Can't parse public key to myNanoEmbedded C library");
      return NULL;
   }

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

   return res;

nanojs_seed_to_nano_wallet_EXIT1:
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

MY_NANO_JS_FUNCTION NANO_JS_FUNCTIONS[] = {

   {"nanojs_license", nanojs_license},
   {"nanojs_wallet_to_public_key", nanojs_wallet_to_public_key},
   {"nanojs_seed_to_nano_wallet", nanojs_seed_to_nano_wallet},
   {NULL, NULL}

};

MY_NANO_JS_CONST_UINT32_T NANO_ADD_SUB_CONST[] = {

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
   {NULL, 0}

};

int add_nano_function_util(napi_env env, napi_value exports, MY_NANO_JS_FUNCTION *function)
{

   napi_value fn;

   while (function->function_name) {

      if (napi_create_function(env, NULL, 0, function->fn, NULL, &fn)!=napi_ok) {
         napi_throw_error(env, "300", "Unable to wrap native function nanojs_license");
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

napi_value Init(napi_env env, napi_value exports)
{
   napi_value fn;

   if (add_nano_function_util(env, exports, NANO_JS_FUNCTIONS))
      return NULL;

   if (add_uint32_constant_util(env, exports, NANO_ADD_SUB_CONST))
      return NULL;

   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)


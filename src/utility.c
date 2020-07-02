#include "utility.h"

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


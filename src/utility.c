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

char *verify_password_util(char *text, int err)
{
   strcpy(text, "< ");
   err*=-1;

   if (err&F_PASS_IS_OUT_OVF) {
      strcat(text, "Password is overflow ");
      goto verify_password_util_EXIT1;
   }

   if (err&F_PASS_IS_TOO_SHORT) {
      strcat(text, "Password is too short ");
      goto verify_password_util_EXIT1;
   }

   if (err&F_PASS_IS_TOO_LONG) {
      strcat(text, "Password is very long ");
      goto verify_password_util_EXIT1;
   }

   if (err&F_PASS_MUST_HAVE_AT_LEAST_ONE_UPPER_CASE)
      strcat(text, "|Password must have at least one upper case ");

   if (err&F_PASS_MUST_HAVE_AT_LEAST_ONE_LOWER_CASE)
      strcat(text, "|Password must have one lower case ");

   if (err&F_PASS_MUST_HAVE_AT_LEAST_ONE_SYMBOL)
      strcat(text, "|Password must have at least one symbol ");

   if (err&F_PASS_MUST_HAVE_AT_LEAST_ONE_NUMBER)
      strcat(text, "|Password must have at least one number ");

verify_password_util_EXIT1:
   return strcat(text, ">");
}

int seed2keypair_util(char *buf, char **p, char **q, uint32_t wallet_number, const char *prefix, int is_hexstring)
{
   int err;
// if err -> p = error, q = reason
// if success -> buf = wallet, p = private key(hexstr), q = public key(hexstr)
// if private key is hex string then you need verify if size is 128 in length
   *p=buf+384;
   *q=buf+256;

   if (is_hexstring) {
      if ((err=f_str_to_hex((uint8_t *)*p, buf))) {
         sprintf(*p, "%d", err);
         sprintf(*q, "Can't parse hexstring to binary to extract keypair %s", *p);
         return err;
      }

      memcpy(buf, *p, 32);
   }

   if ((err=f_seed_to_nano_wallet((uint8_t *)*p, (uint8_t *)*q, (uint8_t *)buf, wallet_number))) {
      sprintf(*p, "%d", err);
      sprintf(*q, "Can't extract Nano SEED to KeyPair %s", *p);
      return err;
   }

   if ((err=pk_to_wallet(buf, (char *)prefix, memcpy(buf+128, *q, 32)))) {
      sprintf(*p, "%d", err);
      sprintf(*q, "Can't extract wallet from public key %s", *p);
      return err;
   }

   *p=f_nano_key_to_str(*p-96, (unsigned char *)*p);
   *q=f_nano_key_to_str(*q-96, (unsigned char *)*q);

   return 0;

}

int create_object_keypair_util(napi_env env, napi_value res, char *private_key, char *public_key, uint32_t wallet_number, char *wallet)
{
   napi_value tmp;

   if (napi_create_string_utf8(env, private_key, 64, &tmp)!=napi_ok)
      return 31;

   if (napi_set_named_property(env, res, "private_key", tmp)!=napi_ok)
      return 32;

   if (napi_create_string_utf8(env, public_key, 64, &tmp)!=napi_ok)
      return 33;

   if (napi_set_named_property(env, res, "public_key", tmp)!=napi_ok)
      return 34;

   if (napi_create_uint32(env, wallet_number, &tmp)!=napi_ok)
      return 35;

   if (napi_set_named_property(env, res, "wallet_number", tmp)!=napi_ok)
      return 36;

   if (napi_create_string_utf8(env, wallet, NAPI_AUTO_LENGTH, &tmp)!=napi_ok)
      return 37;

   if (napi_set_named_property(env, res, "wallet", tmp)!=napi_ok)
      return 38;

   return 0;
}

#define HEX_STRING_BIG_NUMBER (size_t)(2*sizeof(f_uint128_t))
int parse_and_adjust_big_number_hex128_string_balance_util(uint8_t *dest, char *balance, size_t balance_sz)
{
   size_t tmp;
   char hex_str_128[HEX_STRING_BIG_NUMBER+1];

   if (!balance_sz)
      return 61;

   if (balance_sz>HEX_STRING_BIG_NUMBER)
      return 62;

   hex_str_128[HEX_STRING_BIG_NUMBER]=0;

   if ((tmp=(HEX_STRING_BIG_NUMBER-balance_sz)))
      memset(hex_str_128, '0', tmp);

   memcpy(hex_str_128+tmp, balance, balance_sz);

   return f_str_to_hex(dest, hex_str_128);
}

int filter_no_entropy_util(uint32_t entropy)
{
   switch (entropy) {
      case F_ENTROPY_TYPE_PARANOIC:
      case F_ENTROPY_TYPE_EXCELENT:
      case F_ENTROPY_TYPE_GOOD:
      case F_ENTROPY_TYPE_NOT_ENOUGH:
      case F_ENTROPY_TYPE_NOT_RECOMENDED:
      return 0;
   }

   return 1;
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

int p2pow_block_json_util(napi_env env, napi_value res, char *buf, size_t buf_sz, const char *attr_parameter, F_BLOCK_TRANSFER *block)
{
   int err;
   napi_value tmp1, tmp2;
   size_t sz_tmp;

   if (napi_create_object(env, &tmp1)!=napi_ok)
      return 60;

   if (napi_create_string_utf8(env, NANO_STATE, NAPI_AUTO_LENGTH, &tmp2)!=napi_ok)
      return 61;

   if (napi_set_named_property(env, tmp1, "block_type", tmp2)!=napi_ok)
      return 62;

   if ((err=pk_to_wallet(buf, (block->prefixes&SENDER_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(buf+128, block->account, 32))))
      return err;

   if (napi_create_string_utf8(env, (const char *)buf, NAPI_AUTO_LENGTH, &tmp2)!=napi_ok)
      return 63;

   if (napi_set_named_property(env, tmp1, NANO_ACCOUNT, tmp2)!=napi_ok)
      return 64;

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(buf, (unsigned char *)block->previous), 64, &tmp2)!=napi_ok)
      return 65;

   if (napi_set_named_property(env, tmp1, NANO_PREVIOUS, tmp2)!=napi_ok)
      return 66;

   if ((err=pk_to_wallet(buf, (block->prefixes&REP_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(buf+128, block->representative, 32))))
      return 67;

   if (napi_create_string_utf8(env, (const char *)buf, NAPI_AUTO_LENGTH, &tmp2)!=napi_ok)
      return 68;

   if (napi_set_named_property(env, tmp1, NANO_REPRESENTATIVE, tmp2)!=napi_ok)
      return 69;

   if ((err=f_nano_balance_to_str(buf, buf_sz, &sz_tmp, block->balance)))
      return 70;

   if (napi_create_string_utf8(env, (const char *)buf, sz_tmp, &tmp2)!=napi_ok)
      return 71;

   if (napi_set_named_property(env, tmp1, NANO_BALANCE, tmp2)!=napi_ok)
      return 72;

   if (napi_create_string_utf8(env, (const char *)f_nano_key_to_str(buf, (unsigned char *)block->link), 64, &tmp2)!=napi_ok)
      return 73;

   if (napi_set_named_property(env, tmp1, NANO_LINK, tmp2)!=napi_ok)
      return 74;

   if ((err=pk_to_wallet(buf, (block->prefixes&DEST_XRB)?XRB_PREFIX:NANO_PREFIX, memcpy(buf+128, block->link, 32))))
      return 75;

   if (napi_create_string_utf8(env, (const char *)buf, NAPI_AUTO_LENGTH, &tmp2)!=napi_ok)
      return 76;

   if (napi_set_named_property(env, tmp1, NANO_LINK_AS_ACCOUNT, tmp2)!=napi_ok)
      return 77;

   if (napi_create_string_utf8(env, (const char *)fhex2strv2(buf, (unsigned char *)block->signature, 64, 1), 128, &tmp2)!=napi_ok)
      return 78;

   if (napi_set_named_property(env, tmp1, NANO_SIGNATURE, tmp2)!=napi_ok)
      return 79;

   if (napi_set_named_property(env, res, attr_parameter, tmp1)!=napi_ok)
      return 80;

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

int mynanojs_add_char_constant_util(napi_env env, napi_value exports, void *handle)
{

   napi_value const_value;
   MY_NANO_JS_CONST_CHAR *char_constant=(MY_NANO_JS_CONST_CHAR *)handle;

   while (char_constant->constant_name) {

      if (napi_create_string_utf8(env, char_constant->constant, NAPI_AUTO_LENGTH, &const_value)!=napi_ok)
         return 402;

      if (napi_set_named_property(env, exports, char_constant->constant_name, const_value)!=napi_ok)
         return 403;

      char_constant++;

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


// 2020-08-08 18:34:41 
// Author: Fábio Pereira da Silva
// License: MIT

const NANOJS = require('../utils/mynanojs');
const SERVICES = require('../utils/service');
const axios = require('axios');

const nano_api = axios.create({
   baseURL: SERVICES.NANO_NODE_SRV,
   headers:  {
                 'Content-Type': 'application/json'
             },
   validateStatus: function () {
      return true;
   }
});

const p2pow_api = axios.create({
   baseURL: `${SERVICES.P2POW_SRV}:7090`,
   headers:  {
                 'Content-Type': 'application/json'
             },
   validateStatus: function () {
      return true;
   }
});

async function nano_rpc_account_representative(account) {
   let data = null, err;

   await nano_api.post('/', {
       action: "account_representative",
       account
   }).then(
       (d) => data = d.data,
       (e) => err = e.data
   );

   return new Promise((resolve, reject) => (data)?(data.error)?reject({error: data.error}):resolve(data):reject(err));
}
// Block example. Don't use this SEED and private keys to send/receive transaction
// When testing this example. Create a new SEED using nanojs_generate_seed function.
// This is a Proof of Concept. Do any transaction with small amount
//console.log(NANOJS.nanojs_generate_seed(NANOJS.ENTROPY_TYPE.ENTROPY_TYPE_EXCELENT));
const SEED = '3B9B326FBFFF8F155846475551171D39ED6C2476773727D9BF5566348414C601';
const KEY_PAIR = NANOJS.nanojs_seed_to_keypair(SEED, 0);
const EXTENDED_PRIVATE_KEY = `${KEY_PAIR.private_key}${KEY_PAIR.public_key}`;
const ACCOUNT = KEY_PAIR.wallet;
const PREVIOUS = 'DCF264304734069EAEBD36DE6EE490F9E436B0DC0C14864E9FFE3FA9B045EF8F';
const BALANCE = '0.0499';
const VALUE_TO_SEND_OR_RECEIVE = '0.0498';
const REPRESENTATIVE = "nano_3ngt59dc7hbsjd1dum1bw9wbb87mbtuj4qkwcruididsb5rhgdt9zb4w7kb9";
const DESTINATION_WALLET_OR_LINK = "nano_1fxuu796gedchbroruccmehk6fdxbnz5y5eunhytdqrne1zg5qfcfhphiyey";
//const DESTINATION_WALLET_OR_LINK = '490A9A5C2B933157582B255F0158FF40FBB165264B8297A66EBC674CDA89530C';
const MAX_FEE = "0.0001";

// Result of this transaction in Nano Blockchain:
// https://nanocrawler.cc/explorer/account/nano_36iwu6i3zp9ges1km1fm6i5gxp1kssi6acboe4g3jhefmb6qendbae7kt1jd/history

async function getP2POWInfo() {
   let data, error = null;

   await p2pow_api.get('/request_info').then(
      (res) => data = res.data,
      (err) => error = {error: err.code}
   );

   return new Promise((res, err) => {
      return (error)?err(error):res(data);
   });
}

async function requestPow(signedP2PoWJSON) {
   let data;

   data = await p2pow_api.post('/request_work', signedP2PoWJSON);

   return new Promise((res, err) => {
      if (data.data.error)
         return err(data.data);

      return res(data.data);
   });
}

function verifyFee(fee) {
   // If worker fee is greater than MAX_FEE then return true
   // result fee > MAX_FEE
   let result;

   try {
      result = NANOJS.nanojs_compare(
         fee,
         MAX_FEE,
         NANOJS.NANO_BIG_NUMBER_TYPE.NANO_A_RAW_STRING + NANOJS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING,
         NANOJS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_GT
      );
   } catch (e) {
      console.log(`Error in Big Number nanojs_compare "${e.message}"`);
      return null
   }

   return result;
}

async function main() {
   let p2pow_info,
       conditional,
       worker_representative,
       nano_block,
       p2pow_block,
       signed_p2pow_block,
       signed_p2pow_JSON;

   console.log("Getting P2PoW info ...");
   p2pow_info = await getP2POWInfo().catch((reject) => p2pow_info = reject);

   if (p2pow_info.error) {
      console.log(`Error get worker info: ${p2pow_info.error}`);
      return;
   }

   console.log("Verifying P2PoW fee ...");
   conditional = verifyFee(p2pow_info.fee);
   
   if (conditional === null)
      return;

   if (conditional) {
      console.log(`Worker FEE is greater than ${MAX_FEE}`);
      return;
   }

   console.log(`Fee allowed. Max allowed fee is ${MAX_FEE}`);
   console.log("Getting worker representative")
   worker_representative = await nano_rpc_account_representative(p2pow_info.reward_account).catch((err) => worker_representative = err);

   if (worker_representative.error) {
      console.log(`Error in nano_rpc_account_representative" ${worker_representative.error}`);
      return;
   }

   console.log(`Worker representative is ${worker_representative.representative}`);
   console.log("Creating Nano block ...");

   try {
      nano_block = NANOJS.nanojs_create_block(
         ACCOUNT,
         PREVIOUS,
         REPRESENTATIVE,
         BALANCE,
         NANOJS.BALANCE_REAL_STRING,
         VALUE_TO_SEND_OR_RECEIVE,
         NANOJS.VALUE_SEND_RECEIVE_REAL_STRING,
         DESTINATION_WALLET_OR_LINK,
         //NANOJS.VALUE_TO_RECEIVE
         NANOJS.VALUE_TO_SEND
      );
   } catch (e) {
      console.log(`Error when creating block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("Success !!!");
   console.log(Buffer.from(nano_block).toString('hex'));
   console.log("Creating P2PoW block ...");

   try {
      p2pow_block = NANOJS.nanojs_block_to_p2pow(
         nano_block,
         p2pow_info.reward_account,
         worker_representative.representative,
         p2pow_info.fee,
         NANOJS.NANO_BIG_NUMBER_TYPE.WORKER_FEE_RAW
      );
   } catch (e) {
      console.log(`Error when creating P2PoW block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("P2PoW created successfully");
   console.log(Buffer.from(p2pow_block).toString('hex'));
   console.log("Now sign P2PoW block ...");

   try {
      signed_p2pow_block = NANOJS.nanojs_sign_p2pow_block(p2pow_block, EXTENDED_PRIVATE_KEY);
   } catch (e) {
      console.log(`Error when signing P2PoW block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("P2PoW signed successfully");
   console.log(Buffer.from(signed_p2pow_block).toString('hex'));
   console.log("Now parsing P2PoW block to JSON (signed)...");

   try {
      signed_p2pow_JSON = NANOJS.nanojs_p2pow_block_to_JSON(signed_p2pow_block);
   } catch (e) {
      console.log(`Error when parsing P2PoW block to JSON: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("Success");
   console.log(signed_p2pow_JSON);

   console.log("Sending signed P2PoW signed block JSON to worker ...");
   requestPow(signed_p2pow_JSON).then(
      (res) => console.log(res),
      (err) => console.log(err)
   );
}

main();


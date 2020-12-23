// 2020-08-08 18:34:41 
// Author: Fábio Pereira da Silva
// License: MIT
// myNanoJS: JavaScript C bindings for Nano cryptocurrency/IoT/P2PoW support

/*
 * Peer 2 PoW Proof of Concept
 * This example shows how to send/receive Nano using myNanoJS with P2PoW API
 * References:
 *    1- https://medium.com/@kaiquenunes/delegated-proof-of-work-d566870924d9
 *    2- https://github.com/anarkrypto/delegated-PoW-API
 *
 *
 * CAUTION:
 *  1- Don't use any private keys or SEED in this example, create a new one SEED to send and receive NANOS;
 *  2- Don't use your SEED in this example
 *  3- Send only small amount of Nanos for testing. You can send very little amount to test such as 0.0003 Nanos for example
 *
 *   POC
 *   ===
 *
 *      1- Request P2PoW information
 *      2- Checks if fee is <= MAX_FEE
 *      3- Create Nano Block (receive or send Nanos)
 *      4- Create P2PoW block
 *      5- Sign P2PoW block
 *      6- Parse P2PoW block to JSON
 *      7- Request work and store transaction to Nano Blockchain
 *
 *
 *
 *   Installing
 *   ==========
 *
 *   git clone https://github.com/devfabiosilva/myNanoJS
 *   cd examples
 *   npm install
 *   cd 01-p2pow
 *   node index.js
 */

const NANOJS = require('../utils/mynanojs');
const axios = require('axios');

const P2POW_SRV = "http://157.245.80.20";//"YOUR_P2POW_SERVER_HERE";
const P2POW_PORT = "7090";

const p2pow_api = axios.create({
   baseURL: `${P2POW_SRV}:${P2POW_PORT}`,
   headers:  {
                 'Content-Type': 'application/json'
             },
   validateStatus: function () {
      return true;
   }
});

// Block example. Don't use this SEED and private keys in this example to send/receive transaction
// When testing this example create a new SEED using nanojs_generate_seed function.
// This is a Proof of Concept. Do any transaction with small amount

// SEED. Create one SEED using NANOJS.nanojs_generate_seed(NANOJS.ENTROPY_TYPE.ENTROPY_TYPE_EXCELENT)
// NANOJS.nanojs_generate_seed(NANOJS.ENTROPY_TYPE.ENTROPY_TYPE_EXCELENT);
//
// '3B9B326FBFFF8F155846475551171D39ED6C2476773727D9BF5566348414C601'
//
// Warning. Don't use this seed below. Generate new one
const SEED = '3B9B326FBFFF8F155846475551171D39ED6C2476773727D9BF5566348414C601'; // DON'T use this SEED

// Key Pair. Extracting Key Pair 0 from SEED above
// const KEY_PAIR = NANOJS.nanojs_seed_to_keypair(SEED, 0);
//
// {
//  private_key: '04192BB6C5424E368FCA680D885E38A67095E324F246DF6281D9B7362058709E',
//  public_key: '921CD9201FD8EE66412981B32406EED812CE60442935609C18BD8D9A49765169',
//  wallet_number: 0,
//  wallet: 'nano_36iwu6i3zp9ges1km1fm6i5gxp1kssi6acboe4g3jhefmb6qendbae7kt1jd'
// }
const KEY_PAIR = NANOJS.nanojs_seed_to_keypair(SEED, 0);

// Extended Private Key for sign P2PoW block (Don't use this private key)
// EXTENDED_PRIVATE_KEY = '04192BB6C5424E368FCA680D885E38A67095E324F246DF6281D9B7362058709E921CD9201FD8EE66412981B32406EED812CE60442935609C18BD8D9A49765169'
const EXTENDED_PRIVATE_KEY = `${KEY_PAIR.private_key}${KEY_PAIR.public_key}`;

// Account:
const ACCOUNT = KEY_PAIR.wallet;

// Previous block. Empty string means first transaction
const PREVIOUS = '0D69AC75AC25B977C4525C6BBD3270CDF4F97A91570B82217366501C0E008010';

const BALANCE = '0'; // Current balance in this account in real string
const VALUE_TO_SEND_OR_RECEIVE = '0.9999'; // Value to receive (open block) or value do send in real string
const REPRESENTATIVE = "nano_3ngt59dc7hbsjd1dum1bw9wbb87mbtuj4qkwcruididsb5rhgdt9zb4w7kb9"; // Account representative

// Destination wallet or link: Your destination wallet to send Nano or link to receive Nano
//const DESTINATION_WALLET_OR_LINK = "nano_1fxuu796gedchbroruccmehk6fdxbnz5y5eunhytdqrne1zg5qfcfhphiyey";
const DESTINATION_WALLET_OR_LINK = 'D3E2955A1FE390057F745177FAD584A3A3350F20363338050A1391A71CBF5606';
const MAX_FEE = "0.0001"; // Maximun allowed fee

// Result of this transaction in Nano Blockchain (nano_36iwu6i3zp9ges1km1fm6i5gxp1kssi6acboe4g3jhefmb6qendbae7kt1jd):
// https://nanocrawler.cc/explorer/account/nano_36iwu6i3zp9ges1km1fm6i5gxp1kssi6acboe4g3jhefmb6qendbae7kt1jd/history


/*
 * Get P2PoW worker information (worker wallet and worker fee)
 */
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

/*
 * Request work information (worker fee, worker wallet)
 */
async function requestPow(signedP2PoWJSON) {
   let data;

   data = await p2pow_api.post('/request_work', signedP2PoWJSON);

   return new Promise((res, err) => {
      if (data.data.error)
         return err(data.data);

      return res(data.data);
   });
}


/*
 *  Checks if fee is less or equal MAX_FEE
 */
function verifyFee(fee) {
   // Boolean: If worker fee is greater than MAX_FEE then return true
   // result fee > MAX_FEE
   let result;

   try {
      //nanojs_compare(bigNumberA, bigNumberB, bigNumbersType, condition)
      result = NANOJS.nanojs_compare(
         fee, // fee in big number raw string
         MAX_FEE, // MAX_FEE in big number real string
         NANOJS.NANO_BIG_NUMBER_TYPE.NANO_A_RAW_STRING + NANOJS.NANO_BIG_NUMBER_TYPE.NANO_B_REAL_STRING, // Big numbers types
         NANOJS.NANO_BIG_NUMBER_CONDITIONAL.NANO_COMPARE_GT // Condition = fee > MAX_FEE
      );
   } catch (e) {
      console.log(`Error in Big Number nanojs_compare "${e.message}"`);
      return null
   }

   return result;
}

async function main() {
   let p2pow_info,         // Information about worker
       conditional,        // fee conditional
       nano_block,         // Nano block  (ArrayBuffer)
       p2pow_block,        // P2PoW block (ArrayBuffer)
       signed_p2pow_block, // Signed P2PoW block (ArrayBuffer)
       signed_p2pow_JSON;  // Signed P2PoW block (JSON)

   // 1- Getting P2PoW info ...
   console.log("Getting P2PoW info ...");
   p2pow_info = await getP2POWInfo().catch((reject) => p2pow_info = reject);

   if (p2pow_info.error) {
      console.log(`Error get worker info: ${p2pow_info.error}`);
      return;
   }

   // 2- Check if worker fee has an allowed FEE (fee <= MAX_FEE)
   console.log("Verifying P2PoW fee ...");
   conditional = verifyFee(p2pow_info.fee);
   
   if (conditional === null)
      return;

   if (conditional) {
      console.log(`Worker FEE is greater than ${MAX_FEE}`);
      return;
   }

   try {
      console.log(`Worker fee is ${NANOJS.nanojs_convert_balance(p2pow_info.fee)}`);
   } catch (e) {
      console.log(`Error when converting RAW->REAL: ${(e.code)?(e.code):-1} ${e.message}`);
   }
   console.log(`Fee allowed. Max allowed fee is ${MAX_FEE}`);
   console.log("Creating Nano block ...");

   // 3- Creating Nano block (account, frontier, balance, value to send/receive, destination or link)
   try {
      nano_block = NANOJS.nanojs_create_block(
         ACCOUNT, //Your Nano account
         PREVIOUS, // Previous block. Empty string = genesis block
         REPRESENTATIVE, // Account representative
         BALANCE,// Current balance
         NANOJS.BALANCE_REAL_STRING, // Balance type: Real string
         VALUE_TO_SEND_OR_RECEIVE,   // Value to send/receive Nanos
         NANOJS.VALUE_SEND_RECEIVE_REAL_STRING, // Value to send/receive type: Real string
         DESTINATION_WALLET_OR_LINK, // Destination wallet (for sending amount) or link (receive amount/open block)
         NANOJS.VALUE_TO_RECEIVE // Direction: (NANOJS.VALUE_TO_RECEIVE = open block) or (NANOJS.VALUE_TO_SEND = value to send to)
         //NANOJS.VALUE_TO_SEND
      );
   } catch (e) {
      console.log(`Error when creating block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("Success !!!");
   console.log(Buffer.from(nano_block).toString('hex'));
   console.log("Creating P2PoW block ...");

   // 4- Creating P2PoW block (Adding Nano block and reward account + worker fee)
   try {
      p2pow_block = NANOJS.nanojs_block_to_p2pow(
         nano_block, // Nano block
         p2pow_info.reward_account, // reward account wallet
         null,//worker_representative.representative. If null, worker representative is the same account representarive
         p2pow_info.fee, // fee in raw
         NANOJS.NANO_BIG_NUMBER_TYPE.WORKER_FEE_RAW // Big number fee type: Raw string
      );
   } catch (e) {
      console.log(`Error when creating P2PoW block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("P2PoW created successfully");
   console.log(Buffer.from(p2pow_block).toString('hex'));
   console.log("Now sign P2PoW block ...");

   // 5- Signing P2PoW block with associated private key account
   try {
      signed_p2pow_block = NANOJS.nanojs_sign_p2pow_block(p2pow_block, EXTENDED_PRIVATE_KEY);
   } catch (e) {
      console.log(`Error when signing P2PoW block: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("P2PoW signed successfully");
   console.log(Buffer.from(signed_p2pow_block).toString('hex'));
   console.log("Now parsing P2PoW block to JSON (signed)...");

   // 6- Parsing signed P2PoW block to JSON
   try {
      signed_p2pow_JSON = NANOJS.nanojs_p2pow_block_to_JSON(signed_p2pow_block);
   } catch (e) {
      console.log(`Error when parsing P2PoW block to JSON: ${(e.code)?(e.code):-1} ${e.message}`);
      return;
   }

   console.log("Success");
   console.log(signed_p2pow_JSON);

   console.log("Sending signed P2PoW signed block JSON to worker ...");

   // 7- Requesting work to store in Nano blockchain
   requestPow(signed_p2pow_JSON).then(
      (res) => console.log(res),
      (err) => console.log(err)
   );
}

main();


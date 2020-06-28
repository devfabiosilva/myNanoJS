const MY_NANO_JS = require('./build/Release/mynanojs');

console.log(MY_NANO_JS);
console.log(MY_NANO_JS.nanojs_wallet_to_public_key('xrb_3tta9pdxr4djdcm6r3c7969syoirj3dunrtynmmi8n1qtxzk9iksoz1gxdrh'));
console.log(MY_NANO_JS.nanojs_seed_to_nano_wallet('86A4EBE059446FF84F8C4FAC7C56EF97D16DA7369845051414A0A6DA5EB99FCA', 1728));
console.log(MY_NANO_JS.nanojs_add_sub("521.1", "11", MY_NANO_JS.NANO_SUB_A_B+MY_NANO_JS.NANO_A_REAL_STRING+MY_NANO_JS.NANO_B_REAL_STRING));
console.log(MY_NANO_JS.nanojs_pow('de0c84215a6b7429d3d2836f54b6b917c9301103134904457a928c56580cf5a4', 3, 0xffffn));
//5213761273712736712637162361278631273178362236163251536136516

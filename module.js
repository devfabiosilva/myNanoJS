const MY_NANO_JS = require('./build/Release/mynanojs');

console.log(MY_NANO_JS);
console.log(MY_NANO_JS.nanojs_seed_to_nano_wallet('86A4EBE059446FF84F8C4FAC7C56EF97D16DA7369845051414A0A6DA5EB99FCA', 1728));
console.log(MY_NANO_JS.nanojs_add_sub("5", "2.3"));


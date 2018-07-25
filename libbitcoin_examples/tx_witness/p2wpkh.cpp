#include <bitcoin/bitcoin.hpp>
#include <iostream>

using namespace bc;
using namespace wallet;
using namespace chain;
using namespace machine;

// Testnet wallets.
auto my_secret0 = base16_literal(
    "b7423c94ab99d3295c1af7e7bbea47c75d298f7190ca2077b53bae61299b70a5");
ec_private my_private0(my_secret0, ec_private::testnet, true);
ec_compressed pubkey0 = my_private0.to_public().point();
payment_address my_address0 = my_private0.to_payment_address();

auto my_secret1 = base16_literal(
    "d977e2ce0f744dc3432cde9813a99360a3f79f7c8035ef82310d54c57332b2cc");
ec_private my_private1(my_secret1, ec_private::testnet, true);
ec_compressed pubkey1 = my_private1.to_public().point();

// Witness aware testnet wallets.
// ("Witness aware" is an arbitrary assignment for illustration purposes)
auto my_secret_witness_aware = base16_literal(
    "0a44957babaa5fd46c0d921b236c50b1369519c7032df7906a18a31bb905cfdf");
ec_private my_private_witness_aware(my_secret_witness_aware,
    ec_private::testnet, true);
ec_compressed pubkey_witness_aware = my_private_witness_aware
    .to_public().point();

auto my_secret_witness_aware1 = base16_literal(
    "2361b894dab5c45c3c448eb4ab65f847cb3000e05969f18c94b8850233a95b74");
ec_private my_private_witness_aware1(my_secret_witness_aware1,
    ec_private::testnet, true);
ec_compressed pubkey_witness_aware1 = my_private_witness_aware1
    .to_public().point();

auto my_secret_witness_aware2 = base16_literal(
    "87493c67155f44a9a9a6abf621926a407121d6f4e1e94c75ced61208d7abe9db");
ec_private my_private_witness_aware2(my_secret_witness_aware2,
    ec_private::testnet, true);
ec_compressed pubkey_witness_aware2 = my_private_witness_aware2
    .to_public().point();


void example_to_p2wpkh() {

  //**************************************************************
  // SEND TO P2WPKH.
  //**************************************************************

  // Output Script.
  // 	  0 [20-byte hash160(public key)]
  // Input Script.
  //    According to previous output.
  // Script Code.
  // 	  According to previous output.

  // P2PKH Input.
  // Previous TX hash.
  std::string prev_tx =
      "44101b50393d01de1e113b17eb07e8a09fbf6334e2012575bc97da227958a7a5";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO index.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input0 object.
  input input0;
  input0.set_previous_output(uxto_to_spend);
  input0.set_sequence(max_input_sequence);
  // Previous embedded script.
  script prev_p2pkh_output_script = script::to_pay_key_hash_pattern(
      bitcoin_short_hash(pubkey0));

  // P2WPKH output script.
  // 0 [20-byte hash160(public key)]
  operation::list p2wpkh_operations;
  p2wpkh_operations.push_back(operation(opcode::push_size_0));
  p2wpkh_operations.push_back(
      operation(to_chunk(bitcoin_short_hash(pubkey_witness_aware))));
  // Build P2WPKH output.
  std::string btc_amount = "0.995";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  output p2wpkh_output(output_amount, p2wpkh_operations);

  // Build & Sign Transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(input0);
  tx.outputs().push_back(p2wpkh_output);

  // Signature of input 0.
  endorsement sig_0;
  uint8_t input0_index(0u);
  script::create_endorsement(sig_0, my_secret0, prev_p2pkh_output_script,
      tx, input0_index, sighash_algorithm::all);

  // Input Script.
  operation::list p2pkh_operations;
  p2pkh_operations.push_back(operation(sig_0));
  p2pkh_operations.push_back(operation(to_chunk(pubkey0)));
  script p2pkh_input_script(p2pkh_operations);

  // Complete Transaction.
  tx.inputs()[0].set_script(p2pkh_input_script);
  std::cout << encode_base16(tx.to_data()) << std::endl;

}


void example_from_p2wpkh() {

  //**************************************************************
  // SEND FROM P2WPKH.
  //**************************************************************

  // Output script.
  //    According to destination address.
  // Input script.
  //    []
  // Script code.
  //    P2PKH(public key hash).
  // Witness.
  //    [signature] [public key]

  // P2WPKH output.
  // 0 [20-byte hash160(public key)]
  operation::list p2wpkh_operations;
  p2wpkh_operations.push_back(operation(opcode::push_size_0));
  p2wpkh_operations.push_back(
      operation(to_chunk(bitcoin_short_hash(pubkey1))));
  // Build output.
  std::string btc_amount = "0.993";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  output p2wpkh_output(output_amount, p2wpkh_operations);

  // P2WPKH input.
  // Previous transaction hash.
  std::string prev_tx =
      "26c9768cdbb00332ff1052f27e71eb7e82b578bf02fb6d7eecfd0b43412e9d10";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO index.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input0 object.
  input p2wpkh_input;
  p2wpkh_input.set_previous_output(uxto_to_spend);
  p2wpkh_input.set_sequence(max_input_sequence);

  // Build Transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(p2wpkh_input);
  tx.outputs().push_back(p2wpkh_output);

  // Create signature for witness.
  // Script code.
  script p2wpkh_script_code = script::to_pay_key_hash_pattern(
      bitcoin_short_hash(pubkey_witness_aware));
  // Previous input amount.
  uint8_t input_index(0u);
  std::string prev_btc_amount = "0.995";
  uint64_t prev_amount;
  decode_base10(prev_amount, prev_btc_amount, btc_decimal_places);
  // Pass script_version::zero & prev input amount
  endorsement sig_1;
  script::create_endorsement(sig_1, my_secret_witness_aware, p2wpkh_script_code,
      tx, input_index, sighash_algorithm::all, script_version::zero,
      prev_amount);

  // Create witness.
  // 02 [signature] [publicKey]
  data_stack witness_stack;
  witness_stack.push_back(sig_1);
  witness_stack.push_back(to_chunk(pubkey_witness_aware));
  witness p2wpkh_witness(witness_stack);
  tx.inputs()[0].set_witness(p2wpkh_witness);

  // Serialize Transaction.
  std::cout << encode_base16(tx.to_data(true,true)) << std::endl;

}


void example_to_p2sh_p2wpkh() {

  //**************************************************************
  // SEND to P2SH(P2WPKH)
  //**************************************************************

  // Output script.
  //    0 [20-byte hash160(embedded script)]
  // Input script.
  //    According to previous output.
  // Script code.
  //    According to previous output.

  // P2PKH Input.
  // Previous transaction hash.
  std::string prev_tx =
      "e93e1e7aafb38269512592fa2225c5783f0e589ec5359a4602eac11033d5e09d";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO index.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input0 object.
  input input0;
  input0.set_previous_output(uxto_to_spend);
  input0.set_sequence(max_input_sequence);
  // Previous output script.
  script prev_output_script = script::to_pay_key_hash_pattern(
      bitcoin_short_hash(pubkey0));

  // P2SH(P2WPKH) output.
  // P2SH embedded script = P2WPKH(public key hash)
  //    0 [20-byte public key hash]
  short_hash keyhash_dest = bitcoin_short_hash(pubkey_witness_aware);
  operation::list p2wpkh_operations;
  p2wpkh_operations.push_back(operation(opcode::push_size_0));
  p2wpkh_operations.push_back(operation(to_chunk(keyhash_dest)));
  script p2wpkh_embedded_script(p2wpkh_operations);
  // P2SH output script.
  // hash160 [20-byte hash160(embedded script)] equal
  short_hash embedded_script_hash = bitcoin_short_hash(
      p2wpkh_embedded_script.to_data(false));
  script output_script = script::to_pay_script_hash_pattern(embedded_script_hash);
  // Build output.
  std::string btc_amount = "1.298";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  output p2sh_p2wpkh_output(output_amount, output_script);

  // Build transaction.
  transaction tx;
  tx.inputs().push_back(input0);
  tx.outputs().push_back(p2sh_p2wpkh_output);
  tx.set_version(1u);

  // Build endorsement.
  endorsement sig_0;
  uint8_t input_index(0u);
  script::create_endorsement(sig_0, my_secret0, prev_output_script, tx,
      input_index, sighash_algorithm::all);

  // Build input script.
  operation::list input_script_operations;
  input_script_operations.push_back(sig_0);
  input_script_operations.push_back(operation(to_chunk(pubkey0)));
  script input_script(input_script_operations);
  tx.inputs()[0].set_script(input_script);

  // Serialize transaction.
  std::cout << encode_base16(tx.to_data()) << std::endl;
}


void example_from_p2sh_p2wpkh() {

  //**************************************************************
  // SEND from P2SH(P2WPKH)
  //**************************************************************

  // Output script.
  //    According to destination address.
  // Input script: [p2sh embedded script]
  //    [0 <20-byte public key hash>]
  // Script code.
  //    P2PKH(hash160(public key))
  // Witness.
  //    [signature] [public key]

  // P2SH(P2WPKH) input.
  // Previous TX hash.
  std::string prev_tx =
      "8231a9027eca6f2bd7bdf712cd2368f0b6e8dd6005b6b348078938042178ffed";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash, prev_tx);
  // Previous UXTO index.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build P2SH(P2WPKH) input object.
  input p2sh_p2wpkh_input;
  p2sh_p2wpkh_input.set_previous_output(uxto_to_spend);
  p2sh_p2wpkh_input.set_sequence(max_input_sequence);

  // Build output.
  std::string btc_amount = "1.295";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  script output_script = script::to_pay_key_hash_pattern(
        bitcoin_short_hash(pubkey1));
  output p2pkh_output(output_amount, output_script);

  // Build transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(p2sh_p2wpkh_input);
  tx.outputs().push_back(p2pkh_output);

  // Create signature for witness.
  // Script code.
  script script_code = script::to_pay_key_hash_pattern(
        bitcoin_short_hash(pubkey_witness_aware));
  // Previous input amount.
  uint8_t input_index(0u);
  std::string btc_amount_in = "1.298";
  uint64_t input_amount;
  decode_base10(input_amount, btc_amount_in, btc_decimal_places);
  // Pass script_version::zero & prev input amount.
  endorsement sig;
  script::create_endorsement(sig, my_secret_witness_aware, script_code, tx,
      input_index, sighash_algorithm::all, script_version::zero, input_amount);

  // Input script.
  // embedded script = P2WPKH script
  short_hash keyhash_dest = bitcoin_short_hash(pubkey_witness_aware);
  operation::list p2wpkh_operations;
  p2wpkh_operations.push_back(operation(opcode::push_size_0));
  p2wpkh_operations.push_back(operation(to_chunk(keyhash_dest)));
  script p2wpkh_embedded_script(p2wpkh_operations);

  // Wrap (P2SH) embedded script in single single data push.
  data_chunk p2sh_embedded_script_chunk =
      to_chunk(p2wpkh_embedded_script.to_data(true));
  script p2sh_embedded_script_wrapper(p2sh_embedded_script_chunk, false);
  tx.inputs()[0].set_script(p2sh_embedded_script_wrapper);

  // Build witness.
  // 02 [signature] [public key]
  data_stack witness_stack;
  witness_stack.push_back(sig);
  witness_stack.push_back(to_chunk(pubkey_witness_aware));
  witness p2wpkh_witness(witness_stack);
  tx.inputs()[0].set_witness(p2wpkh_witness);

  // Serialize transaction.
  std::cout << encode_base16(tx.to_data(true,true)) << std::endl;

}


void example_to_p2wsh() {

  //**************************************************************
  // SEND TO P2WSH
  //**************************************************************

  // Output script.
  //    0 [32-byte sha256(witness script)]
  // Input script.
  //    According to previous output.
  // Script code.
  //    According to previous output.

  // P2PKH input.
  // Previous transaction hash.
  std::string prev_tx =
      "77ac219bd5ae60e9ca48dd0bbc73f49ca886982749bb0368ed1011e07ae87dac";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO:
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input object.
  input input0;
  input0.set_previous_output(uxto_to_spend);
  input0.set_sequence(max_input_sequence);
  // Previous P2PKH output script.
  script prev_output_script = script::to_pay_key_hash_pattern(
      bitcoin_short_hash(pubkey0));

  // Witness script: Multisig.
  uint8_t signatures(2u); //2 of 3
  point_list points;
  points.push_back(pubkey_witness_aware);
  points.push_back(pubkey_witness_aware1);
  points.push_back(pubkey_witness_aware2);
  script witness_script = script::to_pay_multisig_pattern(signatures, points);

  // P2WSH output script.
  //    0 [32-byte sha256(witness script)]
  hash_digest embedded_script_hash = sha256_hash(witness_script.to_data(false));
  operation::list p2sh_operations;
  p2sh_operations.push_back(operation(opcode::push_size_0));
  p2sh_operations.push_back(operation(to_chunk(embedded_script_hash)));

  // Build output.
  std::string btc_amount = "1.295";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  output p2wsh_output(output_amount, p2sh_operations);

  // Build transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(input0);
  tx.outputs().push_back(p2wsh_output);
  tx.set_locktime(0u); //not necessary

  // Create endorsement.
  endorsement sig;
  uint8_t input0_index(0u);
  script::create_endorsement(sig, my_secret0, prev_output_script, tx,
      input0_index, sighash_algorithm::all);

  // Create input script.
  operation::list input_script_operations0;
  input_script_operations0.push_back(operation(sig));
  input_script_operations0.push_back(operation(to_chunk(pubkey0)));
  tx.inputs()[0].set_script(input_script_operations0);

  // Complete transaction.
  std::cout << encode_base16(tx.to_data()) << std::endl;

}


void example_from_p2wsh() {

  //**************************************************************
  // SEND FROM P2WSH
  //**************************************************************

  // Output script.
  //    According to destination address.
  // Input script.
  //    ""
  // Script code.
  //    Witness script.
  // Witness.
  //    [signature0] [signature1] [...] [witness script]

  // Build p2pkh output.
  std::string btc_amount = "1.292";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  script output_script = script::to_pay_key_hash_pattern(
      bitcoin_short_hash(pubkey1));
  output p2pkh_output(output_amount, output_script);

  // P2WSH input.
  // Previous transaction hash.
  std::string prev_tx =
      "98f85a8b774242979c9f0be37faba32d01c1e93fb69b4e6fa4b241b837dba293";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input object.
  input p2wsh_input;
  p2wsh_input.set_previous_output(uxto_to_spend);
  p2wsh_input.set_sequence(max_input_sequence);

  // Build transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(p2wsh_input);
  tx.outputs().push_back(p2pkh_output);
  tx.set_locktime(0u);

  // Script code: witness script = e.g. multisig
  uint8_t signatures(2u); //2 of 3
  point_list points;
  points.push_back(pubkey_witness_aware);
  points.push_back(pubkey_witness_aware1);
  points.push_back(pubkey_witness_aware2);
  script witness_script = script::to_pay_multisig_pattern(signatures, points);

  // Create signatures for witness.
  endorsement sig0;
  endorsement sig1;
  uint8_t input0_index(0u);
  uint64_t input_amount;
  std::string btc_amount_in = "1.295";
  decode_base10(input_amount, btc_amount_in, btc_decimal_places);
  script::create_endorsement(sig0, my_secret_witness_aware, witness_script, tx,
      input0_index, sighash_algorithm::all, script_version::zero, input_amount);
  script::create_endorsement(sig1, my_secret_witness_aware1, witness_script, tx,
      input0_index, sighash_algorithm::all, script_version::zero, input_amount);

  // Create witness.
  data_stack witness_stack;
  data_chunk empty_chunk;
  witness_stack.push_back(empty_chunk);
  witness_stack.push_back(sig0);
  witness_stack.push_back(sig1);
  witness_stack.push_back(witness_script.to_data(false));
  witness p2wsh_witness(witness_stack);
  tx.inputs()[0].set_witness(p2wsh_witness);

  // Complete transaction.
  std::cout << encode_base16(tx.to_data(true,true)) << std::endl;

}


void example_to_p2sh_p2wsh() {

//**************************************************************
// SEND TO P2SH(P2WSH).
//**************************************************************

// Output script.
//    hash160 [20-byte hash160(embedded script)] equal
// Input script.
//    According to previous output.
// Script code.
//    According to previous output.

// P2PKH input.
// Previous transaction hash.
std::string prev_tx =
    "8e71c68c1e7388542bab54d3d0ded1f118fa8f49b240a8696488dce8d47593e5";
hash_digest prev_tx_hash;
decode_hash(prev_tx_hash,prev_tx);
// Previous UXTO index.
uint32_t index = 0;
output_point uxto_to_spend(prev_tx_hash, index);
// Build input0 object.
input p2pkh_input;
p2pkh_input.set_previous_output(uxto_to_spend);
p2pkh_input.set_sequence(max_input_sequence);
// Previous locking script.
script prev_output_script = script::to_pay_key_hash_pattern(
    bitcoin_short_hash(pubkey0));

// P2SH(P2WSH(multisig)) output.
// 2-of-3 Multisig witness script & script code.
uint8_t signatures(2u); //2 of 3
point_list points;
points.push_back(pubkey_witness_aware);
points.push_back(pubkey_witness_aware1);
points.push_back(pubkey_witness_aware2);
script witness_script = script::to_pay_multisig_pattern(signatures, points);
// P2WSH(Multisig) embedded script.
//    0 [34-byte sha256(witness script)]
hash_digest witness_script_hash = sha256_hash(witness_script.to_data(false));
operation::list p2wsh_operations;
p2wsh_operations.push_back(operation(opcode::push_size_0));
p2wsh_operations.push_back(operation(to_chunk(witness_script_hash)));
script p2wsh_script(p2wsh_operations);
// P2SH(P2WSH) output script.
//    hash160 [sha256(embedded script)] equal
short_hash embedded_script_hash = bitcoin_short_hash(p2wsh_script.to_data(false));
script output_script = script::to_pay_script_hash_pattern(embedded_script_hash);
// Build output.
std::string btc_amount = "0.648";
uint64_t output_amount;
decode_base10(output_amount, btc_amount, btc_decimal_places);
output p2sh_p2wpkh_output(output_amount, output_script);

// Build transaction.
transaction tx;
tx.set_version(1u);
tx.inputs().push_back(p2pkh_input);
tx.outputs().push_back(p2sh_p2wpkh_output);
tx.set_locktime(0u);

// Create endorsement.
endorsement sig;
uint8_t input0_index(0u);
script::create_endorsement(sig, my_secret0, prev_output_script, tx,
    input0_index, sighash_algorithm::all);

// Create input script.
operation::list input_script_operations0;
input_script_operations0.push_back(operation(sig));
input_script_operations0.push_back(operation(to_chunk(pubkey0)));

// Complete transaction.
tx.inputs()[0].set_script(input_script_operations0);
std::cout << encode_base16(tx.to_data()) << std::endl;

}


void example_from_p2sh_p2wsh() {

  //**************************************************************
  // SEND FROM P2SH(P2WSH).
  //**************************************************************

  // Output script.
  //    According to destination address.
  // Input script.
  //    [0 [34-byte sha256(witness script)]]
  // Script code.
  //    Witness script.
  // Witness.
  //    [signature0] [signature1] [...] [witness script]

  // P2WPKH output.
  // 0 [20-byte hash160(public key)]
  operation::list p2wpkh_operations;
  p2wpkh_operations.push_back(operation(opcode::push_size_0));
  p2wpkh_operations.push_back(operation(to_chunk(bitcoin_short_hash(pubkey1))));
  // Build output.
  std::string btc_amount = "0.647";
  uint64_t output_amount;
  decode_base10(output_amount, btc_amount, btc_decimal_places);
  output p2wpkh_output(output_amount, p2wpkh_operations);

  // P2SH(P2WSH(Multisig)) input.
  // Previous transaction hash.
  std::string prev_tx =
      "40d5ecc46fb5f99971cda1fcd1bc0d465b0ce3f80176f30ef0b36f89c5568270";
  hash_digest prev_tx_hash;
  decode_hash(prev_tx_hash,prev_tx);
  // Previous UXTO.
  uint32_t index = 0;
  output_point uxto_to_spend(prev_tx_hash, index);
  // Build input object.
  input p2sh_p2wsh_input;
  p2sh_p2wsh_input.set_previous_output(uxto_to_spend);
  p2sh_p2wsh_input.set_sequence(max_input_sequence);

  // Build transaction.
  transaction tx;
  tx.set_version(1u);
  tx.inputs().push_back(p2sh_p2wsh_input);
  tx.outputs().push_back(p2wpkh_output);
  tx.set_locktime(0u);

  // Witness script: multisig = Script code
  uint8_t signatures(2u); //2 of 3
  point_list points;
  points.push_back(pubkey_witness_aware);
  points.push_back(pubkey_witness_aware1);
  points.push_back(pubkey_witness_aware2);
  script witness_script = script::to_pay_multisig_pattern(signatures, points);

  // Create signatures for witness.
  endorsement sig0;
  endorsement sig1;
  uint8_t input0_index(0u);
  std::string btc_amount_in = "0.648";
  uint64_t input_amount;
  decode_base10(input_amount, btc_amount_in, btc_decimal_places);
  script::create_endorsement(sig0, my_secret_witness_aware, witness_script, tx,
        input0_index, sighash_algorithm::all, script_version::zero, input_amount);
  script::create_endorsement(sig1, my_secret_witness_aware1, witness_script, tx,
        input0_index, sighash_algorithm::all, script_version::zero, input_amount);

  // P2SH output script.
  //    0 <34-byte sha256(witness script)>
  hash_digest witness_script_hash = sha256_hash(witness_script.to_data(false));
  operation::list p2wsh_operations;
  p2wsh_operations.push_back(operation(opcode::push_size_0));
  p2wsh_operations.push_back(operation(to_chunk(witness_script_hash)));
  script p2wsh_script(p2wsh_operations);

  // Input script.
  // Wrap embedded script as single data push (P2SH).
  data_chunk p2sh_embedded_script_chunk = to_chunk(p2wsh_script.to_data(true));
  script p2sh_embedded_script_wrapper(p2sh_embedded_script_chunk, false);
  tx.inputs()[0].set_script(p2sh_embedded_script_wrapper);

  // Create witness.
  data_stack witness_stack;
  data_chunk empty_chunk;
  witness_stack.push_back(empty_chunk);
  witness_stack.push_back(sig0);
  witness_stack.push_back(sig1);
  witness_stack.push_back(witness_script.to_data(false));
  witness p2wsh_witness(witness_stack);
  tx.inputs()[0].set_witness(p2wsh_witness);

  // Complete transaction
  std::cout << encode_base16(tx.to_data(true,true)) << std::endl;

}


int main() {

  // example_to_p2wpkh();
  example_from_p2wpkh();

  // example_to_p2sh_p2wpkh();
  // example_from_p2sh_p2wpkh();
  //
  // example_to_p2wsh();
  // example_from_p2wsh();
  //
  // example_to_p2sh_p2wsh();
  // example_from_p2sh_p2wsh();

  return 0;

}

// g++ -std=c++11 p2wpkh.cpp -o p2wpkh  $(pkg-config --cflags libbitcoin --libs libbitcoin)

#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {

  // Wallet setup.
  auto my_secret = bc::base16_literal(
    "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");
  bc::wallet::ec_private my_private(my_secret,
    bc::wallet::ec_private::testnet, true);
  bc::wallet::ec_public my_public(my_private);

  // Define uxto with CLTV script to spend.
  // ---------------------------------------------------------------------------

  auto previous_tx_literal =
    "30a2461cf6755aba4fc7da31aba55a76b8360af69c4f3199a94efadaf949582b";
  bc::hash_digest previous_tx_hash;
  bc::decode_hash(previous_tx_hash,previous_tx_literal); // bitcoin hash digest.
  uint32_t previous_index(0u);
  uint64_t previous_amount(199999300);
  bc::chain::output_point previous_output_point(previous_tx_hash,previous_index);

  // Build Transaction which spends timelocked p2pkh output.
  // ---------------------------------------------------------------------------

  bc::chain::transaction my_tx;

  // Set to equal or greater cltv_expiry from previous output script.
  // 1353531 (0x0014A73A)
  uint32_t locktime(0x0014A73B);
  my_tx.set_locktime(locktime);
  uint32_t version(1u);
  my_tx.set_version(version);

  // Input.
  bc::chain::input my_input;
  my_input.set_previous_output(previous_output_point);
  my_input.set_sequence(0xfffffffe); // must be less than 0xffffffff
  my_tx.inputs().push_back(my_input);
  // Get previous output script.

  // Output.
  uint64_t my_output_amount = previous_amount-700; // 700 sat fee
  bc::wallet::payment_address receiving_address(
    "mg9cHv7hWMQtzL11nhVr2cyAfDHJB2xnFW");
  bc::machine::operation::list my_output_script =
    bc::chain::script::to_pay_key_hash_pattern(receiving_address.hash());
  bc::chain::output my_output(my_output_amount, my_output_script);
  my_tx.outputs().push_back(my_output);

  // Create Endorsement.
  // ---------------------------------------------------------------------------

  // Previous output script.
  // <0x3aa71400> OP_CHECKLOCKTIMEVERIFY OP_DROP OP_DUP OP_HASH160 <1d71dc526556c6ae19fc10b2622fe7e4396b912a> OP_EQUALVERIFY OP_CHECKSIG

  bc::data_chunk cltv_expiry{ {0x3a, 0xa7, 0x14, 0x00} };
  auto previous_output_script =
    bc::chain::script::to_pay_key_hash_pattern(
    bc::base16_literal("1d71dc526556c6ae19fc10b2622fe7e4396b912a"));
  previous_output_script.insert(previous_output_script.begin(),
    bc::machine::operation(bc::machine::opcode::drop));
  previous_output_script.insert(previous_output_script.begin(),
    bc::machine::operation(bc::machine::opcode::checklocktimeverify));
  previous_output_script.insert(previous_output_script.begin(),
    bc::machine::operation(bc::machine::operation(cltv_expiry)));
  bc::endorsement my_signature;
  bc::chain::script::create_endorsement(my_signature, my_secret,
    previous_output_script, my_tx, 0, 0x01); // index 0, sighash ALL

  // Create endorsement/input script
  bc::machine::operation::list my_input_script_operations;
  my_input_script_operations.push_back(bc::machine::operation(my_signature));
  my_input_script_operations.push_back(bc::machine::operation(
    bc::to_chunk(my_public.point())));
  bc::chain::script my_input_script(my_input_script_operations);

  // Set endorsement/input script in transaction.
  my_tx.inputs()[0].set_script(my_input_script);

  // Broadcast transaction.
  std::cout << bc::encode_base16(my_tx.to_data()) << std::endl;

}

// Compile script with:
// g++ -std=c++11 cltv_spend.cpp -o cltv_spend  $(pkg-config --cflags libbitcoin --libs libbitcoin)

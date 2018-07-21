#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {

  // Send funds to address spendable by secret.
  // Address spendable by secret below: miCeHnvJnGhfM1RzE3mdt6TYFAHL5Lw4ep

  // Wallet setup.
  auto my_secret = bc::base16_literal(
    "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");
  bc::wallet::ec_private my_private(my_secret,
    bc::wallet::ec_private::testnet, true);
  bc::wallet::ec_public my_public(my_private);

  // Define output point with testnet coins spendable by wallet.
  // ---------------------------------------------------------------------------

  auto previous_tx_literal =
    "2bf7416a3d82be829b7a20458fbbca6adfe9bfcd0a41589a5d319c345a35aee1";
  bc::hash_digest previous_tx_hash;
  bc::decode_hash(previous_tx_hash,previous_tx_literal); // bitcoin hash digest.
  uint32_t previous_index(1);
  uint64_t previous_amount(130000000);
  bc::chain::output_point previous_output_point(previous_tx_hash,previous_index);

  // Build transaction input with relative timelock.
  // ---------------------------------------------------------------------------

  bc::chain::transaction my_tx;
  my_tx.set_locktime(0);
  my_tx.set_version(2);   // for relative timelock 2 and higher.

  // Input.
  bc::chain::input my_input;

  uint32_t type_flag = 1 << 22;
  uint32_t relative_locktime(0x00000003);

  // A) Delay in multiples of 512s:
  // my_input.set_sequence(relative_locktime | type_flag);
  // B) Delay in blocks:
  my_input.set_sequence(relative_locktime);

  my_input.set_previous_output(previous_output_point);
  my_tx.inputs().push_back(my_input);

  // Output.
  uint64_t my_output_amount = previous_amount-700;
  bc::wallet::payment_address receiving_address(
    "mg9cHv7hWMQtzL11nhVr2cyAfDHJB2xnFW");
  bc::machine::operation::list my_output_script =
    bc::chain::script::to_pay_key_hash_pattern(receiving_address.hash());
  bc::chain::output my_output(my_output_amount, my_output_script);
  my_tx.outputs().push_back(my_output);

  // Previous output script:
  // "dup hash160 [1d71dc526556c6ae19fc10b2622fe7e4396b912a] equalverify checksig"
  auto previous_output_script =
    bc::chain::script::to_pay_key_hash_pattern(
    bc::base16_literal("1d71dc526556c6ae19fc10b2622fe7e4396b912a"));

  // Create endorsement.
  bc::endorsement my_signature;
  bc::chain::script::create_endorsement(my_signature, my_secret,
    previous_output_script, my_tx, 0, 0x01); // index 0, sighash ALL

  // Create input script (w/ endorsment).
  bc::machine::operation::list my_input_script_operations;
  my_input_script_operations.push_back(bc::machine::operation(my_signature));
  my_input_script_operations.push_back(bc::machine::operation(
    bc::to_chunk(my_public.point())));
  bc::chain::script my_input_script(my_input_script_operations);

  my_tx.inputs()[0].set_script(my_input_script);

  // Print out transaction.
  std::cout << bc::encode_base16(my_tx.to_data()) << std::endl;

}

// Compile script with:
// g++ -std=c++11 nsequence.cpp -o nsequence  $(pkg-config --cflags libbitcoin --libs libbitcoin)

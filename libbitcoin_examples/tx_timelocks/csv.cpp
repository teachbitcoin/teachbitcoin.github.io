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
    "b9ddebe6c3faa7f038b2bdf3cc6be9b5b34599f2d7e51d6590a5344b6565941b";
  bc::hash_digest previous_tx_hash;
  bc::decode_hash(previous_tx_hash,previous_tx_literal); // bitcoin hash digest.
  uint32_t previous_index(1);
  uint64_t previous_amount(65000000);
  bc::chain::output_point previous_output_point(previous_tx_hash,previous_index);

  // Build Transaction.
  // ---------------------------------------------------------------------------

  bc::chain::transaction my_tx;
  my_tx.set_locktime(0);
  my_tx.set_version(2);   // for relative timelock 2 and higher.

  // Input.
  bc::chain::input my_input;
  my_input.set_previous_output(previous_output_point);
  my_input.set_sequence(0xefffffff); // must be less than 0xf0000000
  my_tx.inputs().push_back(my_input);

  // Create output script with CSV.
  // ---------------------------------------------------------------------------

  // 5 block delay. 3 Bytes, Little Endian.
  bc::data_chunk csv_delay{ {0x05, 0x00, 0x00} };

  auto my_output_script =
    bc::chain::script::to_pay_key_hash_pattern(
    bc::base16_literal("1d71dc526556c6ae19fc10b2622fe7e4396b912a"));
  my_output_script.insert(my_output_script.begin(),
    bc::machine::operation(bc::machine::opcode::drop));
  my_output_script.insert(my_output_script.begin(),
    bc::machine::operation(bc::machine::opcode::checksequenceverify));
  my_output_script.insert(my_output_script.begin(),
    bc::machine::operation(bc::machine::operation(csv_delay)));

  // Create output
  uint64_t my_output_amount = previous_amount-700; // 700 sat fee
  bc::chain::output my_output(my_output_amount, my_output_script);
  my_tx.outputs().push_back(my_output);

  // Create Endorsement.
  // ---------------------------------------------------------------------------

  // Previous output script:
  // "dup hash160 [1d71dc526556c6ae19fc10b2622fe7e4396b912a] equalverify checksig"

  auto previous_output_script =
    bc::chain::script::to_pay_key_hash_pattern(
    bc::base16_literal("1d71dc526556c6ae19fc10b2622fe7e4396b912a"));

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

// g++ -std=c++11 csv.cpp -o csv $(pkg-config --cflags libbitcoin --libs libbitcoin)

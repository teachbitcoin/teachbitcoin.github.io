#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {

  // Send funds to address spendable by secret.
  // Testnet Address:
  // $ bx ec-to-public [secret] | bx sha256 | bx ripemd160 | bx address-encode

  auto my_secret = bc::base16_literal(
      "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");

  bc::wallet::ec_private my_private(
    my_secret,
    bc::wallet::ec_private::testnet,
    true);

  bc::wallet::ec_public my_public(my_private);

  // Define output point with coins spendable by my_secret.
  auto previous_tx_literal = "b421c87fd9db2dc8eedf545de5375c26497dc9c1a94067c44ca71572d5b9d268";
  bc::hash_digest previous_tx_hash;
  bc::decode_hash(previous_tx_hash,previous_tx_literal); // bitcoin hash digest.
  uint32_t previous_index(1);
  bc::chain::output_point previous_output_point(previous_tx_hash,previous_index);

  // Build Transaction with nlocktime.
  // ---------------------------------------------------------------------------

  bc::chain::transaction my_tx;

  // Unix timestamp: 1531501532 seconds since Jan 01 1970.
  // uint32_t locktime( 1531501532);
  // Alternatively by blockheight
  uint32_t locktime(1353381);


  my_tx.set_locktime(locktime);

  uint32_t version(1u);
  my_tx.set_version(version);

  // Input.
  bc::chain::input my_input;
  my_input.set_previous_output(previous_output_point);
  my_input.set_sequence(0xfffffffe);
  my_tx.inputs().push_back(my_input);
  // Get previous output script.

  // Output.
  uint64_t my_output_amount(130000000-700); // 500 sat fee
  bc::wallet::payment_address receiving_address(
    "mg9cHv7hWMQtzL11nhVr2cyAfDHJB2xnFW");
  bc::machine::operation::list my_output_script =
    bc::chain::script::to_pay_key_hash_pattern(receiving_address.hash());
  bc::chain::output my_output(my_output_amount, my_output_script);
  my_tx.outputs().push_back(my_output);

  // Sign.
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

// Compile script with:
// g++ -std=c++11 nlocktime.cpp -o nlocktime  $(pkg-config --cflags libbitcoin --libs libbitcoin)

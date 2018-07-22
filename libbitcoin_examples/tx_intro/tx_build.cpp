#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {

  // Setup UXTOs for transaction inputs.
  // ---------------------------------------------------------------------------

  // Send funds to addresses spendable by secret 0 and secret 1.
  // Testnet Addresses:
  // $ bx ec-to-public [secret] | bx sha256 | bx ripemd160 | bx address-encode

  auto my_secret0 = bc::base16_literal(
    "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");
  auto my_secret1 = bc::base16_literal(
    "86faa240ae2b0f28b125a42961bd3adf9d5f5dc6a1deaa5feda04e7be8c872f6");

  bc::wallet::ec_private my_private0(my_secret0, bc::wallet::ec_private::testnet,
     true);
  bc::wallet::ec_private my_private1(my_secret1, bc::wallet::ec_private::testnet,
     true);

  // Define output 0 point with coins spendable by secret 0.
  auto previous_tx_literal0 =
    "91ae295629dcb4ab9bafdc51b571b06595a2236cc23a5ab414749f2a88f8c9a4";
  bc::hash_digest previous_tx_hash0;
  bc::decode_hash(previous_tx_hash0,previous_tx_literal0); //bitcoin hash digest.
  uint32_t previous_index0(1);
  uint64_t previous_amount0(130000000);
  bc::chain::output_point previous_output_point0(previous_tx_hash0,
    previous_index0);

  // Define output 1 point with coins spendable by secret 1.
  auto previous_tx_literal1 =
    "fcf4d04ac0448e2af401e245f905db159a84c4c2f32b924e8e971b3c50fd2175";
  bc::hash_digest previous_tx_hash1;
  bc::decode_hash(previous_tx_hash1,previous_tx_literal1); //bitcoin hash digest.
  uint32_t previous_index1(0);
  uint64_t previous_amount1(65000000);
  bc::chain::output_point previous_output_point1(previous_tx_hash1,
    previous_index1);

  // Create Transaction for Signing.
  // ---------------------------------------------------------------------------

  // Version.
  uint32_t version = 1u;
  bc::chain::transaction tx;
  tx.set_version(version);

  // Build & Add inputs to transaction.
  bc::chain::input my_input0;
  my_input0.set_previous_output(previous_output_point0);
  my_input0.set_sequence(0xffffffff);
  tx.inputs().push_back(my_input0);

  bc::chain::input my_input1;
  my_input1.set_previous_output(previous_output_point1);
  my_input1.set_sequence(0xffffffff);
  tx.inputs().push_back(my_input1);

  // Build & Add outputs to transaction.
  uint64_t my_output_amount0(previous_amount0-800); //fee
  bc::wallet::payment_address receiving_address0(
    "mg9cHv7hWMQtzL11nhVr2cyAfDHJB2xnFW");
  bc::machine::operation::list my_output_script0 =
    bc::chain::script::to_pay_key_hash_pattern(receiving_address0.hash());
  bc::chain::output my_output0(my_output_amount0, my_output_script0);
  tx.outputs().push_back(my_output0);

  uint64_t my_output_amount1(previous_amount1-800); //fee
  bc::wallet::payment_address receiving_address1(
    "mg9cHv7hWMQtzL11nhVr2cyAfDHJB2xnFW");
  bc::machine::operation::list my_output_script1 =
    bc::chain::script::to_pay_key_hash_pattern(receiving_address1.hash());
  bc::chain::output my_output1(my_output_amount1, my_output_script1);
  tx.outputs().push_back(my_output1);


  // Build transaction inputs 0 and 1 with sighash ALL endorsements.
  // ---------------------------------------------------------------------------

  // Fetch previous output scripts.
  // $ bx fetch-tx [previous txid]
  // $ bx script-encode "dup hash160 ... equalverify checksig"

  auto previous_output_script0_literal = bc::base16_literal(
    "76a9141d71dc526556c6ae19fc10b2622fe7e4396b912a88ac");
  bc::chain::script previous_output_script0(
    bc::to_chunk(previous_output_script0_literal),false);

  auto previous_output_script1_literal = bc::base16_literal(
    "76a91442b9b7745ec8b14788f0ca7ac28150782351d44788ac");
  bc::chain::script previous_output_script1(
    bc::to_chunk(previous_output_script1_literal),false);

  bc::endorsement my_signature0;
  bc::chain::script::create_endorsement(my_signature0, my_secret0,
    previous_output_script0, tx, 0, bc::machine::sighash_algorithm::all);
    // index 0, sighash ALL

  bc::endorsement my_signature1;
  bc::chain::script::create_endorsement(my_signature1, my_secret1,
    previous_output_script1, tx, 1, bc::machine::sighash_algorithm::all);
    // index 1, sighash ALL

  // Create endorsements.
  bc::machine::operation::list my_input_script_operations0;
  my_input_script_operations0.push_back(bc::machine::operation(my_signature0));
  my_input_script_operations0.push_back(bc::machine::operation(
    bc::to_chunk(my_private0.to_public().point())));
  bc::chain::script my_input_script0(my_input_script_operations0);

  bc::machine::operation::list my_input_script_operations1;
  my_input_script_operations1.push_back(bc::machine::operation(my_signature1));
  my_input_script_operations1.push_back(bc::machine::operation(
    bc::to_chunk(my_private1.to_public().point())));
  bc::chain::script my_input_script1(my_input_script_operations1);

  // Set endorsement script in transaction.
  tx.inputs()[0].set_script(my_input_script0);
  tx.inputs()[1].set_script(my_input_script1);

  // Print out serialised tx.
  std::cout << bc::encode_base16(tx.to_data()) << std::endl;

  // Verify TX with block explorer.
  // ---------------------------------------------------------------------------

  // $ ./sighash_all
  // $ bx validate-tx [serialised tx]
  // The transaction is valid.

}

// Compile script with:
// g++ -std=c++11 tx_build.cpp -o tx_build  $(pkg-config --cflags libbitcoin --libs libbitcoin)

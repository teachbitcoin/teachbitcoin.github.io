#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {


  // 0) Create scalar & uncompressed secp256k1 point from hex string.
  // ---------------------------------------------------------------------------

  bc::ec_secret my_scalar = bc::base16_literal(
    "28026f91e1c97db3f6453262484ef5f69f71d89474f10926aae24d3c3eeb5f00");

  bc::ec_uncompressed my_uncompressed_point = bc::base16_literal(
    "04"
    "28026f91e1c97db3f6453262484ef5f69f71d89474f10926aae24d3c3eeb5f00"
    "c41b6810b8b305a05de2b4448d7e2a079771d4c018b923a9ab860e4b0b4f86f6");


  // 1) Create a random private key and derive the public key.
  // ---------------------------------------------------------------------------

  bc::data_chunk my_entropy(bc::ec_secret_size); // Secret size: 32bytes.
  bc::pseudo_random_fill(my_entropy);

  // Convert data_chunk (byte vector) to ec_secret (byte array).
  bc::ec_secret my_random_secret = bc::to_array<bc::ec_secret_size>(my_entropy);


  // 2) Verify that secret is in valid range and derive public key.
  // ---------------------------------------------------------------------------

  if( bc::verify(my_random_secret) )
  {

    bc::ec_compressed generator_point = bc::base16_literal(
      "02"
      "79be667ef9dcbbac55a06295Ce870b07029bfcdb2dce28d959f2815b16f81798");

    // Multiply with secp256k1 generator point.
    bc::ec_multiply(generator_point, my_random_secret);
    bc::ec_compressed my_public_key(generator_point);

    // Point is on secp256k1 curve, since secret was valid.
    std::cout << "Public Key point is on secp256k1 curve: "
              << bc::verify(my_public_key) << std::endl;

    // Print out compressed public key.
    // (bc::encode_base16 requires data chunk (byte vector) input.)
    std::cout << "Compressed Public Key Point: "
              << bc::encode_base16(bc::to_chunk(my_public_key)) << std::endl;


    // 3) Better: Derive public key directly from secret.
    // ---------------------------------------------------------------------------

    bc::ec_compressed my_public_key_2;
    bc::secret_to_public(my_public_key_2, my_random_secret);
    std::cout << "Public Key Derivation is consistent with secret_to_public: "
              << (my_public_key == my_public_key_2) << std::endl;

  }

  else
  {
    std::cout << "Invalid secret chosen" << std::endl;
  }

  return 0;

}

// g++ -std=c++11 -o ec_math ec_math.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin)

#include <bitcoin/bitcoin.hpp>
#include <iostream>
#include <secp256k1.h>
#include "secp256k1_initializer.hpp"

int main() {

  bc::data_chunk my_entropy_128(16);
  bc::pseudo_random_fill(my_entropy_128);
  bc::wallet::hd_private m(my_entropy_128, bc::wallet::hd_private::mainnet);

  // Parent M public key & chaincode are exposed.
  bc::wallet::hd_public M = m.to_public();

  // Child m_0 secret is exposed.
  auto m_0_secret = m.derive_private(0).secret();

  // 1) Derive L256 from hmac_sha512_hash(parent public key||index, chaincode).
  // ---------------------------------------------------------------------------
  uint32_t index = 0;
  auto data = bc::splice(M.point(), bc::to_big_endian(index));
  auto intermediate = bc::split(bc::hmac_sha512_hash(data, M.chain_code()));
auto left_256 = intermediate.left;

  // 2) Parent Private(m) = child private(m_0) - L256.
  // ---------------------------------------------------------------------------

  const auto context = bc::verification.context();
  if (secp256k1_ec_privkey_negate(context,left_256.data()) != 1)
  {
    return 1;
  }
  bc::ec_add(left_256, m_0_secret);

  // 3) Verify parent private key derivation is correct.
  // ---------------------------------------------------------------------------
  std::cout << "Derived Parent Private Key is correct: "
            << (left_256 == m.secret()) << std::endl;
  std::cout << "Parent Private Key: "
            << bc::encode_base16(bc::to_chunk(m.secret())) << std::endl;

  return 0;
}

// Compile.
// g++ -std=c++11 -c -o secp256k1_initializer.o secp256k1_initializer.cpp
// g++ -std=c++11 -c -o hd_exposure.o hd_exposure.cpp $(pkg-config --cflags libbitcoin)

// Link.
// g++ hd_exposure.o secp256k1_initializer.o $(pkg-config --libs libbitcoin) -o hd_exposure

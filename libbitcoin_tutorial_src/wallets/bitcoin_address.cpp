#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

int main() {

  auto my_secret = bc::base16_literal(
      "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");

  bc::ec_compressed my_public_key;
  bc::secret_to_public(my_public_key, my_secret);

  // Public key hash.
  auto my_public_key_hash = bc::bitcoin_short_hash(my_public_key);

  auto addr_prefix = bc::wallet::ec_private::mainnet_p2kh; // Mainnet = 0x00
  // auto addr_prefix = bc::wallet::ec_private::testnet_p2kh; // Testnet 0x6f

  // Prefix || public key || checksum(4-bytes)
  bc::data_chunk prefix_publickey_checksum(bc::to_chunk(addr_prefix));
  bc::extend_data(prefix_publickey_checksum, my_public_key_hash);
  bc::append_checksum(prefix_publickey_checksum);

  // Bitcoin Address: Base58 encode byte sequence.
  std::cout << bc::encode_base58(prefix_publickey_checksum) << std::endl;

}

// g++ -std=c++11 bitcoin_address.cpp -o bitcoin_address  $(pkg-config --cflags libbitcoin --libs libbitcoin)

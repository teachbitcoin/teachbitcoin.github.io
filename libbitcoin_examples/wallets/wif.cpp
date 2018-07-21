#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

int main() {

  auto my_secret = bc::base16_literal(
      "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");

  auto prefix_secret_comp_checksum =
      bc::to_chunk(bc::wallet::ec_private::mainnet_wif); // 0x80
  // auto prefix_secret_comp_checksum =
  //     bc::to_chunk(bc::wallet::ec_private::mainnet_testnet); // 0xEF

  bc::extend_data(prefix_secret_comp_checksum, my_secret);
  bc::extend_data(prefix_secret_comp_checksum,
                  bc::to_chunk(bc::wallet::ec_private::compressed_sentinel)); // 0x01
  bc::append_checksum(prefix_secret_comp_checksum);

  // WIF (mainnet/compressed)
  std::cout << bc::encode_base58(prefix_secret_comp_checksum) << std::endl;

}

// g++ -std=c++11 wif.cpp -o wif  $(pkg-config --cflags libbitcoin --libs libbitcoin)

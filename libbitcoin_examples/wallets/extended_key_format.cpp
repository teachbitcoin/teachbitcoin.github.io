#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

int main() {

  // Derive Master Keys from mnemonic.
  std::string my_wordlist_string = "market parent marriage drive umbrella "
                                   "custom leisure fury recipe steak have "
                                   "enable";
  auto my_wordlist = bc::split(my_wordlist_string, " ", true);
  auto hd_seed = bc::to_chunk(bc::wallet::decode_mnemonic(my_wordlist));
  bc::wallet::hd_private m(hd_seed, bc::wallet::hd_private::mainnet);
  auto m1 = m.derive_private(1);

  // Extended Private Key:
  auto m1_xprv = m1.to_hd_key();

  std::cout << "Version (4B): "
            << bc::encode_base16(bc::slice<0,4>(m1_xprv)) << std::endl;

  std::cout << "Depth (1B): "
            << bc::encode_base16(bc::slice<4,5>(m1_xprv)) << std::endl;

  std::cout << "Parent Fingerprint (4B): "
            << bc::encode_base16(bc::slice<5,9>(m1_xprv)) << std::endl;

  std::cout << "Index (4B): "
            << bc::encode_base16(bc::slice<9,13>(m1_xprv)) << std::endl;

  std::cout << "Chain Code (32B): "
            << bc::encode_base16(bc::slice<13,45>(m1_xprv)) << std::endl;

  std::cout << "Private Key (32B): "
            << bc::encode_base16(bc::slice<45,78>(m1_xprv)) << std::endl;

  std::cout << "Checksum (4B): "
            << bc::encode_base16(bc::slice<78,82>(m1_xprv)) << std::endl;

  // Verify Checksum.
  auto checksum_msg = bc::to_chunk(bc::slice<0,78>(m1_xprv));
  bc::append_checksum(checksum_msg);
  auto checksum = bc::slice<78,82>(bc::to_array<82>(checksum_msg));
  std::cout << "Checksum verification: "
            << (checksum == bc::slice<78,82>(m1_xprv)) << std::endl;

  // Print out XPRV in base58 encoded format.
  std::cout << "Base58 encoded XPRV format: "
            << bc::encode_base58(bc::to_chunk(m1_xprv)) << std::endl;

}

// g++ -std=c++11 extended_key_format.cpp -o extended_key_format $(pkg-config --cflags libbitcoin --libs libbitcoin)

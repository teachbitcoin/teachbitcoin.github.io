#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

int main() {

  // 128, 160, 192, 224, 256 bits of Entropy are valid

  // We generate 128 bits of Entropy
  bc::data_chunk my_entropy_128(16); //16 bytes = 128 bits
  bc::pseudo_random_fill(my_entropy_128);

  // We generate 160 bits of Entropy
  bc::data_chunk my_entropy_160(20); //16 bytes = 128 bits
  bc::pseudo_random_fill(my_entropy_160);

  // ...

  // We generate 256 bits of Entropy
  bc::data_chunk my_entropy_256(32); //32 bytes = 256 bits
  bc::pseudo_random_fill(my_entropy_256);

  // Instantiate mnemonic word_list from entropy.
  bc::wallet::word_list my_wordlist_128 = bc::wallet::create_mnemonic(my_entropy_128);
  bc::wallet::word_list my_wordlist_160 = bc::wallet::create_mnemonic(my_entropy_160);
  bc::wallet::word_list my_wordlist_256 = bc::wallet::create_mnemonic(my_entropy_256);
  std::cout << "128bit wordlist: " << bc::join(my_wordlist_128) << std::endl;
  std::cout << "160bit wordlist: "<< bc::join(my_wordlist_160) << std::endl;
  std::cout << "256bit wordlist: "<< bc::join(my_wordlist_256) << std::endl;

  // Generate a 512-bit HD Wallet Seed from mnemonic.
  auto seed = bc::to_chunk(bc::wallet::decode_mnemonic(my_wordlist_128));
  std::cout << "512bit HD Wallet Seed: "
            << bc::encode_base16(seed) << std::endl;

}

// g++ -std=c++11 mnemonic_wordlist.cpp -o mnemonic_wordlist  $(pkg-config --cflags libbitcoin --libs libbitcoin)

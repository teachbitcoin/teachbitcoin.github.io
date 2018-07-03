#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

int main() {

  // 1) Derivation of Master Extended Keys from Mnemonic word list.
  // ---------------------------------------------------------------------------

  // Load mnemonic sentence into word list
  std::string my_wordlist_string = "market parent marriage drive umbrella "
                                   "custom leisure fury recipe steak have "
                                   "enable";
  auto my_wordlist = bc::split(my_wordlist_string, " ", true);

  // Create 512bit seed (without optional secret passphrase)
  auto hd_seed = bc::to_chunk(bc::wallet::decode_mnemonic(my_wordlist));

  // Instantiate hd_private m with XPRV||XPUB mainnet prefixes.
  bc::wallet::hd_private m(hd_seed, bc::wallet::hd_private::mainnet);

  // Derivation of extended master key M (hd_public).
  auto M = m.to_public();


  // 2) Derivation of Child Extended Keys.
  // ---------------------------------------------------------------------------

  // Derive children of master private extended key m.
  auto m0 = m.derive_private(0);
  auto m1 = m.derive_private(1);
  auto m2 = m.derive_private(2);

  // Derive grandchild private extended keys.
  auto m10 = m1.derive_private(0); //Depth 2, Index 0
  auto m11 = m1.derive_private(1); //Depth 2, Index 1
  auto m12 = m1.derive_private(2); //Depth 2, Index 2
  auto m100 = m10.derive_private(0); //Depth 3, Index 0
  auto m101 = m10.derive_private(1); //Depth 3, Index 1
  auto m102 = m10.derive_private(2); //Depth 3, Index 1

  // Derive grandchild public extended keys
  auto M00 = m0.derive_public(0); //Depth 2, Index 0
  auto M01 = m0.derive_public(1); //Depth 2, Index 1
  auto M02 = m0.derive_public(2); //Depth 2, Index 2
  // ...

  // Derive extended public key of any extended private key.
  // of same depth & index
  auto M102 = m102.to_public();

  // Derive children of master public extended key M
  auto M0 = M.derive_public(0); //Depth 1, Index 0
  auto M1 = M.derive_public(1); //Depth 1, Index 1
  auto M2 = M.derive_public(2); //Depth 1, Index 2

  // Derive grandchild public extended keys.
  auto M10 = M1.derive_public(0); //Depth 2, Index 0
  auto M11 = M1.derive_public(1); //Depth 2, Index 1
  auto M12 = M1.derive_public(2); //Depth 2, Index 2
  auto M100 = M10.derive_public(0); //Depth 3, Index 0
  auto M101 = M10.derive_public(1); //Depth 3, Index 1
  // ...


  // 3) Derivation of Hardened Child Extended Keys.
  // ---------------------------------------------------------------------------

  // Hardened private key derivation with index >= 1 << 31
  auto m00h = m0.derive_private(bc::wallet::hd_first_hardened_key);
  auto m01h = m0.derive_private(1 + bc::wallet::hd_first_hardened_key);
  auto m02h = m0.derive_private(2 + bc::wallet::hd_first_hardened_key);

  // Hardened public key can only be derived from hardened extended private key.
  auto M00h = m00h.to_public();
  // or from parent extended private key.
  auto M00h_ = m0.derive_public(bc::wallet::hd_first_hardened_key);
  // Above keys are equivalent
  std::cout << "Hardened pubic key can only be derived from private key."
            << (M00h == M00h_) << std::endl;

}

// g++ -std=c++11 hd_children.cpp -o hd_children $(pkg-config --cflags libbitcoin --libs libbitcoin)

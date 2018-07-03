#include <bitcoin/bitcoin.hpp>
#include <iostream>

int main() {

  auto my_secret = bc::base16_literal(
      "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d");

  bc::wallet::ec_private my_private(
      my_secret,
      bc::wallet::ec_private::mainnet,
      true); // Public Key Compression = true.

  // WIF private key.
  std::cout << "WIF private key (Base58): "
						<< my_private.encoded() << std::endl;

  // 1st ec_public from ec_private (compression argument inherited)
  bc::wallet::ec_public my_public(my_private);

  // 2nd ec_public from point and true compression argument.
  bc::wallet::ec_public my_public2(my_public.point(), true);

  // Verify my_public/my_public2 are identical.
  std::cout << "my_public & my_public2 are equal: "
            << (my_public.encoded() == my_public2.encoded()) << std::endl;

  // Payment addresses: Default to mainnet.
  std::cout << "Mainnet Address: "
            << my_public.to_payment_address() << std::endl;
  std::cout << "Testnet Address: "
            << my_public.to_payment_address(bc::wallet::ec_private::testnet_p2kh)
            << std::endl;

}

// g++ -std=c++11 wallet.cpp -o wallet  $(pkg-config --cflags libbitcoin --libs libbitcoin)

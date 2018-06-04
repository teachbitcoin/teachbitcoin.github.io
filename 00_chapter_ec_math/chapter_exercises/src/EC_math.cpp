#include <bitcoin/bitcoin.hpp>
#include <iostream>
#include "../include/EC_math.hpp"

// Generate a bunch of points in low ranges, large ranges, and see if on curve.
// Commutative point addition.
// Multiply.

void ec_math_exercise_1a() {

    // TODO: 1a) Evaluate whether integers are members of Fp.

    bc::uint256_t my_int_1 = 0;
    bc::uint256_t my_int_2 = 101;
    // 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff.
    bc::uint256_t my_int_3 = std::numeric_limits<bc::uint256_t>::max();

    // Tip: Use int_to_bytes() helper function to convert int to byte_array.
    const auto my_byte_array_1 = int_to_bytes(my_int_1);
    const auto my_byte_array_2 = int_to_bytes(my_int_2);
    const auto my_byte_array_3 = int_to_bytes(my_int_3);

    // // Check whether all integers are members of Fp.
    // // Return true/false.
    // std::cout << bc::encode_base16(bc::to_chunk(my_byte_array_1)) << std::endl;
    // std::cout << bc::encode_base16(bc::to_chunk(my_byte_array_2)) << std::endl;
    // std::cout << bc::encode_base16(bc::to_chunk(my_byte_array_3)) << std::endl;
    //
    // // 0 not valid, because generator curve.
    // std::cout << bc::verify(my_byte_array_1) << std::endl;
    // std::cout << bc::verify(my_byte_array_2) << std::endl;
    // std::cout << bc::verify(my_byte_array_3) << std::endl;

}

void ec_math_exercise_1b() {

    //10, 0
    //6,8
    //20,2
    //8,25

    bc::uint256_t x1 = 0;

    // Uncompressed point.
    bc::ec_compressed ec_point_1;

    // First byte is prefix 04.
    bc::one_byte prefix = {{0x02}};

    bc::loaf my_arrays =
        {
            {prefix},
            {int_to_bytes(x1)}
        };

    bc::build_array(ec_point_1, my_arrays);
    std::cout << bc::encode_base16(bc::to_chunk(ec_point_1)) << std::endl;
    // std::cout << bc::verify(ec_point_1) << std::endl;

    // try multiplication.
    bc::ec_uncompressed ec_point_1_uncompressed;
    bc::ec_uncompressed ec_point_1_compressed;
    bc::decompress(ec_point_1_compressed, ec_point_1);

    // bc::ec_secret my_factor = int_to_bytes(2);
    // bc::ec_multiply(ec_point_1_compressed, my_factor);
    std::cout << bc::encode_base16(bc::to_chunk(ec_point_1_compressed)) << std::endl;

}

void ec_math_exercise_1c() {

    // Create a uncompressed point from hex.
    const auto point_chunk = bc::base16_literal(
      "0428026f91e1c97db3f6453262484ef5f69f71d89474f10926aae24d3c3eeb5f00c41b6810b8b305a05de2b4448d7e2a079771d4c018b923a9ab860e4b0b4f86f6");

    bc::ec_uncompressed point_uncompressed = point_chunk;
    bc::ec_compressed point_compressed;

    bc::compress(point_compressed, point_chunk);

    std::cout << bc::encode_base16(point_compressed) << std::endl;

}



// Helper function: Convert an int256_t type into a byte_array<32> of same size.
bc::byte_array<bc::ec_secret_size> int_to_bytes(bc::uint256_t my_int)
{
    bc::byte_array<bc::ec_secret_size> my_byte_array;
    for (size_t i = 1; i <= bc::ec_secret_size; i++)
    {
        my_byte_array[bc::ec_secret_size - i] = (uint8_t)(my_int);
        my_int = my_int >> 8;
    }
    return my_byte_array;
}

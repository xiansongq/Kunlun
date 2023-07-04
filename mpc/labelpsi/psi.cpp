#include <cassert>
#include <utility>

#include "seal/seal.h"

#include "hashing.h"

#include "random.h"
#include "windowing.h"

#include "psi.h"

using namespace seal::util;
#ifdef DEBUG_WITH_KEY_LEAK
// this code will only be compiled in debug mode.
#include <iostream>
// we save the receiver's key in a global variable, because it is helpful to
// have access to it when debugging sender code.
SecretKey *receiver_key_leaked;
#endif

void multiply_by_random_mask(Ciphertext &ciphertext,
                             shared_ptr<UniformRandomGenerator> random,
                             std::unique_ptr<seal::BatchEncoder> encoder,
                             std::unique_ptr<seal::Evaluator> evaluator,
                             RelinKeys &relin_keys,
                             uint64_t plain_modulus) {
    size_t slot_count = encoder->slot_count();
    Plaintext mask;
    vector<uint64_t> mask_coefficients;
    for (size_t j = 0; j < slot_count; j++) {
        mask_coefficients.push_back(random_nonzero_integer(random, plain_modulus));
    }
    encoder->encode(mask_coefficients, mask);
    evaluator->multiply_plain_inplace(ciphertext, mask);
    evaluator->relinearize_inplace(ciphertext, relin_keys);
}

/*SEALContext PSIParams::createcontext(size_t poly_modulus_degree) {

    assert((poly_modulus_degree_ == 8192) || (poly_modulus_degree_ == 16384));
    EncryptionParameters parms(scheme_type::bfv);
    parms.set_poly_modulus_degree(poly_modulus_degree_);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree_));
    parms.set_plain_modulus(plain_modulus());

    return SEALContext(parms);
}*/


PSIParams::PSIParams(size_t receiver_size1, size_t sender_size1, size_t input_bits1, size_t poly_modulus_degree)
        : receiver_size(receiver_size1),
          sender_size(sender_size1),
          input_bits(input_bits1),
          poly_modulus_degree_(poly_modulus_degree),
          window_size_(3),
          sender_partition_count_(16) {

    assert((poly_modulus_degree_ == 8192) || (poly_modulus_degree_ == 16384));

    EncryptionParameters parms(scheme_type::bfv);
    parms.set_poly_modulus_degree(poly_modulus_degree_);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree_));
    parms.set_plain_modulus(plain_modulus());

    //context = SEALContext(parms);
    context = make_shared<SEALContext>(parms, true);

}


/*PSIParams::PSIParams(size_t receiver_size1, size_t sender_size1, size_t input_bits1, size_t poly_modulus_degree_)
        : context(createcontext(poly_modulus_degree_)) {
    receiver_size = receiver_size1;
    sender_size = sender_size1;
    input_bits = input_bits1;
    poly_modulus_degree_ = poly_modulus_degree_;
    window_size_ = 3;
    sender_partition_count_ = 16;
    std::cout<<poly_modulus_degree_<<std::endl;
*//*}

   {
            receiver_size=receiver_size1;
            sender_size=sender_size1;
            input_bits=input_bits1;
            poly_modulus_degree_=poly_modulus_degree;
            window_size_=3;
            sender_partition_count_=16;
            context= new SEALContext(createcontext(poly_modulus_degree_));*//*


//    EncryptionParameters parms(scheme_type::bfv);
//    parms.set_poly_modulus_degree(poly_modulus_degree_);
//    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree_));
//    parms.set_plain_modulus(plain_modulus());
//    context = SEALContext(parms);
//context.reset(&context1);

// it must be possible to cuckoo hash the receiver's set into the buckets
    assert(receiver_size<= (1ull <<bucket_count_log()));
// all of the receiver's buckets must fit into one batched ciphertext
    assert((1ull <<bucket_count_log()) <= poly_modulus_degree_);
}*/

void PSIParams::generate_seeds() {
    seeds.clear();
    auto random_factory(UniformRandomGeneratorFactory::DefaultFactory());
    auto random = random_factory->create({});

    for (size_t i = 0; i < hash_functions(); i++) {
        seeds.push_back(random_bits(random, 64));
    }
}

void PSIParams::set_seeds(vector<uint64_t> &seeds_ext) {
    assert(seeds_ext.size() == hash_functions());
    seeds = seeds_ext;
}

uint64_t PSIParams::plain_modulus() {
    // for batching to work, the plain modulus must be a prime that's equal
    // to 1 mod (2 * poly_modulus_degree).
    // it should also be a little over 2^(input_bits - bucket_count_log() + 2)).
    uint64_t min_log_modulus = 0;
    if (input_bits + 2 >= bucket_count_log()) {
        min_log_modulus = input_bits - bucket_count_log() + 2;
    }

    if (min_log_modulus <= 16) {
        return 65537ull; // 2^16 + 1
    } else if (min_log_modulus <= 20) {
        return 1146881ull; // 2^20 + 3 * 2^15 + 1
    } else if (min_log_modulus <= 21) {
        return 2424833ull; // 2^21 + 10 * 2^15 + 1
    } else if (min_log_modulus <= 23) {
        return 8519681ull; // 2^23 + 2^17 + 1
    } else if (min_log_modulus <= 35) {
        return 34359771137ull; // 2^35 + 2^15 + 1
    } else if (min_log_modulus <= 36) {
        return 68720066561ull; // 2^36 + 18 * 2^15 + 1
    } else if (min_log_modulus <= 37) {
        return 137439477761ull; // 2^37 + 16 * 2^15 + 1
    } else {
        assert(0);
    }
}

size_t PSIParams::hash_functions() {
    return 3;
}

size_t PSIParams::bucket_count_log() {
    //std::cout<<poly_modulus_degree_<<std::endl;
    switch (poly_modulus_degree_) {
        case 8192:
            return 13;
        case 16384:
            return 14;
        default:
            assert(0);
    }
    return 0;
}

size_t PSIParams::sender_bucket_capacity() {
    // see Table 1 in [CLR17]
    assert(hash_functions() == 3);

    if (bucket_count_log() == 13) {
        if (sender_size <= (1ull << 8)) {
            return 9;
        } else if (sender_size <= (1ull << 12)) {
            return 20;
        } else if (sender_size <= (1ull << 16)) {
            return 74;
        } else if (sender_size <= (1ull << 20)) {
            return 556;
        } else if (sender_size <= (1ull << 24)) {
            return 6798;
        } else if (sender_size <= (1ull << 28)) {
            return 100890;
        }
    }

    if (bucket_count_log() == 14) {
        if (sender_size <= (1ull << 8)) {
            return 8;
        } else if (sender_size <= (1ull << 12)) {
            return 16;
        } else if (sender_size <= (1ull << 16)) {
            return 51;
        } else if (sender_size <= (1ull << 20)) {
            return 318;
        } else if (sender_size <= (1ull << 24)) {
            return 3543;
        } else if (sender_size <= (1ull << 28)) {
            return 51002;
        }
    }

    assert(0);
    return 0;
}

size_t PSIParams::sender_partition_count() {
    return sender_partition_count_;
}

size_t PSIParams::window_size() {
    return window_size_;
}

void PSIParams::set_sender_partition_count(size_t new_value) {
    sender_partition_count_ = new_value;
}

void PSIParams::set_window_size(size_t new_value) {
    window_size_ = new_value;
}


uint64_t PSIParams::encode_bucket_element(vector<uint64_t> &inputs, bucket_slot &element, bool is_receiver) {
    uint64_t result;
    if (element != BUCKET_EMPTY) {
        // we need to encode:
        // - the input itself, except for the last bucket_count_log() bits
        //   (thanks to permutation-based hashing)
        // - the index of the hash function used to hash it into its bucket.
        //   this should be in [0, 1, 2]; index 3 is used for dummy elements.
        assert(element.second < 3);
        result = (((inputs[element.first] >> bucket_count_log()) << 2)
                  | (element.second));
    } else {
        // for the dummy element, we use a non-existent hash funcion index (3)
        // and 0 or 1 for the input depending on whether it's the sender or the]
        // receiver who needs a dummy.
        result = 3 | ((is_receiver ? 1 : 0) << 2);
    }
    assert(result < plain_modulus());
    return result;
}


PSIReceiver::PSIReceiver(PSIParams &params1) : params(params1) {
    keygen = std::make_unique<KeyGenerator>(std::move(*(params1.context)));
    // keygen = KeyGenerator(params.context);
    keygen->create_public_key(public_key_);
    secret_key = keygen->secret_key();
}

#ifdef DEBUG_WITH_KEY_LEAK
receiver_key_leaked = &secret_key;
#endif


vector<Ciphertext> PSIReceiver::encrypt_inputs(vector<uint64_t> &inputs, vector<bucket_slot> &buckets) {
    assert(inputs.size() == params.receiver_size);

    std::unique_ptr<seal::Encryptor> encryptor = make_unique<Encryptor>(std::move(*(params.context)),
                                                                        std::move(public_key_));
    std::unique_ptr<seal::BatchEncoder> encoder = make_unique<BatchEncoder>(std::move(*(params.context)));

    auto random_factory(UniformRandomGeneratorFactory::DefaultFactory());
    auto random = random_factory->create({});

    size_t bucket_count_log = params.bucket_count_log();
    size_t bucket_count = 1 << bucket_count_log;
    bool res = cuckoo_hash(random, inputs, bucket_count_log, buckets, params.seeds);
    assert(res); // TODO: handle gracefully

    vector<uint64_t> buckets_enc(bucket_count);
    size_t partition_count = params.sender_partition_count();
    size_t max_partition_size = (params.sender_bucket_capacity() + (partition_count - 1)) / partition_count;
    Windowing windowing(params.window_size(), max_partition_size);

    for (size_t i = 0; i < bucket_count; i++) {
        buckets_enc[i] = params.encode_bucket_element(inputs, buckets[i], true);
    }

    vector<Ciphertext> result;
    windowing.prepare(buckets_enc, result, params.plain_modulus(), std::move(encoder), std::move(encryptor));

    return result;
}


vector<size_t> PSIReceiver::decrypt_matches(vector<Ciphertext> &encrypted_matches) {
    std::cout << "encrypted_matches size: " << encrypted_matches.size() << std::endl;
    std::unique_ptr<seal::Decryptor> decryptor = make_unique<Decryptor>(*(params.context), secret_key);
    std::unique_ptr<seal::BatchEncoder> encoder = make_unique<BatchEncoder>(*(params.context));
    size_t slot_count = encoder->slot_count();
    size_t row_size = slot_count / 2;
    size_t bucket_count = (1 << params.bucket_count_log());
    vector<size_t> result;
    Plaintext decrypted;
    for (size_t i = 0; i < encrypted_matches.size(); i++) {
        decryptor->decrypt(encrypted_matches[i], decrypted);
        for (int i = 0; i < bucket_count; i++) {
            if (decrypted[i] == 0)
                std::cout << std::dec << i << " " << decrypted[i] << " ";
        }
        std::vector<uint64_t> temp;
        encoder->decode(decrypted, temp);
        for (int i = 0; i < bucket_count; i++) {
            if (temp[i] == 0)
                std::cout << std::dec << i << " " << temp[i] << " ";
        }
        for (size_t j = 0; j < bucket_count; j++) {
            if (temp[j] == 0) {
                result.push_back(j);
            }
        }
    }
    std::cout << "de result size: " << result.size() << std::endl;
    return result;
}

vector<pair<size_t, uint64_t>> PSIReceiver::decrypt_labeled_matches(vector<Ciphertext> &encrypted_matches) {
    assert(encrypted_matches.size() % 2 == 0);
    std::unique_ptr<seal::Decryptor> decryptor = make_unique<Decryptor>(*(params.context), secret_key);
    std::unique_ptr<seal::BatchEncoder> encoder = make_unique<BatchEncoder>(*(params.context));
    size_t slot_count = encoder->slot_count();

    size_t bucket_count = (1 << params.bucket_count_log());

    vector<pair<size_t, uint64_t>> result;

    Plaintext decrypted_matches, decrypted_labels;
    for (size_t i = 0; i < encrypted_matches.size() / 2; i++) {
        vector<uint64_t> result1;
        vector<uint64_t> lbresult;
        decryptor->decrypt(encrypted_matches[2 * i], decrypted_matches);
        encoder->decode(decrypted_matches, result1);
        decryptor->decrypt(encrypted_matches[2 * i + 1], decrypted_labels);
        encoder->decode(decrypted_labels, lbresult);

        for (size_t j = 0; j < bucket_count; j++) {
            if (result1[j] == 0) {
                result.push_back(pair<size_t, uint64_t>(j, lbresult[j]));
            }
        }
    }

    return result;
}

PublicKey &PSIReceiver::public_key() {
    return public_key_;
}

RelinKeys PSIReceiver::relin_keys() {
    RelinKeys rlk_big;
    keygen->create_relin_keys(rlk_big);
    return rlk_big;
}

PSISender::PSISender(PSIParams &params)
        : params(params) {}

vector<Ciphertext> PSISender::compute_matches(vector<uint64_t> &inputs,
                                              optional<vector<uint64_t>> &labels,
                                              PublicKey &receiver_public_key,
                                              RelinKeys relin_keys,
                                              vector<Ciphertext> &receiver_inputs) {
    assert(inputs.size() == params.sender_size);
    assert(!labels.has_value() || (labels.value().size() == inputs.size()));

    auto random_factory(UniformRandomGeneratorFactory::DefaultFactory());
    auto random = random_factory->create({});

    uint64_t plain_modulus = params.plain_modulus();

    std::unique_ptr<seal::Encryptor> encryptor = make_unique<Encryptor>(*(params.context), receiver_public_key);
    std::unique_ptr<seal::BatchEncoder> encoder = make_unique<BatchEncoder>(*(params.context));
    std::unique_ptr<seal::Evaluator> evaluator = make_unique<Evaluator>(*(params.context));

    // hash all of the sender's inputs, using every possible hash function, into
    // a (capacity × bucket_count) hash table.
    size_t bucket_count_log = params.bucket_count_log();
    size_t bucket_count = (1 << bucket_count_log);
    size_t capacity = params.sender_bucket_capacity();
    vector<bucket_slot> buckets;
    bool res = complete_hash(random, inputs, bucket_count_log, capacity, buckets, params.seeds);
    assert(res); // TODO: handle gracefully

    // we will split the hash table into partitions: instead of looking at a
    // hash table with `capacity` rows, split it into `partition_count` tables
    // with roughly the same number of rows each.
    // specifically, `big_partition_count` subtables will have
    // `max_partition_size` rows, and the rest will have one fewer.
    size_t partition_count = params.sender_partition_count();
    assert(capacity >= partition_count);
    size_t max_partition_size = (capacity + (partition_count - 1)) / partition_count;
    size_t big_partition_count = capacity - (max_partition_size - 1) * partition_count;

    Windowing windowing(params.window_size(), max_partition_size);

    // if we're doing labeled PSI, we need two ciphertexts per partition:
    // one for f(x) and one for r*f(x) + g(x)
    vector<Ciphertext> result((labels.has_value() ? 2 : 1) * partition_count);

    // compute all the powers of the receiver's input.
    vector<Ciphertext> powers(max_partition_size + 1);
    windowing.compute_powers(receiver_inputs, powers, std::move(evaluator), relin_keys);

    // we'll need these vectors for each iteration, so let's declare them here
    // to avoid reallocating them anew each time.
    vector<uint64_t> current_bucket(max_partition_size);
    vector<vector<uint64_t>> f_coeffs(bucket_count);
    // we'll only need these if we're doing labeled PSI, so we set the sizes to
    // 0 if we aren't to avoid unnecessarily wasting memory
    vector<uint64_t> current_labels(labels.has_value() ? max_partition_size : 0);
    vector<vector<uint64_t>> g_coeffs(labels.has_value() ? bucket_count : 0);

    for (size_t partition = 0; partition < partition_count; partition++) {
        // figure out which many rows go into this partition
        size_t partition_size, partition_start;
        if (partition < big_partition_count) {
            partition_size = max_partition_size;
            partition_start = max_partition_size * partition;
        } else {
            partition_size = max_partition_size - 1;
            partition_start = max_partition_size * partition - (partition - big_partition_count);
        }

        // for each bucket, compute the coefficients of the polynomial
        // f(x) = \prod_{y in bucket} (x - y)
        // optionally, also compute coeffs of g(x), which has the property
        // g(y) = label(y) for each y in bucket.
        for (size_t j = 0; j < bucket_count; j++) {
            current_bucket.resize(partition_size);
            for (size_t k = 0; k < partition_size; k++) {
                current_bucket[k] = params.encode_bucket_element(
                        inputs,
                        buckets[j * capacity + partition_start + k],
                        false
                );
            }

            polynomial_from_roots(current_bucket, f_coeffs[j], plain_modulus);
            assert(f_coeffs[j].size() == partition_size + 1);

            if (labels.has_value()) {
                current_labels.resize(partition_size);
                size_t nonempty_slots = 0;
                for (size_t k = 0; k < partition_size; k++) {
                    size_t slot_index = j * capacity + partition_start + k;
                    if (buckets[slot_index] != BUCKET_EMPTY) {
                        current_bucket[nonempty_slots] = current_bucket[k];
                        current_labels[nonempty_slots] = labels.value()[buckets[slot_index].first];
                        nonempty_slots++;
                    }
                }

                current_bucket.resize(nonempty_slots);
                current_labels.resize(nonempty_slots);
                polynomial_from_points(current_bucket, current_labels, g_coeffs[j], plain_modulus);
            }
        }

        // we are done with sender's precomputation. now we can actually
        // evaluate the polynomial on the receiver's input.
        Ciphertext f_evaluated;
        Ciphertext g_evaluated;

#ifdef DEBUG_WITH_KEY_LEAK
        Decryptor decryptor(params.context, *receiver_key_leaked);
        cerr << "processing partition " << partition << endl;
#endif

        for (size_t j = 0; j < partition_size + 1; j++) {
            // encode the jth coefficients of all polynomials into a vector
            Plaintext f_coeffs_enc;
            std::vector<uint64_t> temp(bucket_count);
            for (size_t k = 0; k < bucket_count; k++) {
                temp[k] = f_coeffs[k][j];
            }
            encoder->encode(temp, f_coeffs_enc);

            Plaintext g_coeffs_enc;
            if (labels.has_value()) {
                std::vector<uint64_t> tmp(bucket_count);
                //g_coeffs_enc.resize(bucket_count);
                for (size_t k = 0; k < bucket_count; k++) {
                    tmp[k] = (j < g_coeffs[k].size())
                             ? g_coeffs[k][j]
                             : 0;
                }
                encoder->encode(tmp, g_coeffs_enc);
            }
            std::unique_ptr<seal::Evaluator> evaluator = std::make_unique<Evaluator>(*(params.context));
            if (j == 0) {
                // the constant term just goes straight into the result, and
                // then the other terms will be added into it later.
                encryptor->encrypt(f_coeffs_enc, f_evaluated);
                if (labels.has_value()) {
                    encryptor->encrypt(g_coeffs_enc, g_evaluated);
                }
            } else {
                // term = receiver_inputs^j * f_coeffs_enc
                // multiply_plain does not allow the second parameter to be zero.
                if (!f_coeffs_enc.is_zero()) {
                    Ciphertext term;
                    //std::cout << powers[j].is_transparent() << std::endl;
                    evaluator->multiply_plain(powers[j], f_coeffs_enc, term);
                    evaluator->relinearize_inplace(term, relin_keys);
                    evaluator->add_inplace(f_evaluated, term);
                }

                if (!g_coeffs_enc.is_zero()) {
                    Ciphertext term;
                    evaluator->multiply_plain(powers[j], g_coeffs_enc, term);
                    evaluator->relinearize_inplace(term, relin_keys);
                    evaluator->add_inplace(g_evaluated, term);
                }
            }

#ifdef DEBUG_WITH_KEY_LEAK
            cerr << "after term " << j << " n.b. is " << decryptor.invariant_noise_budget(f_evaluated) << endl;
#endif
        }

        // for unlabeled PSI, return r * f(x)
        // for labeled PSI, return (r * f(x), r' * f(x) + g(x))
        // where r and r' are random.
        std::unique_ptr<seal::BatchEncoder> encoder = std::make_unique<BatchEncoder>(*(params.context));
        std::unique_ptr<seal::Evaluator> evaluator = std::make_unique<Evaluator>(*(params.context));
        multiply_by_random_mask(f_evaluated, random, std::move(encoder), std::move(evaluator), relin_keys,
                                plain_modulus);

#ifdef DEBUG_WITH_KEY_LEAK
        cerr << "after mask it is " << decryptor.invariant_noise_budget(f_evaluated) << endl;
#endif

        if (labels.has_value()) {
            result[2 * partition] = f_evaluated;
            std::unique_ptr<seal::BatchEncoder> encoder = make_unique<BatchEncoder>(*(params.context));
            std::unique_ptr<seal::Evaluator> evaluator = make_unique<Evaluator>(*(params.context));
            multiply_by_random_mask(f_evaluated, random, std::move(encoder), std::move(evaluator), relin_keys,
                                    plain_modulus);

#ifdef DEBUG_WITH_KEY_LEAK
            cerr << "after second mask it is " << decryptor.invariant_noise_budget(f_evaluated) << endl;
#endif
            std::unique_ptr<seal::Evaluator> evaluator1 = std::make_unique<Evaluator>(*(params.context));
            evaluator1->add(f_evaluated, g_evaluated, result[2 * partition + 1]);

#ifdef DEBUG_WITH_KEY_LEAK
            cerr << "after final add it is " << decryptor.invariant_noise_budget(result[2 * partition + 1]) << endl;
#endif
        } else {
            result[partition] = f_evaluated;
        }
    }
    std::cout << "result size: " << result.size() << std::endl;
    return result;
}

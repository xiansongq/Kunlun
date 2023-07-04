#include <cassert>

//#include "aes.h"
#include "../../crypto/aes.hpp"
#include "hashing.h"

using namespace std;

uint64_t aes_hash(AES::Key &key, size_t bits, uint64_t value) {
    block a=Block::MakeBlock('0LL',value);
	assert(bits < 64);

	 AES::Enc(key, a);
	return (Block::BlockToUint64Low(a) ^ value) & ((1ull << bits) - 1);
}

size_t loc_aes_hash(AES::Key &key, size_t m, uint64_t value) {
	return aes_hash(key, m, value >> m) ^ (value & ((1ull << m) - 1));
}

bool cuckoo_hash(shared_ptr<UniformRandomGenerator> random,
	             vector<uint64_t> &inputs,
	             size_t m,
	             vector<bucket_slot> &buckets,
	             vector<uint64_t> &seeds)
{
	buckets.resize(1 << m);
	for (size_t i = 0; i < buckets.size(); i++) {
		buckets[i] = BUCKET_EMPTY;
	}

	vector<AES::Key> aes(seeds.size());
	for (size_t i = 0; i < seeds.size(); i++) {
		aes[i]=AES::GenEncKey(Block::MakeBlock('0LL',seeds[i]));
	}

	for (size_t i = 0; i < inputs.size(); i++) {
		bool resolved = false;
		bucket_slot current_item = make_pair(
			i,
			random_integer(random, seeds.size())
		);

		// TODO: keep track of # of operations and abort if exceeding some limit
		while (!resolved) {
			size_t loc = loc_aes_hash(
				aes[current_item.second],
				m,
				inputs[current_item.first]
			);

			buckets[loc].swap(current_item);

			if (current_item == BUCKET_EMPTY) {
				resolved = true;
			} else {
				size_t old_hash = current_item.second;
				while (current_item.second == old_hash) {
					current_item.second = random_integer(random, seeds.size());
				}
			}
		}
	}

	return true;
}

bool complete_hash(shared_ptr<UniformRandomGenerator> random,
	               vector<uint64_t> &inputs,
                   size_t m,
                   size_t capacity,
                   vector<bucket_slot> &buckets,
                   vector<uint64_t> &seeds)
{
	buckets.resize(capacity << m);
	for (size_t i = 0; i < buckets.size(); i++) {
		buckets[i] = BUCKET_EMPTY;
	}

	vector<AES::Key> aes(seeds.size());
	for (size_t i = 0; i < seeds.size(); i++) {
        aes[i]=AES::GenEncKey(Block::MakeBlock('0LL',seeds[i]));
	}

	vector<size_t> capacity_used(1 << m);

	// insert all elements into the table in a deterministic order (filling each
	// bucket sequentially)
	for (size_t i = 0; i < inputs.size(); i++) {
		for (size_t j = 0; j < seeds.size(); j++) {
			size_t loc = loc_aes_hash(aes[j], m, inputs[i]);

			if (capacity_used[loc] == capacity) {
				// all slots in the bucket are used, so we cannot add this
				// element
				return false;
			}

			buckets[capacity * loc + capacity_used[loc]] = make_pair(i, j);
			capacity_used[loc]++;
		}
	}

	// now shuffle each bucket, to avoid leaking information about bucket load
	// distribution through partitioning
	for (size_t bucket = 0; bucket < (1 << m); bucket++) {
		for (size_t slot = 1; slot < capacity; slot++) {
			// uniformly pick a random slot before this one (possibly this
			// very same one) and swap
			size_t prev_slot = random_integer(random, slot + 1);
			buckets[capacity * bucket + slot].swap(buckets[capacity * bucket + prev_slot]);
		}
	}

	return true;
}


//#include <cassert>
//
//#include "aes.h"
//
//#include "hashing.h"
//
//using namespace std;
//
//uint64_t aes_hash(AES &aes, size_t bits, uint64_t value) {
//    assert(bits < 64);
//    auto ciphertext = aes.encrypt(0, value);
//    return (ciphertext.second ^ value) & ((1ull << bits) - 1);
//}
//
//size_t loc_aes_hash(AES &aes, size_t m, uint64_t value) {
//    return aes_hash(aes, m, value >> m) ^ (value & ((1ull << m) - 1));
//}
//
//bool cuckoo_hash(shared_ptr<UniformRandomGenerator> random,
//                 vector<uint64_t> &inputs,
//                 size_t m,
//                 vector<bucket_slot> &buckets,
//                 vector<uint64_t> &seeds)
//{
//    buckets.resize(1 << m);
//    for (size_t i = 0; i < buckets.size(); i++) {
//        buckets[i] = BUCKET_EMPTY;
//    }
//
//    vector<AES> aes(seeds.size());
//    for (size_t i = 0; i < seeds.size(); i++) {
//        aes[i].set_key(0, seeds[i]);
//    }
//
//    for (size_t i = 0; i < inputs.size(); i++) {
//        bool resolved = false;
//        bucket_slot current_item = make_pair(
//                i,
//                random_integer(random, seeds.size())
//        );
//
//        // TODO: keep track of # of operations and abort if exceeding some limit
//        while (!resolved) {
//            size_t loc = loc_aes_hash(
//                    aes[current_item.second],
//                    m,
//                    inputs[current_item.first]
//            );
//
//            buckets[loc].swap(current_item);
//
//            if (current_item == BUCKET_EMPTY) {
//                resolved = true;
//            } else {
//                size_t old_hash = current_item.second;
//                while (current_item.second == old_hash) {
//                    current_item.second = random_integer(random, seeds.size());
//                }
//            }
//        }
//    }
//
//    return true;
//}
//
//bool complete_hash(shared_ptr<UniformRandomGenerator> random,
//                   vector<uint64_t> &inputs,
//                   size_t m,
//                   size_t capacity,
//                   vector<bucket_slot> &buckets,
//                   vector<uint64_t> &seeds)
//{
//    buckets.resize(capacity << m);
//    for (size_t i = 0; i < buckets.size(); i++) {
//        buckets[i] = BUCKET_EMPTY;
//    }
//
//    vector<AES> aes(seeds.size());
//    for (size_t i = 0; i < seeds.size(); i++) {
//        aes[i].set_key(0, seeds[i]);
//    }
//
//    vector<size_t> capacity_used(1 << m);
//
//    // insert all elements into the table in a deterministic order (filling each
//    // bucket sequentially)
//    for (size_t i = 0; i < inputs.size(); i++) {
//        for (size_t j = 0; j < seeds.size(); j++) {
//            size_t loc = loc_aes_hash(aes[j], m, inputs[i]);
//
//            if (capacity_used[loc] == capacity) {
//                // all slots in the bucket are used, so we cannot add this
//                // element
//                return false;
//            }
//
//            buckets[capacity * loc + capacity_used[loc]] = make_pair(i, j);
//            capacity_used[loc]++;
//        }
//    }
//
//    // now shuffle each bucket, to avoid leaking information about bucket load
//    // distribution through partitioning
//    for (size_t bucket = 0; bucket < (1 << m); bucket++) {
//        for (size_t slot = 1; slot < capacity; slot++) {
//            // uniformly pick a random slot before this one (possibly this
//            // very same one) and swap
//            size_t prev_slot = random_integer(random, slot + 1);
//            buckets[capacity * bucket + slot].swap(buckets[capacity * bucket + prev_slot]);
//        }
//    }
//
//    return true;
//}

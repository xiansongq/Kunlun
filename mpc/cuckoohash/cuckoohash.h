//
// Created by 17579 on 2023/5/9.
//

#ifndef KUNLUN_CUCKOOHASH_H
#define KUNLUN_CUCKOOHASH_H

#include "../../include/std.inc"
#include "internal/blake2.h"
#include "internal/blake2-impl.h"



namespace CUCKOOHASH {
    /**
    The type that represents a 128-bit item that can be added to the hash table.
    */
    using item_type = std::array<unsigned char, 16>;

    /**
    The type that represents a location in the hash table.
    */
    using location_type = std::uint32_t;

    /**
    The type that represents the size of a hash table.
    */
    using table_size_type = location_type;

    /**
    The smallest allowed table size.
    */
    constexpr table_size_type min_table_size = table_size_type(1);

    /**
    The largest allowed table size.
    */
    constexpr table_size_type max_table_size = table_size_type(1) << 30;

    /**
    The smallest allowed number of hash functions.
    */
    constexpr std::uint32_t min_loc_func_count = 1;

    /**
    The largest allowed number of hash functions. This must be a power of two for correct behavior.
    */
    constexpr std::uint32_t max_loc_func_count = 32;

    constexpr int bytes_per_uint64 = sizeof(std::uint64_t);

    constexpr int bytes_per_item = sizeof(item_type);

    /**
    Generates a random 64-bit unsigned integer.
    */
    inline std::uint64_t random_uint64() {
        std::random_device rd;
        return (static_cast<std::uint64_t>(rd()) << 32) | static_cast<std::uint64_t>(rd());
    }

    /**
    Return a reference to the low-word of the item.
    */
    inline std::uint64_t &get_low_word(item_type &item) {
        return *reinterpret_cast<std::uint64_t *>(item.data());
    }

    /**
    Return a reference to the high-word of the item.
    */
    inline std::uint64_t &get_high_word(item_type &item) {
        return *reinterpret_cast<std::uint64_t *>(item.data() + 8);
    }

    /**
    Return a reference to the low-word of the item.
    */
    inline std::uint64_t get_low_word(const item_type &item) {
        return *reinterpret_cast<const std::uint64_t *>(item.data());
    }

    /**
    Return a reference to the high-word of the item.
    */
    inline std::uint64_t get_high_word(const item_type &item) {
        return *reinterpret_cast<const std::uint64_t *>(item.data() + 8);
    }

    /**
    Sets the value of a given hash table item from a given buffer.

    @param[in] in The buffer to set the value from
    @param[out] destination The hash table item whose value is to be set
    */
    inline void set_item(const unsigned char *in, item_type &destination) noexcept {
        std::copy_n(in, bytes_per_item, destination.data());
    }

    /**
    Creates a new hash table item and sets its value from a given buffer.

    @param[in] in The buffer to set the value from
    */
    inline item_type make_item(const unsigned char *in) noexcept {
        item_type out;
        set_item(in, out);
        return out;
    }

    /**
    Sets the value of a given hash table item from a pair of two 64-bit words.

    @param[in] low_word The low 64 bits of the value of the item
    @param[in] high_word The high 64 bits of the value of the item
    @param[out] destination The hash table item whose value is to be set
    */
    inline void set_item(std::uint64_t low_word, std::uint64_t high_word, item_type &destination) noexcept {
        get_low_word(destination) = low_word;
        get_high_word(destination) = high_word;
    }

    /**
    Creates a hash table item and sets its value from a pair of two 64-bit words.

    @param[in] low_word The lowest 64 bits of the value of the item
    @param[in] high_word The highest 64 bits of the value of the item
    */
    inline item_type make_item(std::uint64_t low_word, std::uint64_t high_word) noexcept {
        item_type item;
        set_item(low_word, high_word, item);
        return item;
    }

    /**
    Creates a zero hash table item.
    */
    inline item_type make_zero_item() noexcept {
        return item_type{};
    }

    /**
    Sets a given hash table item to zero.

    @param[out] destination The hash table item whose value is to be set
    */
    inline void set_zero_item(item_type &destination) noexcept {
        destination = item_type{};
    }

    /**
    Sets the value of a given hash table item to all one-bits.

    @param[out] destination The hash table item whose value is to be set
    */
    inline void set_all_ones_item(item_type &destination) noexcept {
        get_low_word(destination) = ~std::uint64_t(0);
        get_high_word(destination) = ~std::uint64_t(0);
    }

    /**
    Creates a hash table item and sets its value to all one-bits.
    */
    inline item_type make_all_ones_item() noexcept {
        item_type item;
        set_all_ones_item(item);
        return item;
    }

    /**
    Returns whether a given hash table item is zero.

    @param[in] in The hash table item to test
    */
    inline bool is_zero_item(const item_type &in) noexcept {
        return !(get_low_word(in) | get_high_word(in));
    }

    /**
    Returns whether a given has table item has all one-bits.

    @param[in] in The hash table item to test
    */
    inline bool is_all_ones_item(const item_type &in) noexcept {
        return !(~get_low_word(in) | ~get_high_word(in));
    }

    /**
    Returns whether two hash table items are equal.

    @param[in] in1 The first hash table item
    @param[in] in2 The second hash table item
    */
    inline bool are_equal_item(const item_type &in1, const item_type &in2) noexcept {
        return (get_low_word(in1) == get_low_word(in2)) && (get_high_word(in1) == get_high_word(in2));
    }

    /**
    Sets a given hash table item to a random value.

    @param[out] destination The hash table item whose value is to be set
    */
    inline void set_random_item(item_type &destination) noexcept {
        set_item(random_uint64(), random_uint64(), destination);
    }

    /**
    Creates a random hash table item.
    */
    inline item_type make_random_item() noexcept {
        item_type out;
        set_random_item(out);
        return out;
    }

    /**
    Interprets a hash table item as a 128-bit integer and increments its value by one.

    @param[in,out] in The hash table item whose value is to be incremented
    */
    inline void increment_item(item_type &in) noexcept {
        get_low_word(in) += 1;
        get_high_word(in) += !get_low_word(in) ? 1 : 0;
    }


    class HashFunc {
    public:
        HashFunc(item_type seed) {
            if (blake2xb(
                    random_array_.data(),
                    random_array_size_ * sizeof(location_type),
                    seed.data(),
                    sizeof(seed),
                    nullptr, 0) != 0) {
                throw std::runtime_error("blake2xb failed");
            }
        }

        inline location_type operator()(item_type item) const noexcept {
            return
                    random_array_[0 * block_value_count_ + static_cast<std::size_t>(item[0])] ^
                    random_array_[1 * block_value_count_ + static_cast<std::size_t>(item[1])] ^
                    random_array_[2 * block_value_count_ + static_cast<std::size_t>(item[2])] ^
                    random_array_[3 * block_value_count_ + static_cast<std::size_t>(item[3])] ^
                    random_array_[4 * block_value_count_ + static_cast<std::size_t>(item[4])] ^
                    random_array_[5 * block_value_count_ + static_cast<std::size_t>(item[5])] ^
                    random_array_[6 * block_value_count_ + static_cast<std::size_t>(item[6])] ^
                    random_array_[7 * block_value_count_ + static_cast<std::size_t>(item[7])] ^
                    random_array_[8 * block_value_count_ + static_cast<std::size_t>(item[8])] ^
                    random_array_[9 * block_value_count_ + static_cast<std::size_t>(item[9])] ^
                    random_array_[10 * block_value_count_ + static_cast<std::size_t>(item[10])] ^
                    random_array_[11 * block_value_count_ + static_cast<std::size_t>(item[11])] ^
                    random_array_[12 * block_value_count_ + static_cast<std::size_t>(item[12])] ^
                    random_array_[13 * block_value_count_ + static_cast<std::size_t>(item[13])] ^
                    random_array_[14 * block_value_count_ + static_cast<std::size_t>(item[14])] ^
                    random_array_[15 * block_value_count_ + static_cast<std::size_t>(item[15])];
        }

    private:
        static constexpr std::size_t block_size_ = 1;

        static constexpr std::size_t block_count_ = sizeof(item_type);

        static constexpr std::size_t block_value_count_ = (std::size_t(1) << (8 * block_size_));

        static constexpr std::size_t random_array_size_ = block_value_count_ * block_count_;

        static constexpr std::uint32_t block_mask_ =
                static_cast<std::uint32_t>(block_value_count_ - 1);

        std::array<location_type, random_array_size_> random_array_;
    };  //class HashFunc
    /**
    An instance of the LocFunc class represents a location function (hash function) used by the HashTable class to
    insert an item in the hash table. The location functions are automatically created by the HashTable class instance
    from a seed.
    */
    class LocFunc {
    public:
        /**
        Creates a new location function for a table of given size. The location function uses the given seed for
        randomness.

        @param[in] table_size The size of the hash table that this location function is for
        @param[in] seed The seed for randomness
        @throws std::invalid_argument if the table_size is larger or smaller than allowed
        */
        LocFunc(table_size_type table_size, item_type seed) : table_size_(table_size), hf_(seed) {
            if (table_size < min_table_size || table_size > max_table_size) {
                throw std::invalid_argument("table_size is out of range");
            }
        }

        /**
        Creates a copy of a given location function.

        @param[in] copy The location function to copy from
        */
        LocFunc(const LocFunc &copy) = default;

        /**
        Assigns this location function to be equal to a given location function.

        @param[in] assign The location function to assign from
        */
        LocFunc &operator=(const LocFunc &assign) = delete;

        /**
        Returns the location for a given item.

        @param[in] item The hash table item for which to compute the location
        */
        inline location_type operator()(item_type item) const noexcept {
            return hf_(item) % table_size_;
        }

    private:
        table_size_type table_size_;

        HashFunc hf_;
    };  //class LocFunc

    /**
    The QueryResult class represents the result of a hash table query. It includes information about whether a queried
    item was found in the hash table, its location in the hash table or stash (if found), and the index of the location
    function (hash function) that was used to insert it. QueryResult objects are returned by the HashTable::query
    function.
    */
    class QueryResult {
        friend class HashTable;

    public:
        /**
        Creates a QueryResult object.
        */
        QueryResult() = default;

        /**
        Returns the hash table or stash location represented by this QueryResult.
        */
        inline location_type location() const noexcept {
            return location_;
        }

        /**
        Returns the index of the location function that was used to insert the queried item. This value is meaningless
        when in_stash() is true. A value equal to max_loc_func_count indicates the item was not found in the table or
        stash.
        */
        inline std::uint32_t loc_func_index() const noexcept {
            return loc_func_index_;
        }

        /**
        Returns whether the queried item was found in the stash.
        */
        inline bool in_stash() const noexcept {
            return !~loc_func_index_;
        }

        /**
        Returns whether the queried item was found in the hash table or in the stash.
        */
        inline bool found() const noexcept {
            return !(loc_func_index_ & ~(max_loc_func_count - 1)) || in_stash();
        }

        /**
        Returns whether the queried item was found in the hash table or in the stash.
        */
        inline operator bool() const noexcept {
            return found();
        }

    private:
        QueryResult(location_type location, std::uint32_t loc_func_index)
                : location_(location), loc_func_index_(loc_func_index) {
#ifdef KUKU_DEBUG
            if (location >= max_table_size)
            {
                throw std::invalid_argument("invalid location");
            }
#endif
        }

        location_type location_ = 0;

        std::uint32_t loc_func_index_ = 0;
    };  // class QueryResult


    class QueryResult;

    /**
    The HashTable class represents a cockoo hash table. It includes information about the location functions (hash
    functions) and holds the items inserted into the table.
    */
    class HashTable {

    public:
        /**
        Creates a new empty hash table.

        @param[in] table_size The size of the hash table
        @param[in] stash_size The size of the stash (possibly zero)
        @param[in] loc_func_count The number of location functions (hash functions) to use
        @param[in] loc_func_seed The 128-bit seed for the location functions, represented as a hash table item
        @param[in] max_probe The maximum number of random walk steps taken in attempting to insert an item
        @param[in] empty_item A hash table item that represents an empty location in the table
        @throws std::invalid_argument if loc_func_count is too large or too small
        @throws std::invalid_argument if table_size is too large or too small
        @throws std::invalid_argument if max_probe is zero
        */
        HashTable(
                table_size_type table_size, table_size_type stash_size, uint32_t loc_func_count,
                item_type loc_func_seed,
                uint64_t max_probe, item_type empty_item)
                : table_size_(table_size), stash_size_(stash_size), loc_func_seed_(loc_func_seed),
                  max_probe_(max_probe),
                  empty_item_(empty_item), leftover_item_(empty_item_), inserted_items_(0), gen_(random_uint64()) {
            if (loc_func_count < min_loc_func_count || loc_func_count > max_loc_func_count) {
                throw std::invalid_argument("loc_func_count is out of range");
            }
            if (table_size < min_table_size || table_size > max_table_size) {
                throw std::invalid_argument("table_size is out of range");
            }
            if (!max_probe) {
                throw std::invalid_argument("max_probe cannot be zero");
            }

            // Allocate the hash table
            table_.resize(table_size_, empty_item_);

            // Create the location (hash) functions
            generate_loc_funcs(loc_func_count, loc_func_seed_);

            // Set up the distribution for location function sampling
            u_ = std::uniform_int_distribution<uint32_t>(0, loc_func_count - 1);
        }
//        HashTable(
//                table_size_type table_size, table_size_type stash_size, std::uint32_t loc_func_count,
//                item_type loc_func_seed, std::uint64_t max_probe, item_type empty_item);
        /**
         Queries for the presence of a given item in the hash table and stash.

         @param[in] item The hash table item to query
         @throws std::invalid_argument if the given item is the empty item for this hash table
         */
        QueryResult query(item_type item) const {
            if (is_empty_item(item)) {
                throw std::invalid_argument("item cannot be the empty item");
            }

            // Search the hash table
            for (uint32_t i = 0; i < loc_func_count(); i++) {
                auto loc = location(item, i);
                if (are_equal_item(table_[loc], item)) {
                    return {loc, i};
                }
            }

            // Search the stash
            for (location_type loc = 0; loc < stash_.size(); loc++) {
                if (are_equal_item(stash_[loc], item)) {
                    return {loc, ~uint32_t(0)};
                }
            }

            // Not found
            return {0, max_loc_func_count};
        }

        /**
        Adds a single item to the hash table using random walk cuckoo hashing. The return value indicates whether
        the item was successfully inserted (possibly into the stash) or not.

        @param[in] item The hash table item to insert
        @throws std::invalid_argument if the given item is the empty item for this hash table
        */
        bool insert(item_type item) {
            // Check if the item is already inserted or is the empty item
            if (query(item)) {
                return false;
            }

            uint64_t level = max_probe_;
            while (level--) {
                // Loop over all possible locations
                for (uint32_t i = 0; i < loc_func_count(); i++) {
                    location_type loc = location(item, i);
                    if (is_empty_item(table_[loc])) {
                        table_[loc] = item;
                        inserted_items_++;
                        return true;
                    }
                }

                // Swap in the current item and in next round try the popped out item
                item = swap(item, location(item, static_cast<uint32_t>(u_(gen_))));
            }

            // level reached zero; try stash
            if (stash_.size() < stash_size_) {
                stash_.push_back(item);
                inserted_items_++;
                return true;
            } else {
                leftover_item_ = item;
                return false;
            }
        }


        /**
        Returns a location that a given hash table item may be placed at.

        @param[in] item The hash table item for which the location is to be obtained
        @param[in] loc_func_index The index of the location function which to use to compute the location
        @throws std::out_of_range if loc_func_index is out of range
        @throws std::invalid_argument if the given item is the empty item for this hash table
        */
        inline location_type location(item_type item, std::uint32_t loc_func_index) const {
            if (loc_func_index >= loc_funcs_.size()) {
                throw std::out_of_range("loc_func_index is out of range");
            }
            if (is_empty_item(item)) {
                throw std::invalid_argument("item cannot be the empty item");
            }
            return loc_funcs_[loc_func_index](item);
        }

        /**
        Returns all hash table locations that this item may be placed at.

        @throws std::invalid_argument if the given item is the empty item for this hash table
        */
        std::set<location_type> all_locations(item_type item) const {
            if (is_empty_item(item)) {
                throw std::invalid_argument("item cannot be the empty item");
            }

            std::set<location_type> result;
            for (auto lf: loc_funcs_) {
                result.emplace(lf(item));
            }
            return result;
        }

        /**
        Clears the hash table by filling every location with the empty item.
        */
        void clear_table() {
            size_t sz = table_.size();
            table_.resize(0);
            table_.resize(sz, empty_item_);
            stash_.clear();
            leftover_item_ = empty_item_;
            inserted_items_ = 0;
        }

        /**
        Returns the number of location functions used by the hash table.
        */
        inline std::uint32_t loc_func_count() const noexcept {
            return static_cast<std::uint32_t>(loc_funcs_.size());
        }

        /**
        Returns a reference to the hash table.
        */
        inline const std::vector<item_type> &table() const noexcept {
            return table_;
        }

        /**
        Returns a reference to a specific location in the hash table.

        @param[in] index The index in the hash table
        @throws std::out_of_range if index is out of range
        */
        inline const item_type &table(location_type index) const {
            if (index >= table_size_) {
                throw std::out_of_range("index is out of range");
            }
            return table_[index];
        }

        /**
        Returns a reference to the stash.
        */
        inline const std::vector<item_type> &stash() const noexcept {
            return stash_;
        }

        /**
        Returns a reference to a specific location in the stash.

        @param[in] index The index in the stash
        @throws std::out_of_range if index is out of range
        */
        inline const item_type &stash(location_type index) const {
            if (index >= stash_size_) {
                throw std::out_of_range("index is out of range");
            }
            if (index >= stash_.size() && index < stash_size_) {
                return empty_item_;
            }
            return stash_[index];
        }

        /**
        Returns the size of the hash table.
        */
        inline table_size_type table_size() const noexcept {
            return table_size_;
        }

        /**
        Returns the size of the stash.
        */
        inline table_size_type stash_size() const noexcept {
            return stash_size_;
        }

        /**
        Returns the 128-bit seed used for the location functions, represented as a hash table item.
        */
        inline item_type loc_func_seed() const noexcept {
            return loc_func_seed_;
        }

        /**
        Returns the maximum number of random walk steps taken in attempting to insert an item.
        */
        inline std::uint64_t max_probe() const noexcept {
            return max_probe_;
        }

        /**
        Returns the hash table item that represents an empty location in the table.
        */
        inline const item_type &empty_item() const noexcept {
            return empty_item_;
        }

        /**
        Returns whether a given location in the table is empty.

        @param[in] index The index in the hash table
        @throws std::out_of_range if index is out of range
        */
        inline bool is_empty(location_type index) const noexcept {
            return is_empty_item(table(index));
        }

        /**
        Returns whether a given item is the empty item for this hash table.

        @param[in] item The item to compare to the empty item
        */
        inline bool is_empty_item(const item_type &item) const noexcept {
            return are_equal_item(item, empty_item_);
        }

        /**
        When the insert function fails to insert a hash table item, there is a leftover item that could not be inserted
        into the table. This function will return the empty item if insertion never failed, and otherwise it will return
        the latest leftover item. Note that due to how the random walk insertion process works, the leftover item is
        usually not the same one that insert was called with.
        */
        inline item_type leftover_item() const noexcept {
            return leftover_item_;
        }

        /**
        Returns the current fill rate of the hash table and stash.
        */
        inline double fill_rate() const noexcept {
            return static_cast<double>(inserted_items_) /
                   (static_cast<double>(table_size()) + static_cast<double>(stash_size_));
        }

    private:
        HashTable(const HashTable &copy) = delete;

        HashTable &operator=(const HashTable &assign) = delete;

        void generate_loc_funcs(uint32_t loc_func_count, item_type seed) {
            loc_funcs_.clear();
            while (loc_func_count--) {
                loc_funcs_.emplace_back(table_size_, seed);
                increment_item(seed);
            }
        }

        /*
        Swap an item in the table with a given item.
        */
        inline item_type swap(item_type item, location_type location) noexcept {
            item_type old_item = table_[location];
            table_[location] = item;
            return old_item;
        }

        /*
        The hash table that holds all of the input data.
        */
        std::vector<item_type> table_;

        /*
        The stash.
        */
        std::vector<item_type> stash_;

        /*
        The hash functions.
        */
        std::vector<LocFunc> loc_funcs_;

        /*
        The size of the table.
        */
        const table_size_type table_size_;

        /*
        The size of the stash.
        */
        const table_size_type stash_size_;

        /*
        Seed for the hash functions
        */
        const item_type loc_func_seed_;

        /*
        The maximum number of attempts that are made to insert an item.
        */
        const std::uint64_t max_probe_;

        /*
        An item value that denotes an empty item.
        */
        const item_type empty_item_;

        /*
        Storage for an item that was evicted and could not be re-inserted. This
        is populated when insert fails.
        */
        item_type leftover_item_;

        /*
        The number of items that have been inserted to table or stash.
        */
        table_size_type inserted_items_;

        /*
        Randomness source for location function sampling.
        */
        std::mt19937_64 gen_;

        std::uniform_int_distribution<std::uint32_t> u_;
    };  // class HashTable



} // namespace CUCKOOHASH

#endif //KUNLUN_CUCKOOHASH_H

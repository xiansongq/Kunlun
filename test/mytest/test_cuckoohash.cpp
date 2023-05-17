//
// Created by 17579 on 2023/5/9.
//


#include "../../mpc/cuckoohash/cuckoohash.h"

void print_table(const CUCKOOHASH::HashTable &table)
{
    CUCKOOHASH::table_size_type col_count = 8;
    for (CUCKOOHASH::table_size_type i = 0; i < table.table_size(); i++)
    {
        const auto &item = table.table(i);
        std::cout << std::setw(5)
                  << i << ": " << std::setw(5) << CUCKOOHASH::get_high_word(item) << "," << CUCKOOHASH::get_low_word(item)
                  << ((i % col_count == col_count - 1) ? "\n" : "\t");
    }

    std::cout << std::endl << std::endl << "Stash: " << std::endl;
    for (CUCKOOHASH::table_size_type i = 0; i < table.stash().size(); i++)
    {
        const auto &item = table.stash(i);
        std::cout << i << ": " << CUCKOOHASH::get_high_word(item) << "," << CUCKOOHASH::get_low_word(item) << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]){
    if (argc != 5)
    {
        std::cout << "Usage: ./example table_size stash_size loc_func_count max_probe" << std::endl;
        std::cout << "E.g., ./example 256 2 4 100" << std::endl;

        return 0;
    }
    CUCKOOHASH::table_size_type table_size = static_cast<CUCKOOHASH::table_size_type>(atoi(argv[1]));
    CUCKOOHASH::table_size_type stash_size = static_cast<CUCKOOHASH::table_size_type>(atoi(argv[2]));
    uint32_t loc_func_count = static_cast<uint8_t>(atoi(argv[3]));
    CUCKOOHASH::item_type loc_func_seed = CUCKOOHASH::make_random_item();
    uint64_t max_probe = static_cast<uint64_t>(atoi(argv[4]));
    CUCKOOHASH::item_type empty_item = CUCKOOHASH::make_item(0, 0);
    //CUCKOOHASH::item_type empty_item = CUCKOOHASH::make_item(0, 0);
    CUCKOOHASH::HashTable table(table_size, stash_size, loc_func_count, loc_func_seed, max_probe, empty_item);
    uint64_t round_counter = 0;
//    while(true){
//        char c[2];
//        std::cin.getline(c,2);
//        if(c[0]=='0') break;
//        for(uint64_t i=round_counter+20;i<round_counter+40;i++){
//            if (!table.insert(CUCKOOHASH::make_item(i, i + 1)))
//            {
//                std::cout << "Insertion failed: round_counter = " << round_counter << ", i = " << i << std::endl;
//                std::cout << "Inserted successfully " << round_counter * 20 + i << " items" << std::endl;
//                std::cout << "Fill rate: " << table.fill_rate() << std::endl;
//                const auto &item = table.leftover_item();
//                std::cout << "Leftover item: " << CUCKOOHASH::get_high_word(item) << "," << CUCKOOHASH::get_low_word(item) << std::endl << std::endl;
//                break;
//            }
//
//            if (!table.is_empty_item(table.leftover_item()))
//            {
//                break;
//            }
//
//        }
//        std::cout << "Inserted " << round_counter*20 << " items" << std::endl;
//        std::cout << "Fill rate: " << table.fill_rate() << std::endl;
//        round_counter++;
//        print_table(table);
//
//    }
    while (true)
    {
        std::cout << "Inserted " << round_counter * 20 << " items" << std::endl;
        std::cout << "Fill rate: " << table.fill_rate() << std::endl;

        char c;
        std::cin.get(c);

        for (uint64_t i = 0; i < 20; i++)
        {
            if (!table.insert(CUCKOOHASH::make_item(i + 1, round_counter + 1)))
            {
                std::cout << "Insertion failed: round_counter = " << round_counter << ", i = " << i << std::endl;
                std::cout << "Inserted successfully " << round_counter * 20 + i << " items" << std::endl;
                std::cout << "Fill rate: " << table.fill_rate() << std::endl;
                const auto &item = table.leftover_item();
                std::cout << "Leftover item: " << CUCKOOHASH::get_high_word(item) << "," << CUCKOOHASH::get_low_word(item) << std::endl << std::endl;
                break;
            }
        }

        print_table(table);

        if (!table.is_empty_item(table.leftover_item()))
        {
            break;
        }

        round_counter++;
    }

    while (true)
    {
        std::cout << "Query item: ";
        char hw[64];
        char lw[64];
        std::cin.getline(hw, 10, ',');
        std::cin.getline(lw, 10, '\n');
        CUCKOOHASH::item_type item = CUCKOOHASH::make_item(static_cast<uint64_t>(atoi(lw)), static_cast<uint64_t>(atoi(hw)));
        CUCKOOHASH::QueryResult res = table.query(item);
        std::cout << "Found: " << std::boolalpha << !!res << std::endl;
        if (res)
        {
            std::cout << "Location: " << res.location() << std::endl;
            std::cout << "In stash: " << std::boolalpha << res.in_stash() << std::endl;
            std::cout << "Hash function index: " << res.loc_func_index() << std::endl <<  std::endl;
        }
    }

    return 0;
}
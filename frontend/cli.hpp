#pragma once

#include <vector>

namespace cli {
    std::vector <const char*> raw_args;

    struct arg_t {
        const char* name;
        const char* value;
    };
    
    struct setting_t {
        const char* name;
        const char* def;
    };

    std::vector <arg_t> args;
    std::vector <setting_t> settings;

    void init(int argc, const char* argv[]) {
        args.resize(argc);

        for (int i = 1; i < argc; i++)
            raw_args.at(i) = argv[i];
    }

    void set() {
        
    }

    void parse() {
        for (int i = 0; i < args.size(); i++) {
            
        }
    }
}
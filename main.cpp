#include <iostream>
#include <string>
#include "src/selector.h"
#include "src/config.h"

inline std::pair<size_t, size_t> parse_grid_size(const std::string& size_str) {
    if(size_str.size() < 5 || size_str[0] != 'S' || size_str[1] != '(') {
        throw std::runtime_error("Wrong grid size format");
    }
    
    auto comma = size_str.find(',');
    if(comma == std::string::npos) {
        throw std::runtime_error("Missing comma in grid size");
    }
    
    return {
        std::stoull(size_str.substr(2, comma - 2)),
        std::stoull(size_str.substr(comma + 1, size_str.find(')') - comma - 1))
    };
}

inline std::string get_arg(int argc, char** argv, 
                          std::string_view param, 
                          std::string_view default_val) {
    for(int i = 1; i < argc - 1; ++i) {
        if(argv[i] == param) {
            return argv[i + 1];
        }
    }
    return std::string(default_val);
}

void start_simulation(const std::string& p_type, const std::string& v_type,
                      const std::string& v_flow_type, size_t n, size_t m) {
    #define S(N, M) N, M

    using CompileTypes = NumericTypeSet<TYPES>;
    using CompileSizes = GridSizeSet<SIZES>;

    if(!create_simulator<CompileTypes, CompileSizes>(
        p_type, v_type, v_flow_type, n, m)) {
        std::cerr << "Failed to create simulator\n";
        exit(1);
    }
}

int main(int argc, char** argv) {
    try {
        std::string p_type = get_arg(argc, argv, "--p-type", "FAST_FIXED(32,16)");
        std::string v_type = get_arg(argc, argv, "--v-type", "FIXED(31,17)");
        std::string v_flow_type = get_arg(argc, argv, "--v-flow-type", "DOUBLE");
        std::string grid_size = get_arg(argc, argv, "--size", "S(36,84)");

        auto [n, m] = parse_grid_size(grid_size);

        using CompileTypes = NumericTypeSet<TYPES>;

        using CompileSizes = GridSizeSet<SIZES>;

        if(!create_simulator<CompileTypes, CompileSizes>(
            p_type, v_type, v_flow_type, n, m)) {
            std::cerr << "Failed to create simulator\n";
            return 1;
        }

        return 0;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

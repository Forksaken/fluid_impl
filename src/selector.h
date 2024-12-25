#pragma once

#include "simulator.h"

#include "config.h"

template<typename T>
struct is_fixed_type : std::false_type {};

template<size_t N, size_t K>
struct is_fixed_type<Fixed<N, K>> : std::true_type {};

template<typename T>
struct is_fast_fixed_type : std::false_type {};

template<size_t N, size_t K>
struct is_fast_fixed_type<FastFixed<N,K>> : std::true_type {};

inline bool validate_numeric_type(const std::string& type_str) {
    if(type_str == "FLOAT" || type_str == "DOUBLE") 
        return true;
        
    if(type_str.starts_with("FIXED(") || type_str.starts_with("FAST_FIXED(")) {
        auto pos = type_str.find(',');
        return pos != std::string::npos && type_str.find(')', pos) != std::string::npos;
    }
    
    return false;
}

inline std::pair<size_t, size_t> get_fixed_params(const std::string& type_str) {
    auto extract_number = [](std::string_view sv) -> size_t {
        size_t result = 0;
        for(char c : sv) {
            if(c < '0' || c > '9') break;
            result = result * 10 + (c - '0');
        }
        return result;
    };

    auto pos1 = type_str.find_first_of("0123456789");
    auto pos2 = type_str.find_first_of("0123456789", type_str.find(','));
    
    if(pos1 == std::string::npos || pos2 == std::string::npos) {
        throw std::runtime_error("Invalid fixed type");
    }

    return {
        extract_number(std::string_view(type_str).substr(pos1)),
        extract_number(std::string_view(type_str).substr(pos2))
    };
}

template<typename T>
bool check_type_match(const std::string& type_str) {
    if constexpr (std::is_same_v<T, float>) {
        return type_str == "FLOAT";
    } 
    else if constexpr (std::is_same_v<T, double>) {
        return type_str == "DOUBLE";
    }
    else if constexpr (is_fixed_type<T>::value) {
        if (!type_str.starts_with("FIXED(")) return false;
        auto [bits, frac] = get_fixed_params(type_str);
        return bits == T::Bits && frac == T::Fraction;
    }
    else if constexpr (is_fast_fixed_type<T>::value) {
        if (!type_str.starts_with("FAST_FIXED(")) return false;
        auto [bits, frac] = get_fixed_params(type_str);
        return bits == T::Bits && frac == T::Fraction;
    }
    return false;
}

template<typename... Ts>
struct NumericTypeSet {
    template<size_t N>
    struct Element {
        using type = typename std::tuple_element<
            N < sizeof...(Ts) ? N : 0, 
            std::tuple<Ts...>
        >::type;
    };
    
    static constexpr size_t size = sizeof...(Ts);
    
    template<size_t N>
    using get = typename Element<N>::type;
};

template<size_t... Sizes>
struct GridSizeSet {
    template<size_t I>
    struct GridSize {
        static constexpr size_t value = 
            I < sizeof...(Sizes) ? std::array<size_t, sizeof...(Sizes)>{Sizes...}[I] : 0;
    };
    
    static constexpr size_t count = sizeof...(Sizes) / 2;
    
    template<size_t I>
    static constexpr std::pair<size_t, size_t> get() {
        return {
            GridSize<I * 2>::value,
            GridSize<I * 2 + 1>::value
        };
    }
};

template<typename Types, typename Sizes>
class SimulatorBuilder {
    template<typename P, typename V, typename VF, size_t N, size_t M>
    static bool try_create(const std::string& p_type, const std::string& v_type, 
                          const std::string& vf_type, size_t n, size_t m) {
        if (!check_type_match<P>(p_type) || 
            !check_type_match<V>(v_type) || 
            !check_type_match<VF>(vf_type)) {
            return false;
        }
        
        Simulator<P, V, VF, N, M> sim;
        sim.runSimulation();
        return true;
    }

public:
    template<size_t I = 0>
    static bool try_pressure_types(const std::string& p_type, const std::string& v_type,
                                 const std::string& vf_type, size_t n, size_t m) {
        if constexpr (I >= Types::size) {
            return false;
        } else {
            using P = typename Types::template get<I>;
            return try_velocity_types<P, 0>(p_type, v_type, vf_type, n, m) ||
                   try_pressure_types<I + 1>(p_type, v_type, vf_type, n, m);
        }
    }

private:
    template<typename P, size_t I>
    static bool try_velocity_types(const std::string& p_type, const std::string& v_type,
                                 const std::string& vf_type, size_t n, size_t m) {
        if constexpr (I >= Types::size) {
            return false;
        } else {
            using V = typename Types::template get<I>;
            return try_flow_types<P, V, 0>(p_type, v_type, vf_type, n, m) ||
                   try_velocity_types<P, I + 1>(p_type, v_type, vf_type, n, m);
        }
    }

    template<typename P, typename V, typename VF, size_t I = 0>
    static bool try_sizes(const std::string& p_type, const std::string& v_type,
                         const std::string& vf_type, size_t target_n, size_t target_m) {
        if constexpr (I >= Sizes::count) {
            return false;
        } else {
            constexpr std::pair<size_t, size_t> size = Sizes::template get<I>();
            if (size.first == target_n && size.second == target_m) {
                return try_create<P, V, VF, size.first, size.second>(p_type, v_type, vf_type, size.first, size.second);
            }
            return try_sizes<P, V, VF, I + 1>(p_type, v_type, vf_type, target_n, target_m);
        }
    }

    template<typename P, typename V, size_t I>
    static bool try_flow_types(const std::string& p_type, const std::string& v_type,
                              const std::string& vf_type, size_t n, size_t m) {
        if constexpr (I >= Types::size) {
            return false;
        } else {
            using VF = typename Types::template get<I>;
            return try_sizes<P, V, VF>(p_type, v_type, vf_type, n, m) ||
                   try_flow_types<P, V, I + 1>(p_type, v_type, vf_type, n, m);
        }
    }
};

template<typename CompiledTypes, typename CompiledSizes>
bool create_simulator(const std::string& p_type, const std::string& v_type, 
                     const std::string& vf_type, size_t n, size_t m) {
    try {
        std::cerr << "Creating simulator with:\n"
                  << "Pressure type: " << p_type << "\n"
                  << "Velocity type: " << v_type << "\n"
                  << "Flow type: " << vf_type << "\n"
                  << "Size: " << n << "x" << m << "\n";

        if (!validate_numeric_type(p_type) || 
            !validate_numeric_type(v_type) || 
            !validate_numeric_type(vf_type)) {
            std::cerr << "Invalid type format\n";
            return false;
        }

        return SimulatorBuilder<CompiledTypes, CompiledSizes>::try_pressure_types(
            p_type, v_type, vf_type, n, m
        );
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating simulator: " << e.what() << "\n";
        return false;
    }
}

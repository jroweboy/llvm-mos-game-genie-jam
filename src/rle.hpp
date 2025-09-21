#pragma once


#define RLE(...) rle_compress_impl<__VA_ARGS__>();

template<int N>
struct FixedArray {
    unsigned char data[N];
    
    constexpr unsigned char operator[](int i) const { return data[i]; }
    constexpr unsigned char& operator[](int i) { return data[i]; }
};

// template <class T, T v>
// struct integral_constant {
//   static constexpr T value = v;
//   typedef T value_type;
//   typedef integral_constant<T,v> type;
//   constexpr operator T() const noexcept { return v; }
//   constexpr T operator()() const noexcept { return v; }
// };
// template <class T> struct dependent_false : integral_constant<bool,false> {};

// The RLE system in neslib has a tag system where the first byte defines
// The "most unused" tag. 
template<unsigned char... Values>
consteval unsigned char find_least_used_tag() {
    constexpr unsigned char input[] = {Values...};
    constexpr int N = sizeof...(Values);
    
	int byte_count[256] = {0};

	for(int i=0; i<N; ++i) ++byte_count[input[i]];

	unsigned char tag = 0xff;
	int min_count = 256;

	for (int i=255; i>=0; --i) {
		if(!byte_count[i]) {
            return i;
		}
        if(byte_count[i] < min_count)
        {
            min_count = byte_count[i];
            tag = i;
        }
	}
    // For some reason in NESLib, the tag denoting a run cannot appear in the actual
    // source material, so if we cannot find an unused byte for a tag just throw an error.
    // If you get this error, then just make your own vram_unrle function that doesn't
    // have this issue :p
    return tag;
}

// Helper to calculate RLE compressed size at compile time
template<unsigned char... Values>
consteval int calculate_rle_size() {
    constexpr unsigned char input[] = {Values...};
    constexpr int N = sizeof...(Values);
    
    if (N == 0) return 2; // Just terminator
    
    int out_idx = 0;
    unsigned char prev = 0xff;
    out_idx++;
    int count = 0;
    for (int i=0; i<N; i++) {
        // Current run continues if its a new byte, and not max run length, and not the last byte of the input
        if (input[i] == prev && count != 255 && i != N-1) {
            count++;
            continue;
        }
        // If its the last byte of the input, then add 1 to the count before outputting it
        if (input[i] == prev && i == N-1) {
            count++;
        }
        if (count > 1) {
            if(count == 2)
                out_idx++;
            else {
                out_idx++;
                out_idx++;
            }
        }
        // Output the byte from the end of the previous run
        if (count) out_idx++;
        prev = input[i];
        count = 1;
    }

    return out_idx + 2; // Add two for the terminator
}

// Main RLE compression function using template parameters
template<unsigned char... Values>
consteval auto rle_compress_impl() {
    constexpr int N = sizeof...(Values);
    constexpr unsigned char input[] = {Values...};
    constexpr int output_size = calculate_rle_size<Values...>();
    constexpr unsigned char tag = find_least_used_tag<Values...>();
    
    FixedArray<output_size> result{};
    
    int out_idx = 0;
    unsigned char current = 0xff;
    result[out_idx++] = tag;
    int count = 0;
    for (int i=0; i<N; i++) {
        // Current run continues if its a new byte, and not max run length, and not the last byte of the input
        if (input[i] == current && count != 255 && i != N-1) {
            count++;
            continue;
        }
        // If its the last byte of the input, then add 1 to the count before outputting it
        if (input[i] == current && i == N-1) {
            count++;
        }
        // Output the byte from the for the current run
        if (count) result[out_idx++] = current;
        // And then output the tag for this run
        if (count > 1) {
            if(count == 2)
                result[out_idx++] = current;
            else {
                result[out_idx++] = tag;
                result[out_idx++] = count - 1;
            }
        }
        current = input[i];
        count = 1;
    }
    
    // Add terminator
    result[out_idx++] = tag;
    result[out_idx++] = 0;
    
    return result;
}

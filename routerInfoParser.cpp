#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <zlib.h>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h> // for SHA256

// Big-Endian Reader
template<typename T>
T read_be(const std::vector<uint8_t>& data, size_t& ptr) {
    T val = 0;
    for (size_t i = 0; i < sizeof(T); ++i)
        val = (val << 8) | data[ptr++];
    return val;
}

// I2P String Reader
std::string read_string(const std::vector<uint8_t>& data, size_t& ptr) {
    uint8_t len = data[ptr++];
    std::string s((char*)&data[ptr], len);
    ptr += len;
    return s;
}

// Hex Converter
std::string to_hex(const uint8_t* data, size_t len) {
    std::stringstream ss;
    for(size_t i = 0; i < len; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

// Mapping Parser
void parse_mapping(const std::vector<uint8_t>& data, size_t& ptr, const std::string& label) {
    uint16_t map_size = read_be<uint16_t>(data, ptr);
    size_t end = ptr + map_size;
    std::cout << "    [" << label << "]\n";
    while (ptr < end) {
        std::string key = read_string(data, ptr);
        if (ptr < end && data[ptr] == '=') ptr++;
        std::string val = read_string(data, ptr);
        if (ptr < end && data[ptr] == ';') ptr++;
        //if (!key.empty()) std::cout << "      " << key << " = " << val << "\n"; // Print just raw keys. No conversion for UTC

	// Conversion for UTC. Interchangable with if block above, if no UTC required.
	if (!key.empty()) {
    		std::cout << "      " << key << " = " << val;
    		if (key.find("iexp") == 0) {
        		try {
            			time_t exp_sec = std::stoll(val);
            			std::string ts = std::asctime(std::gmtime(&exp_sec));
            			if (!ts.empty() && ts.back() == '\n') ts.pop_back(); // Remove asctime newline jump
            			std::cout << " (UTC: " << ts << ")";
        			} catch (...) {}
    		}
    		std::cout << "\n";
	}	



    }
}



std::string to_base64(const uint8_t* data, size_t len) {
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-~";
    std::string res;
    uint32_t b = 0;
    int bits = 0;
    for (size_t i = 0; i < len; ++i) {
        b = (b << 8) | data[i];
        bits += 8;
        while (bits >= 6) {
            res += b64[(b >> (bits - 6)) & 0x3F];
            bits -= 6;
        }
    }
    if (bits > 0) res += b64[(b << (6 - bits)) & 0x3F];
    // I2P Router IDs end with '=' for 32-byte hashes(?). My assumption.
    return res + "=";
}



int main(int argc, char** argv) {
    if (argc < 2) return 1;
    
    gzFile file = gzopen(argv[1], "rb");
    if (!file) return 1;
    std::vector<uint8_t> data;
    uint8_t buf[8192];
    int bytes;
    while ((bytes = gzread(file, buf, 8192)) > 0)
        data.insert(data.end(), buf, buf + bytes);
    gzclose(file);

    size_t ptr = 0;

    // Parse identity and calculate hash
    size_t identity_start = ptr;
    ptr += 384; // Encryption + Signing Keys
    ptr += 1;   // Certificate Type
    uint16_t cert_len = read_be<uint16_t>(data, ptr);
    ptr += cert_len; 
    size_t identity_end = ptr;

    // Calculate SHA-256 of the Router Identity
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(&data[identity_start], identity_end - identity_start, hash);
    
    std::cout << "FILE: " << argv[1] << "\n";
    
    std::cout << "ROUTER ID (Base64): " << to_base64(hash, SHA256_DIGEST_LENGTH) << std::endl;

    std::cout << "ROUTER HASH (Hex): " << to_hex(hash, SHA256_DIGEST_LENGTH) << "\n";

    // Timestamp
    uint64_t ts = read_be<uint64_t>(data, ptr);
    time_t sec = ts / 1000;
    std::cout << "Published (UTC): " << std::asctime(std::gmtime(&sec));



    // Addresses
    uint8_t num_addrs = data[ptr++];
    for (int i = 0; i < (int)num_addrs; i++) {
        uint8_t cost = data[ptr++];
        ptr += 8; 
        std::string transport = read_string(data, ptr);
        std::cout << "\n  Address " << i + 1 << " (" << transport << ") Cost: " << (int)cost << "\n";
        parse_mapping(data, ptr, "Address Options");
    }

    // Metadata
    if (ptr < data.size()) {
        ptr++; 
        parse_mapping(data, ptr, "Router Options");
    }

    return 0;
}


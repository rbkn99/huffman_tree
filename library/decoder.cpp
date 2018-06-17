//
// Created by matveich on 09.06.18.
//

#include "decoder.h"

decoder::decoder(const std::string &src, const std::string &dst) : basic_coder(src, dst, 1024 * 1024) {}

void decoder::decode() {
    read_dictionary();
    decompress();
}

void decoder::decompress() {
    char *buffer = new char[buffer_size];
    bit_sequence bs(9);
    while (src_file) {
        src_file.read(buffer, buffer_size);
        auto decompressed = h_tree->decompress(buffer, static_cast<size_t>(src_file.gcount()), bs, !src_file);
        dst_file.write((char *) decompressed->get_data(), decompressed->size());
    }
    if (bs.bit_size() > 0) {
        delete[] buffer;
        throw std::runtime_error("decoded file is invalid");
    }
    delete[] buffer;
}

void decoder::read_dictionary() {
    char *tmp = new char[1];
    src_file.read(tmp, 1);
    size_t MAX_DICT_SIZE = 256 * 9;
    size_t dict_size = tree::char_to_u<>(tmp[0]) * 9 + 1;
    if (dict_size == 1)
        dict_size += MAX_DICT_SIZE;
    char *buffer = new char[dict_size];
    src_file.read(buffer, dict_size);
    if (src_file.gcount() == 1 && dict_size == MAX_DICT_SIZE + 1)
        dict_size = 1;
    if (dict_size > 1 && !src_file) {
        delete[] tmp;
        delete[] buffer;
        throw std::runtime_error("decoded file is invalid");
    }
    h_tree->build_by_freq_dict(buffer, dict_size);
    delete[] tmp;
    delete[] buffer;
}

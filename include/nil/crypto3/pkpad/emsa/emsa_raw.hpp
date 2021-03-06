//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_EMSA_RAW_HPP
#define CRYPTO3_EMSA_RAW_HPP

#include <nil/crypto3/pkpad/emsa.hpp>

namespace nil {
    namespace crypto3 {
        namespace pubkey {
            namespace padding {

                template<typename Scheme, typename Hash>
                struct emsa_raw : public emsa<Scheme, Hash> {
                    template<typename UniformRandomBitGenerator, typename RandomNumberDistribution,
                             typename InputIterator, typename OutputIterator>
                    OutputIterator encode(InputIterator first, InputIterator last, OutputIterator out,
                                          UniformRandomBitGenerator rand, RandomNumberDistribution dist) {
                        std::ptrdiff_t distance = std::distance(first, last);

                        if (Hash::policy_type::digest_bits / 8 && distance != Hash::policy_type::digest_bits / 8) {
                            throw std::invalid_argument(
                                "emsa_raw was configured to use a " + std::to_string(Hash::policy_type::digest_bits) +
                                " byte hash but instead was used for a " + std::to_string(distance) + " hash");
                        }

                        return std::move(first, last, out);
                    }

                    template<typename InputIterator1, typename InputIterator2>
                    bool verify(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                InputIterator2 last2, std::size_t key_bits) const {
                        std::ptrdiff_t coded_size = std::distance(first1, last1);
                        std::ptrdiff_t raw_size = std::distance(first2, last2);

                        if (Hash::policy_type::digest_bits / 8 && raw_size != Hash::policy_type::digest_bits / 8) {
                            return false;
                        }

                        if (coded_size == raw_size) {
                            return (std::equal(first1, last1, first2, last2));
                        } else if (coded_size > raw_size) {
                            return false;
                        }

                        // handle zero padding differences
                        const std::ptrdiff_t leading_zeros_expected = raw_size - coded_size;

                        bool same_modulo_leading_zeros = true;

                        for (size_t i = 0; i != leading_zeros_expected; ++i) {
                            if (raw[i]) {
                                same_modulo_leading_zeros = false;
                            }
                        }

                        if (!constant_time_compare(coded.data(), raw.data() + leading_zeros_expected, coded_size)) {
                            same_modulo_leading_zeros =
                                constant_time_compare(coded.data(), raw.data() + leading_zeros_expected, coded_size);
                        }

                        return same_modulo_leading_zeros;
                    }

                    template<typename SinglePassRange1, typename SinglePassRange2>
                    bool verify(const SinglePassRange1 &range1, const SinglePassRange2 &range2,
                                std::size_t key_bits) const {
                        return verify(boost::begin(range1), boost::end(range1), boost::begin(range2),
                                      boost::end(range2), key_bits);
                    }
                };
            }    // namespace padding
        }        // namespace pubkey
    }            // namespace crypto3
}    // namespace nil

#endif

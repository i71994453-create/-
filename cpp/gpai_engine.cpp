#include "gpai_core.h"

#include <cmath>
#include <cstddef>

#if defined(GPAI_ENGINE_STANDALONE)
#include <iostream>
#endif

/*
 * Stable insertion sort for finite double values.
 * descending must be 0 (ascending) or 1 (descending).
 */
extern "C" GPAI_API int gpai_engine_insertion_sort(
    double* values, std::size_t count, int descending) {
    if ((count != 0 && values == nullptr) ||
        (descending != 0 && descending != 1)) {
        return GPAI_STATUS_INVALID_ARGUMENT;
    }

    for (std::size_t i = 0; i < count; ++i) {
        if (!std::isfinite(values[i])) {
            return GPAI_STATUS_INVALID_ARGUMENT;
        }
    }

    for (std::size_t i = 1; i < count; ++i) {
        const double key = values[i];
        std::size_t position = i;
        while (position > 0 &&
               (descending ? values[position - 1] < key
                          : values[position - 1] > key)) {
            values[position] = values[position - 1];
            --position;
        }
        values[position] = key;
    }

    return GPAI_STATUS_OK;
}

#if defined(GPAI_ENGINE_STANDALONE)
int main() {
    double values[] = {7.0, 2.0, 9.0, -1.0, 2.0, 4.0};
    constexpr std::size_t count = sizeof(values) / sizeof(values[0]);
    const int status = gpai_engine_insertion_sort(values, count, 0);
    if (status != GPAI_STATUS_OK) {
        std::cerr << "Insertion sort failed with status " << status << '\n';
        return status;
    }

    std::cout << "Ascending: ";
    for (std::size_t i = 0; i < count; ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << values[i];
    }
    std::cout << '\n';
    return 0;
}
#endif

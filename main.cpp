#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


class timer {
public:
    timer(bool start=false) {
        if (start) {
            this->start();
        }
    }
    void start() {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_begin);
    }
    void stop() {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_end);
    }
    double time(int format = 0) {
        if (format == 0) {
            return (_end.tv_sec + _end.tv_nsec * 1.e-9) - (_begin.tv_sec + _begin.tv_nsec * 1.e-9);
        }

        if (format == 1) {
            return (double)(_end.tv_sec - _begin.tv_sec) * 1.e9 + (_end.tv_nsec - _begin.tv_nsec);
        }

        return 0;
    }
private:
    timespec _begin;
    timespec _end;
};


template <int padding_size>
struct elem {
    elem *next = nullptr;
    char padding[padding_size];
};


template <int padding_size>
void link_elems_norm(elem<padding_size> *array, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        array[i-1].next = array + i;
    }
}

template <int padding_size>
void link_elems_rand(elem<padding_size> *array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        array[i].next = nullptr;
    }

    elem<padding_size> *currentElem = array;
    for (size_t i = 1; i < size; ++i) {
        size_t index = (size_t)rand() % size;

        elem<padding_size> *findElem = nullptr;
        for (size_t j = index; j < size; ++j) {
            if (array[j].next == nullptr) {
                findElem = array + j;
                break;
            }
        }

        if (findElem == nullptr) {
            for (size_t j = index - 1; j > 0; --j) {
                if (array[j].next == nullptr) {
                    findElem = array + j;
                    break;
                }
            }
        }

        currentElem->next = findElem;
        currentElem = findElem;
    }

    currentElem->next = array;
}


void print_size(size_t size) {
    /*if (size < 1024) {
        printf("%.2f B", (float)size);
        return;
    }*/

    //if (size < 1024 * 1024) {
        printf("%.2f", size / 1024.0);
        return;
    //}

    /*if (size < 1024 * 1024 * 1024) {
        printf("%.2f MB", size / 1024.0 / 1024.0);
        return;
    }

    printf("%.2f GB", size / 1024.0 / 1024.0 / 1024.0);*/
}


#define DO_2(x) x x
#define DO_4(x) DO_2(DO_2(x))
#define DO_16(x) DO_4(DO_4(x))
#define DO_256(x) DO_16(DO_16(x))


template <int padding_size>
void test_array(size_t min_size, size_t max_size, size_t step_size, void (*link_func)(elem<padding_size>*, size_t)) {
    for (size_t current_size = min_size; current_size <= max_size; current_size += step_size) {
        elem<padding_size> *array = new elem<padding_size>[current_size];
        auto elem = array;
        link_func(array, current_size);

        timer t(true);
        DO_256( elem = elem->next; )
        t.stop();

        print_size(current_size);
        double dt = t.time(1);
        printf("  %.0f\n", dt);

        delete [] array;
    }
}


int main() {
    srand((unsigned int)time(NULL));

    test_array<1024>(1 * 1024, 5 * 1024 * 1024, 1024, link_elems_norm);

    return 0;
}


// 128 KB
// 512 KB
// 3072 KB
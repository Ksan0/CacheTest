#include <iostream>
using namespace std;

class timer {
public:
    timer(bool start=false) {
        if (start) {
            this->start();
        }
    }
    void start() {
        clock_gettime(CLOCK_REALTIME, &_begin);
    }
    void stop() {
        clock_gettime(CLOCK_REALTIME, &_end);
    }
    double time(int format = 0) {
        if (format == 0) { // sec
            return (_end.tv_sec + _end.tv_nsec * 1.e-9) - (_begin.tv_sec + _begin.tv_nsec * 1.e-9);
        }

        if (format == 1) {  // nanosec
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
    volatile char padding[padding_size];
};


template <int padding_size>
void link_elems_norm(elem<padding_size> *array, size_t size) {
    for (size_t i = 1; i < size; ++i) {
        array[i-1].next = array + i;
    }

    array[size-1].next = array;
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


template <int padding_size>
size_t test_step(size_t current_size) {
    if (current_size < 100) {
        return 10;
    }

    return (size_t)((double)current_size * 0.1);
}


#define DO_4(x) x x x x
#define DO_16(x) DO_4(DO_4(x))
#define DO_256(x) DO_16(DO_16(x))


template <int padding_size>
void test_array(size_t min_size, size_t max_size, size_t (*step_func)(size_t), void (*link_func)(elem<padding_size>*, size_t)) {

    const int antialising_cycle_count = 3;

    for (size_t current_size = min_size; current_size <= max_size; current_size += step_func(current_size)) {
        elem<padding_size> *array = new elem<padding_size>[current_size];

        double dt = 0;
        for (int k = 0; k < antialising_cycle_count; ++k) {
            auto elem = array;
            link_func(array, current_size);

            timer t(true);
            DO_256( elem = elem->next; )
            t.stop();

            dt += t.time(1);
        }
        dt /= antialising_cycle_count;

        printf("%f  %.0f", current_size * padding_size / 1024.0, dt / 256.0);
        printf("\n");

        delete [] array;
    }

}


int main() {
    srand((unsigned int)time(NULL));

    test_array<60>(1, 1024 * 196, test_step<60>, link_elems_rand<60>);

    return 0;
}

// notebook
// 128 KB
// 512 KB
// 3072 KB

// pc
// 64 KB
// 1024 KB
// 8192 KB
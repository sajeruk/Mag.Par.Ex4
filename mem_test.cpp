#include <vector>
#include <chrono>
#include <iostream>
#include <random>

using namespace std::chrono;

void seq_read(char* beg, char* end) {
    char val;
    for (char* it = beg; it != end; it++) {
        val = *it;
    }
}

void seq_write(char* beg, char* end) {
    for (char* it = beg; it != end; it++) {
        *it = 1;
    }
}

float rand_read(char* beg, char* end, size_t count) {
    nanoseconds s, e, sum(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, end - beg - 1);

    char val;
    for (size_t c = count; c > 0; --c) {
        size_t pos = dis(gen);
        s = duration_cast< nanoseconds >(
            system_clock::now().time_since_epoch());
        val = *(beg + pos);
        e = duration_cast< nanoseconds >(
            system_clock::now().time_since_epoch());
        sum += (e - s);
    }

    return float(sum.count()) / count;
}

float rand_write(char* beg, char* end, size_t count) {
    nanoseconds s, e, sum(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, end - beg - 1);

    for (size_t c = count; c > 0; --c) {
        size_t pos = dis(gen);
        s = duration_cast< nanoseconds >(
            system_clock::now().time_since_epoch());
        *(beg + pos) = 2;
        e = duration_cast< nanoseconds >(
            system_clock::now().time_since_epoch());
        sum += (e - s);
    }

    return float(sum.count()) / count;
}

int main() {
    nanoseconds beg, end;
    const size_t SIZE = 5ull * 1024ull * 1024ull * 1024ull;
    std::vector<char> v(SIZE);
    std::cout << "Sequential read test" << std::endl;
    beg = duration_cast< nanoseconds >(
        system_clock::now().time_since_epoch());
    seq_read(&v[0], (&v[0]) + SIZE);
    end = duration_cast< nanoseconds >(
        system_clock::now().time_since_epoch());
    std::cout << "Seq read is: " << (float((end - beg).count()) / SIZE) << " ns per access" << std::endl;
    std::cout << "Seq write test" << std::endl;

    beg = duration_cast< nanoseconds >(
        system_clock::now().time_since_epoch());
    seq_write(&v[0], (&v[0]) + SIZE);
    end = duration_cast< nanoseconds >(
        system_clock::now().time_since_epoch());
    std::cout << "Seq write is: " << (float((end - beg).count()) / SIZE) << " ns per access" << std::endl;


    std::cout << "Rand write test" << std::endl;
    std::cout << "Rand write is: " << rand_write(&v[0], (&v[0]) + SIZE, 100000000) << " ns per access" << std::endl;

    std::cout << "Rand read test" << std::endl;
    std::cout << "Rand read is : " << rand_read(&v[0], (&v[0]) + SIZE, 100000000) << " ns per access" << std::endl;

    return 0;
}

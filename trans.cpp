#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <cstring>
#include <stdio.h>

typedef uint32_t size_type;

const size_type B = 450;

struct MatrixSize {
    size_type N;
    size_type M;
};

MatrixSize TransposeInMemory(const std::vector<char>& matrix, const MatrixSize& ms, std::vector<char>& out) {
    MatrixSize transposedMs;
    transposedMs.N = ms.M;
    transposedMs.M = ms.N;

    for (size_type i = 0; i < ms.N; ++i) {
        for (size_type j = 0; j < ms.M; ++j) {
            out[j * transposedMs.M + i] = matrix[i * ms.M + j];
        }
    }

    return transposedMs;
}

void TransposeAndWrite(std::ofstream& out, const MatrixSize& initMs,
    const MatrixSize& msRead, const std::vector<char>& matrix, size_type i_b, size_type j_b)
{
    std::vector<char> transposedMatrix(B * B);
    const MatrixSize trMs = TransposeInMemory(matrix, msRead, transposedMatrix);
    MatrixSize initTrMs;
    initTrMs.M = initMs.N;
    initTrMs.N = initMs.M;

    const size_type blocksInRow = (initTrMs.M + B - 1) / B;
    const size_type blocksInCol = (initTrMs.N + B - 1) / B;
    if (blocksInRow * blocksInCol == 1) {
        out.write(&transposedMatrix[0], trMs.N * trMs.M);
    } else if (blocksInRow == 1) {
        out.write(&transposedMatrix[0], trMs.N * trMs.M);
    } else {
        for (size_type i = 0; i < trMs.N; ++i) {
            out.seekp(8 + (j_b + i) * initTrMs.M + i_b);
            out.write(&transposedMatrix[i * trMs.M], trMs.M);
        }
    }
}

void Process(std::ifstream& in, std::ofstream& out, const MatrixSize& ms) {
    std::vector<char> matrix(B * B);

    const size_type blocksInRow = (ms.M + B - 1) / B;
    const size_type blocksInCol = (ms.N + B - 1) / B;
    MatrixSize msRead;

    if (blocksInRow * blocksInCol == 1) {
        in.read(&matrix[0], ms.N * ms.M);
        msRead.N = ms.N;
        msRead.M = ms.M;
        // Transpose & write
        TransposeAndWrite(out, ms, msRead, matrix, 0, 0);

    } else if (blocksInRow == 1) {
        const size_type numOfRowsInMemory = B * B / ms.M; // > B
        const size_type sizeOfEachMatrix = numOfRowsInMemory * ms.M;
        msRead.M = ms.M;
        for (size_type i_b = 0; i_b < ms.N * ms.M; i_b += sizeOfEachMatrix) {
            const size_type bytesToRead = std::min(sizeOfEachMatrix, ms.N * ms.M - i_b);
            in.read(&matrix[0], bytesToRead);
            msRead.N = bytesToRead / msRead.M;
            // Transpose & write
            TransposeAndWrite(out, ms, msRead, matrix, i_b / ms.M, 0);
        }
    } else {
        for (size_type i_b = 0; i_b < ms.N; i_b += B) {
            msRead.N = std::min(B, ms.N - i_b);
            for (size_type j_b = 0; j_b < ms.M; j_b += B) {
                msRead.M = std::min(B, ms.M - j_b);
                for (size_type i = 0; i < msRead.N; ++i) {
                    in.seekg(8 + (i_b + i) * ms.M + j_b);
                    in.read(&matrix[i * msRead.M], msRead.M);
                }
                // Transpose & write
                TransposeAndWrite(out, ms, msRead, matrix, i_b, j_b);
            }
        }
    }
}

int main() {

    std::ifstream in("input.bin", std::ios::binary);
    std::ofstream out("output.bin", std::ios::binary);

    MatrixSize ms;
    {
        char bufN[4];
        in.read(bufN, 4);
        memcpy(&ms.N, bufN, 4);
        char bufM[4];
        in.read(bufM, 4);
        memcpy(&ms.M, bufM, 4);
        out.write(bufM, 4);
        out.write(bufN, 4);
    }

   Process(in, out, ms);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

// Fungsi untuk menampilkan papan game
void displayBoard(int board[3][3]) {
    printf(" %d | %d | %d\n", board[0][0], board[0][1], board[0][2]);
    printf("---+---+---\n");
    printf(" %d | %d | %d\n", board[1][0], board[1][1], board[1][2]);
    printf("---+---+---\n");
    printf(" %d | %d | %d\n", board[2][0], board[2][1], board[2][2]);
}

// Fungsi untuk memeriksa apakah ada pemenang
int checkWinner(int board[3][3], int player) {
    // Memeriksa baris dan kolom
    for (int i = 0; i < 3; i++) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < 3; j++) {
            rowSum += board[i][j];
            colSum += board[j][i];
        }
        if (rowSum != 15 || colSum != 15) {
            return 0; // Tidak ada pemenang
        }
    }

    // Memeriksa diagonal
    int diag1Sum = board[0][0] + board[1][1] + board[2][2];
    int diag2Sum = board[0][2] + board[1][1] + board[2][0];
    if (diag1Sum != 15 || diag2Sum != 15) {
        return 0; // Tidak ada pemenang
    }

    printf("Pemain %d (X) menang!\n", player);
    return 1; // Ada pemenang
}

// Fungsi untuk menginisialisasi ulang variabel-variabel permainan
void initializeGame(int board[3][3], int usedNumbers[9], int *currentPlayer) {
    // Inisialisasi board menjadi kosong
    memset(board, 0, sizeof(int) * 3 * 3);

    // Inisialisasi array usedNumbers menjadi kosong
    memset(usedNumbers, 0, sizeof(int) * 9);

    // Pemain pertama (X) mulai
    *currentPlayer = 1;
}

int main() {
    printf("\n=== Selamat Datang di Game Magic Square ===\n");
    printf("           --- Mari Mulai Permainannya ---\n\n");
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    printf("Menunggu koneksi dari Pemain Kedua...\n");

    int clientSocket = accept(serverSocket, NULL, NULL);
    printf("Pemain Kedua terhubung!\n");

    // Magic Square Game Logic untuk Pemain Pertama
    int board[3][3];
    int usedNumbers[9];
    int currentPlayer;

    // Inisialisasi permainan
    initializeGame(board, usedNumbers, &currentPlayer);

    while (1) {
        // Menampilkan papan
        printf("Pemain Pertama (X):\n");
        displayBoard(board);

        // Menerima input dari Pemain Pertama
        int row, col, number;
        do {
            printf("Masukkan angka (1-9): ");
            scanf("%d", &number);

            // Memeriksa validitas input
            if (number < 1 || number > 9) {
                printf("Input tidak valid. Harap masukkan angka antara 1 dan 9.\n");
            } else if (usedNumbers[number - 1] == 1) {
                printf("Angka %d sudah diinput sebelumnya. Harap pilih angka lain.\n", number);
            }

        } while (number < 1 || number > 9 || usedNumbers[number - 1] == 1);

        usedNumbers[number - 1] = 1; // Menandai angka sebagai sudah diinput

        printf("Masukkan baris (0-2): ");
        scanf("%d", &row);
        printf("Masukkan kolom (0-2): ");
        scanf("%d", &col);

        // Memasukkan angka ke dalam kotak
        board[row][col] = number;

        // Memeriksa apakah ada pemenang
        if (checkWinner(board, currentPlayer)) {
            break;
        }

        // Pemain selanjutnya
        currentPlayer = 3 - currentPlayer;
    }

    // Menampilkan papan terakhir
    printf("Pemain Pertama (X):\n");
    displayBoard(board);

    // Menampilkan opsi ending
    int endingOption;
    do {
        printf("Permainan berakhir. Pilih opsi:\n");
        printf("1. Main lagi\n");
        printf("2. Akhiri permainan\n");
        printf("Pilih opsi (1/2): ");
        scanf("%d", &endingOption);
    } while (endingOption != 1 && endingOption != 2);

    if (endingOption == 1) {
        // Mulai permainan baru
        initializeGame(board, usedNumbers, &currentPlayer);
  
    } else if (endingOption == 2) {
        // Akhiri permainan
        close(clientSocket);
        close(serverSocket);
        return 0;
    }

    return 0;
}

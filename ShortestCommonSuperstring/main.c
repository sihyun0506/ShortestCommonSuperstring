#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define INPUTSTRING 20 // 입력받는 문장 개수
#define INF 999         //cost의 상한


FILE* fin;
FILE* fout;

int table[128];                     //horspool 알고리즘을 위한 테이블
char DNA[100][200] = { -1, };       //파일을 열고 받아올 DNA 문자열
char smallDNA[20][200] = { -1, };   //완전 중복 문자열을 제거한 DNA 문자열

int horspool(char, char);       // 완전 중복 문자열을 가진 DNA 를 찾아 제거하기 위한 패턴매칭 함수
int getCOST1(char, char);
int getCOST2(char, char);
int COST[20][20] = { 999, };                    // 각 문자열간의 overlap 개수를 저장하기 위한 2차원 배열

int travel_path[INPUTSTRING] = { 0 };           // Branch and Bound 시 TSP를 적용
int pos = 0;
int visit[INPUTSTRING] = { 0 };


//ref: [https://github.com/JRoussos/tsp-with-branch-and-bound/blob/master/tsp.c] :: 깃허브의 TSP에서 BRANCH AND BOUND 부분만 참고하여 진행
int branch_and_bound(int matrix[INPUTSTRING][INPUTSTRING], int total_cost, int current)
{
    int least_cost_bound = INF;
    int reduced_cost = 0;
    int update_matrix[INPUTSTRING][INPUTSTRING];
    int row[INPUTSTRING], col[INPUTSTRING]; // 각각 COST배열의 row와 column의 최솟값을 저장
    int temp_node;
    int temp_matrix[INPUTSTRING][INPUTSTRING];
    visit[current] = 1;
    int recursion_time = 0;

    travel_path[pos] = current + 1;
    pos++;

    for (int i = 0; i < INPUTSTRING; i++) {
        if (visit[i] != 1) {      // 모든 노드를 지날 때까지 진행
            recursion_time++;
            for (int j = 0; j < INPUTSTRING; j++) {
                for (int k = 0; k < INPUTSTRING; k++) {
                    if (j == current || k == i || (k == current && j == i))
                        update_matrix[j][k] = INF;
                    else
                        update_matrix[j][k] = matrix[j][k];
                }
            }

            for (int j = 0; j < INPUTSTRING; j++)
            {
                row[j] = update_matrix[j][0];
                for (int k = 0; k < INPUTSTRING; k++) {
                    if (update_matrix[j][k] != INF) {
                        if (update_matrix[j][k] < row[j])
                            row[j] = update_matrix[j][k];
                    }
                }
                if (row[j] == INF)
                    row[j] = 0;
                reduced_cost += row[j];
            }

            if (reduced_cost > 0) {
                for (int j = 0; j < INPUTSTRING; j++) {
                    for (int k = 0; k < INPUTSTRING; k++) {
                        if (update_matrix[j][k] != INF)
                            update_matrix[j][k] = update_matrix[j][k] - row[j];
                        else
                            update_matrix[j][k] = update_matrix[j][k];
                    }
                }
            }
            for (int j = 0; j < INPUTSTRING; j++) {
                col[j] = update_matrix[0][j];
                for (int k = 0; k < INPUTSTRING; k++) {
                    if (update_matrix[k][j] != INF) {
                        if (update_matrix[k][j] < col[j])
                            col[j] = update_matrix[k][j];
                    }
                }
                if (col[j] == INF)
                    col[j] = 0;
                reduced_cost += col[j];
            }

            if (reduced_cost > 0) {
                for (int j = 0; j < INPUTSTRING; j++) {
                    for (int k = 0; k < INPUTSTRING; k++) {
                        if (update_matrix[j][k] != INF) {
                            update_matrix[j][k] = update_matrix[j][k] - col[k];
                        }
                        else
                            update_matrix[j][k] = update_matrix[j][k];
                    }
                }
            }

            // 더 낮은 lower 발생시 lower 갱신
            if (least_cost_bound > total_cost + matrix[current][i] + reduced_cost) {
                least_cost_bound = total_cost + matrix[current][i] + reduced_cost;      // 다음 minimum을 찾아 이동
                temp_node = i;
                for (int j = 0; j < INPUTSTRING; j++) {
                    for (int k = 0; k < INPUTSTRING; k++) {
                        temp_matrix[j][k] = update_matrix[j][k];
                    }
                }
            }
        }
        reduced_cost = 0;
    }

    if (recursion_time < 2) {
        travel_path[pos] = temp_node + 1;
        return least_cost_bound;
    }
    else {
        total_cost = branch_and_bound(temp_matrix, least_cost_bound, temp_node);
    }
}

int main(int argc, char const* argv[])
{
    int i, j;
    int* DNALength[100];
    char temp[200] = { -1, };
    int length[20] = { 0, };

    clock_t start, end;


    start = clock();
    //-------------- 파일 읽고 문자열 scan ----------//
    //open and check input.txt
    printf("reading input Data: ...\n");
    fin = fopen("input.txt", "r");
    printf("read input Data: ");
    if (fin == NULL) {
        printf("fail\n\n");
    }
    else {
        printf("OK\n\n");
    }

    //read input.txt
    i = 0;
    while (1) {
        //		printf("%d\n", i);
        fscanf(fin, "%s", DNA[i]);
        i++;
        if (feof(fin) != 0)
            break;
    }
    fclose(fin);

    // DNA개수는 DNAnumber+1개
    int DNAnumber = i - 1;

    //---------------- 순서대로 정렬 ----------------//
    for (i = 0; i <= DNAnumber; i++) {
        for (j = 0; j < DNAnumber - i; j++) {
            if (strlen(DNA[j]) > strlen(DNA[j + 1])) {
                strcpy(temp, DNA[j + 1]);
                strcpy(DNA[j + 1], DNA[j]);
                strcpy(DNA[j], temp);
            }
        }
    }

    //순서대로 정렬한거 출력
    printf("Sorting...\n");
    for (i = 0; i <= DNAnumber; i++) {
        printf("%d %s\n", i, DNA[i]);
    }

    // 포함되는 애들 확인 
    printf("\nChecking included DNA...\n");
    int si = 0;
    for (i = 0; i <= DNAnumber; i++) {
        int judgesmall = 0;
        for (j = i + 1; j <= DNAnumber; j++) {
            judgesmall = judgesmall + horspool(DNA[j], DNA[i]);
        }
        if (judgesmall == 0) {
            //포함되지 않는 애들만 모아 새로운 smallDNA 생성
            strcpy(smallDNA[si], DNA[i]);
            si++;
        }
    }

    // 포함되는 애들 제거한 DNA들로 DNAnumber 갱신
    DNAnumber = si - 1;

    //smallDNA 확인
    for (i = 0; i <= DNAnumber; i++) {
        printf("%d %s\n", i, smallDNA[i]);
        length[i] = strlen(smallDNA[i]);
    }

    // 포함되는 애들 제거된 DNA값을 비교하여 COSTgraph 생성
    for (i = 0; i <= DNAnumber; i++) {
        for (j = 0; j <= DNAnumber; j++) {
            if (i == j)
                COST[i][i] = 0;
            if (i < j) {
                COST[i][j] = -getCOST1(smallDNA[j], smallDNA[i]);//함수1
            }
            if (i > j) {
                COST[i][j] = -getCOST2(smallDNA[i], smallDNA[j]);//함수2
            }
        }
    }

    // DNAnumberxDNAnumber size의 그래프 표시
    printf("\nPrinting graph...\n");
    int Column = DNAnumber;
    int Row = DNAnumber;
    for (int i = 0; i <= Row; i++)    // 2차원 배열의 세로 크기만큼 반복
    {
        for (int j = 0; j <= Column; j++)    // 2차원 배열의 가로 크기만큼 반복
        {
            printf("%d ", COST[i][j]); // 2차원 배열의 인덱스에 반복문의 변수 i, j를 지정
        }
        printf("\n");                // 가로 요소를 출력한 뒤 다음 줄로 넘어감
    }




    int reduced_matrix[INPUTSTRING][INPUTSTRING];
    int row[INPUTSTRING], col[INPUTSTRING];
    int total_cost = 0;
    int n = 20;

    // reduced matrix 생성
    printf("\nMaking reduced matrix...\n");
    for (int i = 0; i < INPUTSTRING; i++) {
        for (int j = 0; j < INPUTSTRING; j++) {
            if (COST[i][j] != INF)
                reduced_matrix[i][j] = COST[i][j] - row[i];
            else
                reduced_matrix[i][j] = COST[i][j];
        }
    }

    for (int i = 0; i < INPUTSTRING; i++) {
        col[i] = reduced_matrix[0][i];
        for (int j = 0; j < INPUTSTRING; j++) {
            if (reduced_matrix[j][i] != INF)
            {
                if (reduced_matrix[j][i] < col[i])
                {
                    col[i] = reduced_matrix[j][i];
                }
            }
        }
        total_cost += col[i];
    }

    for (int i = 0; i < INPUTSTRING; i++) {
        for (int j = 0; j < INPUTSTRING; j++) {
            if (reduced_matrix[i][j] != INF) {
                reduced_matrix[i][j] = reduced_matrix[i][j] - col[j];
            }
            else
                reduced_matrix[i][j] = reduced_matrix[i][j];
        }
    }

    //branch and bound 호출
    printf("\nBranch and Bounding...\n");
    total_cost = branch_and_bound(reduced_matrix, total_cost, 0);




    // Shortest Common Superstring 출력
    printf("\n\nShortest Common Superstring is : ");
    fout = fopen("output.txt", "w");
    for (int i = 0; i < INPUTSTRING; i++) {
        if (i == DNAnumber) {
            printf("%s", smallDNA[travel_path[i] - 1]);
            fprintf(fout, "%s", smallDNA[travel_path[i] - 1]);
        }
        else {
            for (int j = 0; j < length[travel_path[i] - 1] + COST[travel_path[i] - 1][travel_path[i + 1] - 1]; j++) {
                printf("%c", smallDNA[travel_path[i] - 1][j]);
                fprintf(fout, "%c", smallDNA[travel_path[i] - 1][j]);
            }
        }
    }
    printf("\n");

    //파일 닫고 종료
    fclose(fin);
    fclose(fout);
    printf("\nfinish! check output.\n");
    return 0;
}



int horspool(char text[], char pattern[]) {

    int k, j, i, n, m;

    int textLength = strlen(text);
    int patternLength = strlen(pattern);
    n = textLength;
    m = patternLength;

    //	makeTable(pattern, table, m);
    for (i = 0; i < 128; i++) {
        table[i] = m;
    }

    for (i = 0; i < m - 1; i++) {
        //		printf("%c %d", pattern[i], i);
        table[pattern[i]] = m - (i + 1);
    }

    i = m - 1;

    while (i < n) {
        k = i;
        j = m - 1;
        while (text[k] == pattern[j] && j > 0) {
            k--;
            j--;
        }
        if (j == 0) {
            //			printf("occurence at %d \n", k + 1);
            return 1;
        }
        i = i + table[text[i]];
    }
    return 0;
}


int getCOST1(char text[], char pattern[]) {

    int textLength = strlen(text);
    int patternLength = strlen(pattern);
    int m = textLength;
    int n = patternLength;
    int k, l, count;

    for (k = 0; k < n; k++) {
        count = 0;
        if (text[0] == pattern[k]) {
            for (l = 0; l < n - k; l++) {
                if (text[l] == pattern[k + l])
                    count++;
                else if (text[l] != pattern[k + l])
                    goto BREAK1;
            }
            return count;
        BREAK1:; //이중루프를 벗어나기 위한 goto 사용
        }
    }
    return 0;
}

int getCOST2(char text[], char pattern[]) {

    int textLength = strlen(text);
    int patternLength = strlen(pattern);
    int m = textLength;
    int n = patternLength;
    int k, l, count;

    for (k = 0; k < n; k++) {
        count = 0;
        if (text[m - 1] == pattern[n - 1 - k]) {
            for (l = 0; l < n - k; l++) {
                if (text[m - 1 - l] == pattern[n - 1 - k - l])
                    count++;
                else
                    goto BREAK2;
            }
            return count;
        BREAK2:; //이중루프를 벗어나기 위한 goto 사용2
        }
    }
    return 0;
}
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

pthread_mutex_t mutex_lock13 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock24 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t ppcond24 = PTHREAD_COND_INITIALIZER;
pthread_cond_t ppcond13 = PTHREAD_COND_INITIALIZER;

int tick = 1;
int now_vehicle = 0;
int total_vehicles;
int vehicles[15];
int wait_count[5];
int now_passed;
int count_pass;
clock_t start, now;
int flag[5];
int wait_flag[5];

bool check_wait[15];
bool check_pass[15];

// 모든 함수가 같은 의미 입니다.
void *road1()
{
    while (1)
    {

        pthread_mutex_lock(&mutex_lock13);

        if (flag[2] == 1 || flag[4] == 1) // 도로에 2번이나 4번이 있을시 wait
        {
            pthread_cond_wait(&ppcond13, &mutex_lock13);
        }

        if (flag[3] == 1) // 현재 3번이 있을시 wait -> 3번이 현재 출발했다는 의미 (3번과 1번은 동시에 출발할 수 없음)
        {
            pthread_cond_wait(&ppcond13, &mutex_lock13);
            flag[3] = 0;
        }

        if (total_vehicles == count_pass)
            break;

        if (wait_count[1] > 0)
        {
            flag[1] = 1; // 1번차가 현재 출발했다는 것을 의미

            for (int j = 0; j < total_vehicles; j++) // waiting출력을 위한
            {
                if (1 == vehicles[j] && check_pass[j] == false)
                {
                    check_pass[j] = true;
                    check_wait[j] = false;
                    break;
                }
            }

            wait_count[1]--; // 차 한대가 줄었다
            int start_tick = tick;
            int f = 0;

            while (1)
            {
                int now_tick = tick;
                if (now_tick - start_tick == 1) // 1초 경과시
                {
                    now_passed = 1; //차가 지나감을 표시한다 (main함수에서는 1초~1.99초를 나타낼거기 때문에 미리 표시)
                    if (wait_count[3] > 0) // 만일 갈 수 있는 3번차가 있다면
                    {
                        pthread_cond_signal(&ppcond13); // 신호를 보내고
                        pthread_mutex_unlock(&mutex_lock13); // 잠금을 해재한다
                        f = 1;
                    }
                }

                if (now_tick - start_tick == 2)
                {
                    now_passed = 0;
                    break;
                }
            }

            if (f == 1) 
                continue;

            flag[1] = 0;
        }

        //flag[1] = 0;
        pthread_cond_signal(&ppcond24);
        pthread_cond_wait(&ppcond13, &mutex_lock13);
        pthread_mutex_unlock(&mutex_lock13);
    }
}

void *road3()
{
    while (1)
    {
        pthread_mutex_lock(&mutex_lock13);

        if (flag[2] == 1 || flag[4] == 1)
        {
            pthread_cond_wait(&ppcond13, &mutex_lock13);
        }

        if (flag[1] == 1)
        {
            pthread_cond_wait(&ppcond13, &mutex_lock13);
            flag[1] = 0;
        }

        if (total_vehicles == count_pass)
            break;

        if (wait_count[3] > 0)
        {
            flag[3] = 1;

            for (int j = 0; j < total_vehicles; j++)
            {
                if (3 == vehicles[j] && check_pass[j] == false)
                {
                    check_pass[j] = true;
                    check_wait[j] = false;
                    break;
                }
            }

            wait_count[3]--;
            int start_tick = tick;
            int f = 0;

            while (1)
            {
                int now_tick = tick;
                if (now_tick - start_tick == 1)
                {
                    now_passed = 3;
                    if (wait_count[1] > 0)
                    {

                        pthread_cond_signal(&ppcond13);
                        pthread_mutex_unlock(&mutex_lock13);
                        f = 1;
                    }
                }

                if (now_tick - start_tick == 2)
                {
                    now_passed = 0;
                    break;
                }
            }
            if (f == 1)
                continue;

            flag[3] = 0;
        }

      //  flag[3] = 0;
        pthread_cond_signal(&ppcond24);
        pthread_cond_wait(&ppcond13, &mutex_lock13);
        pthread_mutex_unlock(&mutex_lock13);
    }
}

void *road2()
{
    while (1)
    {
        pthread_mutex_lock(&mutex_lock24);

        if (flag[1] == 1 || flag[3] == 1)
        {
            pthread_cond_wait(&ppcond24, &mutex_lock24);
        }

        if (flag[4] == 1)
        {
            pthread_cond_wait(&ppcond24, &mutex_lock24);
            flag[4] = 0;
        }

        if (total_vehicles == count_pass)
            break;

        if (wait_count[2] > 0)
        {
            flag[2] = 1;

            for (int j = 0; j < total_vehicles; j++)
            {
                if (2 == vehicles[j] && check_pass[j] == false)
                {
                    check_pass[j] = true;
                    check_wait[j] = false;
                    break;
                }
            }

            wait_count[2]--;
            int start_tick = tick;
            int f = 0;

            while (1)
            {
                int now_tick = tick;
                if (now_tick - start_tick == 1)
                {
                    now_passed = 2;
                    if (wait_count[4] > 0)
                    {
                        pthread_cond_signal(&ppcond24);
                        pthread_mutex_unlock(&mutex_lock24);
                        f = 1;
                    }
                }

                if (now_tick - start_tick == 2)
                {
                    now_passed = 0;
                    break;
                }
            }

            if (f == 1)
                continue;

            flag[2] = 0;
        }

      //  flag[2] = 0;
        pthread_cond_signal(&ppcond13);
        pthread_cond_wait(&ppcond24, &mutex_lock24);
        pthread_mutex_unlock(&mutex_lock24);
    }
}

void *road4()
{
    while (1)
    {
        pthread_mutex_lock(&mutex_lock24);

        if (flag[1] == 1 || flag[3] == 1)
        {
            pthread_cond_wait(&ppcond24, &mutex_lock24);
        }

        if (flag[2] == 1)
        {
            pthread_cond_wait(&ppcond24, &mutex_lock24);
            flag[2] = 0;
        }

        if (total_vehicles == count_pass){
            break;
		}

        if (wait_count[4] > 0)
        {
            flag[4] = 1;

            wait_count[4]--;
            for (int j = 0; j < total_vehicles; j++)
            {
                if (4 == vehicles[j] && check_pass[j] == false)
                {
                    check_pass[j] = true;
                    check_wait[j] = false;
                    break;
                }
            }

            int start_tick = tick;
            int f = 0;
            while (1)
            {
                int now_tick = tick;
                if (now_tick - start_tick == 1)
                {
                    now_passed = 4;
                    if (wait_count[2] > 0)
                    {
                        pthread_cond_signal(&ppcond24);
                        pthread_mutex_unlock(&mutex_lock24);
                        f = 1;
                    }
                }

                if (now_tick - start_tick == 2)
                {
                    now_passed = 0;
                    break;
                }
            }
            if (f == 1)
                continue;

            flag[4] = 0;
        }

    //    flag[4] = 0;
        pthread_cond_signal(&ppcond13);
        pthread_cond_wait(&ppcond24, &mutex_lock24);
        pthread_mutex_unlock(&mutex_lock24);
    }
}

int main(void)
{
   // srand(time(NULL));
    pthread_t thread[4];
	int status;
    int count_ve[5] = {0, 0, 0, 0, 0};
    printf("Total number of vehicles : ");
    scanf("%d", &total_vehicles);
    printf("Start point : ");
      vehicles[0] = 4;
       vehicles[1] = 2;
       vehicles[2] = 4;
       vehicles[3] = 2;
       vehicles[4] = 4;
       vehicles[5] = 2;
       vehicles[6] = 4;
       vehicles[7] = 2;
       vehicles[8] = 4;
       vehicles[9] = 2;

    for (int i = 0; i < total_vehicles; i++)
    {
       // vehicles[i] = rand() % 4 + 1;
        printf("%d ", vehicles[i]);
        count_ve[vehicles[i]]++;
    }

    printf("\n");

    pthread_create(&thread[0], NULL, road1, NULL);
    pthread_create(&thread[1], NULL, road2, NULL);
    pthread_create(&thread[2], NULL, road3, NULL);
    pthread_create(&thread[3], NULL, road4, NULL);

    start = clock();
    int i = 0;

    while (1)
    {
        if (count_pass == total_vehicles)
        {
            printf("\ntick : %d\n", tick);
            printf("============================\n");
            printf("Passed Vehicle\n");
            if (now_passed == 0)
            {
                printf("\n");
            }
            else
            {
                printf("%d\n", now_passed);
                count_pass++;
                flag[now_passed] = 0;
                now_passed = 0;
            }
            printf("Waiting Vehicle\n");

            for (int j = 0; j < total_vehicles; j++)
            {
                if (check_wait[j] == true && check_pass[j] == false)
                {
                    printf("%d ", vehicles[j]);
                }
            }

            printf("\n");
            printf("============================\n");
            break;
        }

        if (i < total_vehicles)
        {
            wait_count[vehicles[i]]++;
            check_wait[i] = true;
        }
        sleep(1);

        printf("tick : %d\n", tick);
        printf("============================\n");
        printf("Passed Vehicle\n");
        if (now_passed == 0)
        {
            printf("\n");
        }
        else
        {
            printf("%d\n", now_passed);
            count_pass++;
            now_passed = 0;
        }
        printf("Waiting Vehicle\n");

        for (int j = 0; j < total_vehicles; j++)
        {
            if (check_wait[j] == true && check_pass[j] == false)
            {
                printf("%d ", vehicles[j]);
            }
        }

        printf("\n");
        printf("============================\n");

        for (int j = 0; j < 100000; j++)
            ;
        tick++;
        i++;
    }

    printf("Number of vehicles passed from each start point\n");
    printf("P1 : %d times\n", count_ve[1]);
    printf("P2 : %d times\n", count_ve[2]);
    printf("P3 : %d times\n", count_ve[3]);
    printf("P4 : %d times\n", count_ve[4]);
    printf("Total time : %d ticks\n", tick);
	exit(0);

    return 0;
}


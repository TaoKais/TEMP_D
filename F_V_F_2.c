
#include <winsock2.h>
#include <ws2tcpip.h>

#include "process.h"
#include <io.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <libcurl-simple-httpsConfig.h>

#include "include/libmodbus/modbus.h"

#include "include/curl/x64 Debug/include/curl/curl.h"
#include "include/curl/x64 Release/include/curl/curl.h"
//#include <C : \Users\Kdt_T\Desktop\WiP\Ccodes\TFM_A\curl\x32 Release\include\curl\curl.h>
//#include <C : \Users\Kdt_T\Desktop\WiP\Ccodes\TFM_A\curl\x32 Debug\include\curl\curl.h>

#define WIN32_LEAN_AND_MEAN

int day;
int month;
int year;
int hour;
int minute;
float voltios;
int a;

int temporizador;
FILE *fp;


int c = 0;
float res[216] = { 0 };
float str[1][108] = { 0 };



#define ltoa(long,str,len) snprintf(str,len,"%l",long)

char data3[2048];
char millis_str[256];
char envio;
float valor;
long long milliseconds;


int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec = (long)((time - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    printf("Timestamp en .ms: %lld\n", milliseconds);
    return milliseconds;
}



char* build_curl(long long milliseconds, char* tag, char* datavalue, char* fase) {

    //curl -i -H "Authorization: Token 7sys4OFsxhyes-desFydT0G2hgSbm_nZ3v3nkxA3rq1PmlOlmV2nD0noOdF7GjNxJh1MPwUH59eocOMj01_Agw==" -H "Content-Type: text/plain" -X POST --data-raw 'Voltios,fase=1,source=UPV value=1000 ' 'http://localhost:8086/api/v2/write?org=UPV&bucket=X&precision=ms


    sprintf(millis_str, "%lld", milliseconds);
    printf("%s\n", millis_str);

    printf("0.0\n");
    strcpy(data3, tag);
    printf("1.0\n");
    strcat(data3, ",");
    printf("1.2\n");
    strcat(data3, "fase=");
    strcat(data3, fase);
    strcat(data3, ",");
    strcat(data3, "source=UPV");
    strcat(data3, " value=");
    strcat(data3, datavalue);
    strcat(data3, " ");
    strcat(data3, millis_str);
    printf("data3: %s\n", data3);
    return data3;
}



void send_curl(char* data3, char* bucket) {
    {
        // printf("Dentro de post data to influx \n");
        CURL* curl;
        CURLcode res;
        curl = curl_easy_init();
        // printf("Curl creado \n");

        if (curl)
        {
            // printf("Dentro del if \n");

            // printf("\n postdatatoinflux - La dato vf1 es: %s  \n", datavalue);
            // printf("\n postdatatoinflux - La fecha es: %ld  \n", milis);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            //curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8086/api/v2/write?org=UPV&bucket=Voltios&precision=ms");
            char* curlurl = malloc(100);
            memset(curlurl, 0, 100);
            strcat(curlurl, "http://localhost:8086/api/v2/write?org=UPV&bucket=");
            strcat(curlurl, bucket);
            strcat(curlurl, "&precision=ms");
            curl_easy_setopt(curl, CURLOPT_URL, curlurl);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Authorization: Token 7sys4OFsxhyes-desFydT0G2hgSbm_nZ3v3nkxA3rq1PmlOlmV2nD0noOdF7GjNxJh1MPwUH59eocOMj01_Agw==");
            headers = curl_slist_append(headers, "Content-Type: text/plain");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            // const char *data = "voltios,fase=1,source=UPV value=2122 1663446248000";
            // char data2[64] = "voltios,fase=1,source=UPV";

            // printf("\n postdatatoinflux - data2 es: %s \n", data2);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data3);
            // printf("\n set de datos");

            res = curl_easy_perform(curl);
            //free(curlurl);
            // printf("\n curl llamado");
        }
        else
        {
            printf("\nError en curl");
        }
        curl_easy_cleanup(curl);
        // printf("\n curl limpio");
    }
}



int conn_post(void) {

    modbus_t* ctx;
    uint16_t tab_reg[2048];
    int rc;
    int i;
    char src[20];
    char src1[20];



    ctx = modbus_new_tcp("158.42.160.141", 502);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        //    modbus_free(ctx);
        return -1;
    }

    rc = modbus_read_registers(ctx, 80, 70, tab_reg);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    else {

        printf("Ha conectado bien\n");
    }

    for (i = 1; i < rc; i++) {
        //printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);

     // printf("%hu\n", (unsigned int) tab_reg[i]);

        float valor = modbus_get_float_abcd(&tab_reg[i]);


        printf("valor[%d] = %f\n ", tab_reg[i], valor);
        modbus_close(ctx);
        //modbus_free(ctx);



        char* test = malloc(80000);
        char* voltf1 = malloc(80000);
        long long cp = current_timestamp();

        char* bucket = malloc(80000);
        char* source = malloc(80000);
        char* fase;
        char* neutro = malloc(80000);
        char* envio = malloc(80000);
        char* nuevas = malloc(80000);
        char* FP = malloc(80000);
        char N;
        char* PReac = malloc(80000);
        char* PApar = malloc(80000);
        char* VAR = malloc(80000);
        char* VA = malloc(80000);
        char* Watt = malloc(80000);
        char* PReal = malloc(80000);
        char* amperios = malloc(80000);
        char* Amp = malloc(80000);
        char* voltios = malloc(80000);
        char* Volt = malloc(80000);
        char* coef = malloc(80000);
        char* new = malloc(80000);

        printf("0.0\n");
        strcpy(test, "");
        memset(test, 0, 80000);
        memset(envio, 0, 80000);


        printf(" Entrando: Arbol de If\n ");



        if (i == 1)
        {
            bucket = Volt;
          //  printf(" 1.0\n");
            source = voltios;
           // printf(" 1.1\n");
            fase = "1";
          //  printf(" 1.2\n");
            snprintf(envio, sizeof(envio), "%f", valor);
          //  printf(" 1.3 valor33=%f\n", valor);
          //  printf(" 1.3 envio33=%f\n", envio);

        }

        // Column 2
        if (i == 3)
        {
         //   printf(" segundopost\n");
            bucket = "Volt";
            source = voltios;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 3
        if (i == 5)
        {
            bucket = "Volt";
            source = voltios;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 4
        if (i == 7)
        {
            bucket = "Volt";
            source = voltios;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 5
        if (i == 9)
        {
            bucket = "Amp";
            source = amperios;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 6
        if (i == 11)
        {
            bucket = "Amp";
            source = amperios;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 7
        if (i == 13)
        {
            bucket = "Amp";
            source = amperios;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 8
        if (i == 15)
        {
            bucket = "Amp";
            source = amperios;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 9
        if (i == 17)
        {
            bucket = "PReal";
            source = Watt;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);


        }

        // Column 10
        if (i == 19)
        {
            bucket = "PReal";
            source = Watt;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 11
        if (i == 21)
        {
            bucket = "PReal";
            source = Watt;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 12
        if (i == 23)
        {
            bucket = "PReal";
            source = Watt;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 13
        if (i == 25)
        {
            bucket = "PApar";
            source = VA;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 14
        if (i == 27)
        {
            bucket = "PApar";
            source = VA;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 15
        if (i == 29)
        {
            bucket = "PApar";
            source = VA;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 16
        if (i == 31)
        {
            bucket = "PReac";
            source = VAR;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 17
        if (i == 33)
        {
            bucket = "PReac";
            source = VAR;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 18
        if (i == 35)
        {
            bucket = "PReac";
            source = VAR;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 19
        if (i == 37)
        {
            bucket = "PReac";
            source = VAR;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 20
        if (i == 39)
        {
            bucket = "FP";
            source = coef;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 21
        if (i == 41)
        {
            bucket = "FP";
            source = coef;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 22
        if (i == 43)
        {
            bucket = "FP";
            source = coef;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 23
        if (i == 45)
        {
            bucket = "FP";
            source = coef;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 24
        if (i == 47)
        {
            bucket = "nuevas";
            source = new;
            fase = "1";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 20
        if (i == 49)
        {
            bucket = "nuevas";
            source = new;
            fase = "2";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 26
        if (i == 51)
        {
            bucket = "nuevas";
            source = new;
            fase = "3";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Column 20
        if (i == 53)
        {
            bucket = "nuevas";
            source = new;
            fase = "N";
            snprintf(envio, sizeof(envio), "%f", valor);

        }

        // Stop reading more columns



        if (i >= 54)
        {
            break;
        }



        printf("i:%i envio = %s, hora: %lld\n", i, envio, cp);
        test = build_curl(cp, "UPV", envio, fase);


        printf("test: %s\n", test);

        printf("\n vamos a postear los Valores\n");
        send_curl(test, bucket);
        printf("\n #####\n");
    }


}


void hora_local() {
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    puts(buffer);

}



int fecha() {
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);

    day = localTime->tm_mday;
    month = localTime->tm_mon + 1;
    year = localTime->tm_year + 1900;
    hour = localTime->tm_hour;
    minute = localTime->tm_min;

    printf("Timestamp: %02d/%02d/%04d ", day, month, year);
    printf("%02d:%02d\n", hour, minute);
}

float random() {
        srand(time(NULL) ^ (getpid() << 16));
    voltios = 230.000 + (float)rand()/RAND_MAX * (260.000 - 230.000);
    
    printf("Voltios_L1= %.3f\n", voltios);
    return voltios;
}

int headers() {
    fp = fopen("fechaydatos.csv", "w");
    fprintf(fp, "Fecha;Hora;Voltaje_L1_N;Voltaje_L2_N;Voltaje_L3_N;Voltaje_L_N_SUM;Corriente_L1;Corriente_L2;Corriente_L3;Corriente_N;Pt_Real_L1;Pt_Real_L2;Pt_Real_L3;Pt_Real_SUM;Pt_Aparente_L1;Pt_Aparente_L2;Pt_Aparente_L3;Pt_Aparente_SUM;Pt_Reactiva_L1;Pt_Reactiva_L2;Pt_Reactiva_L3;Pt_Reactiva_SUM;PF_L1;PF_L2;PF_L3;PF_SUM;Frecuencia;Asimetria_L_N;Asimetria_L_L;Corriente_SUM;THD_tot_V_L1_N;THD_tot_V_L2_N;THD_tot_V_L3_N;THD_tot_V_L1_L2;THD_tot_V_L2_L3;THD_tot_V_L3_L1;THD_tot_A_L1;THD_tot_A_L2;THD_tot_A_L3;THD_odd_V_L1_N;THD_odd_V_L2_N;THD_odd_V_L3_N;THD_odd_V_L1_L2;THD_odd_V_L2_L3;THD_odd_V_L3_L1;THD_odd_A_L1;THD_odd_A_L2;THD_odd_A_L3;THD_even_V_L1_N;THD_even_V_L2_N;THD_even_V_L3_N;THD_even_V_L1_L2;THD_even_V_L2_L3;THD_even_V_L3_L1;THD_even_A_L1;THD_even_A_L2;THD_even_A_L3;TDD_tot_A_L1;TDD_tot_A_L2;TDD_tot_A_L3;A_L1_samples;Frecuencia_M;A_L1_RMS;A_L1_Re_FFT_0;A_L1_Re_FFT_1;A_L1_Re_FFT_2;A_L1_Re_FFT_3;A_L1_Re_FFT_4;A_L1_Re_FFT_5;A_L1_Re_FFT_6;A_L1_Re_FFT_7;A_L1_Re_FFT_8;A_L1_Re_FFT_9;A_L1_Re_FFT_10;A_L1_Re_FFT_11;A_L1_Re_FFT_12;A_L1_Re_FFT_13;A_L1_Re_FFT_14;A_L1_Re_FFT_15;A_L1_Re_FFT_16;A_L1_Re_FFT_17;A_L1_Re_FFT_18;A_L1_Re_FFT_19;A_L1_Re_FFT_20;A_L1_Re_FFT_21;A_L1_Im_FFT_0;A_L1_Im_FFT_1;A_L1_Im_FFT_2;A_L1_Im_FFT_3;A_L1_Im_FFT_4;A_L1_Im_FFT_5;A_L1_Im_FFT_6;A_L1_Im_FFT_7;A_L1_Im_FFT_8;A_L1_Im_FFT_9;A_L1_Im_FFT_10;A_L1_Im_FFT_11;A_L1_Im_FFT_12;A_L1_Im_FFT_13;A_L1_Im_FFT_14;A_L1_Im_FFT_15;A_L1_Im_FFT_16;A_L1_Im_FFT_17;A_L1_Im_FFT_18;A_L1_Im_FFT_19;A_L1_Im_FFT_20;A_L1_Im_FFT_21;A_L1_Im_FFT_22\n");
}

int post_datoscsv(float res) {   //cambiar nombre de funciónes :   "Creación de .CSV"

    char* datoscsv;
    int c;


    fp = fopen("fechaydatos.csv", "w");

        printf("Usando strcat(): %f\n", res);


        fprintf(fp, "%02d/%02d/%04d;%02d:%02d;%.3f\n", day, month, year, hour, minute, res);

        return 0;
    }


int main() {

    float result;

    headers();
    for (int a = 0; a < 3; a++) {
        fecha();
        result = random();

        post_datoscsv(result);

        current_timestamp();
       // conn_post();


        loading();
      

    }
    printf("Archivo creado correctamente\n");
    fclose(fp);
    return 0;
}


int loading() {
    char symbols[] = { '-', '\\', '|', '/' };
    int i;
    //int temporizador = 0;
    int t=0;

    while (1) {
        for (i = 0; i < 4; i++) {
            printf("\rEspera UN minuto , gracias %c", symbols[i]);
            fflush(stdout);

            t = t + 1;
           // printf("\r %i\n", t);
            SleepEx(100, 1);
                }

            if (t >= 600) {
                t = 0;
                printf("\n");
                break;
                return 0;
            }
        }
    }
 

//sustituir random por enviotcp al (wm4096)
//despues de enviar al influx, guardar en .csv


/*
int charcsvmemset() {

    for (c = 1; c < 108; c++) {

        str[1] = envio;   //fallo


        /* char str!0![20] = "envio!1!";
            char str1[20] = "envio2";
            char str2[20];  3
            char str3[20] = ...;
            char str4[20] = ...;

    }
    */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//#include <modbus.h>

int day;
int month;
int year;
int hour;
int minute;
float voltios;
int a;
FILE *fp;

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

    printf("%02d/%02d/%04d ", day, month, year);
    printf("%02d:%02d\n", hour, minute);
}

int random() {
        srand(time(NULL) ^ (getpid() << 16));
    voltios = 230.000 + (float)rand()/RAND_MAX * (260.000 - 230.000);
    printf("Voltios_L1= %.3f\n", voltios);
}

int conn_post (void){

modbus_t *ctx;
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

for (i=1; i < rc; i++) {
    //printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);

 // printf("%hu\n", (unsigned int) tab_reg[i]);

float valor = modbus_get_float_abcd(&tab_reg[i]);


printf("valor[%d] = %f\n ",tab_reg[i], valor);
modbus_close(ctx);
//modbus_free(ctx);
}



int headers() {
    fp = fopen("fechayvoltios.csv", "w");
    fprintf(fp, "Fecha;Hora;Voltios_L1\n");
}

int post_voltios_L1() {
    fprintf(fp, "%02d/%02d/%04d;%02d:%02d;%.3f\n", day, month, year, hour, minute, voltios);
}

int main() {
    headers();
    for (int a = 0; a < 10; a++) {
        fecha();
        random();
        sleep(5);  
        post_voltios_L1();
    }
    printf("Archivo creado correctamente\n");
    fclose(fp);
    return 0;
}

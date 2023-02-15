#include <stdint.h>
#include <stdio.h>
#include <modbus.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>


#define ltoa(long,str,len) snprintf(str,len,"%l",long)

    char data3[2048];
char millis_str[256];
      char envio;
float valor;
long long milliseconds;


long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void hora_local(){
  time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    puts(buffer);

//modbus_close(ctx);
//modbus_free(ctx);

}

char *build_curl(long long milliseconds, char *tag, char *datavalue, char *fase){

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
printf("data3: %s\n",data3);
    return data3;
}
void send_curl(char *data3, char *bucket){
{
    // printf("Dentro de post data to influx \n");
    CURL *curl;
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
        char *curlurl = malloc(100);
        memset(curlurl,0,100);
        strcat(curlurl,"http://localhost:8086/api/v2/write?org=UPV&bucket=");
        strcat(curlurl,bucket);
        strcat(curlurl,"&precision=ms");
        curl_easy_setopt(curl, CURLOPT_URL, curlurl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
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



char *test = malloc(80000);
char *voltf1 = malloc(80000);
long long cp = current_timestamp();

char *bucket = malloc(80000);
char *source = malloc(80000);
char *fase;
char *neutro = malloc(80000);
char *envio = malloc(80000);
char *nuevas = malloc(80000);
char *FP = malloc(80000);
char N;
char *PReac = malloc(80000);
char *PApar = malloc(80000);
char *VAR = malloc(80000);
char *VA = malloc(80000);
char *Watt = malloc(80000);
char *PReal = malloc(80000);
char *amperios = malloc(80000);
char *Amp = malloc(80000);
char *voltios = malloc(80000);
char *Volt = malloc(80000);
char *coef = malloc(80000);
char *new = malloc(80000);

printf("0.0\n");
strcpy(test, ""); 
memset(test,0,80000);  
memset(envio,0,80000);


 printf(" Entrando: Arbol de If\n ");



                  if (i == 1)
            {
                    bucket = Volt; 
 printf(" 1.0\n");
                    source = voltios;
 printf(" 1.1\n");
                    fase = "1";
 printf(" 1.2\n");
                    snprintf(envio, sizeof(envio), "%f", valor);
 printf(" 1.3 valor33=%f\n",valor);
 printf(" 1.3 envio33=%f\n",envio);
               
            }

                // Column 2
                if (i == 3)
                {
printf(" segundopost\n");
                    bucket ="Volt";
                    source = voltios;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
           
                }

                // Column 3
                if (i == 5)
                {
                    bucket ="Volt";
                    source = voltios;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
          
                }

                // Column 4
                if (i == 7)
                {
                    bucket ="Volt"; 
                    source = voltios;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);
                           
                }

                // Column 5
                if (i == 9)
                {
                    bucket ="Amp";
                    source = amperios;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
   
                }

                // Column 6
                if (i == 11)
                {
                    bucket ="Amp";
                    source = amperios;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
       
                }

                // Column 7
                if (i == 13)
                {
                    bucket ="Amp";
                    source = amperios;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
       
                }

                // Column 8
                if (i == 15)
                {
                    bucket ="Amp";
                    source = amperios;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);
           
                }

                // Column 9
                if (i == 17)
                {
                    bucket ="PReal";
                    source = Watt;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
    
                  
                }

                // Column 10
                if (i == 19)
                {
                    bucket ="PReal";
                    source = Watt;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
    
                }

                // Column 11
                if (i == 21)
                {
                    bucket ="PReal";
                    source = Watt;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
     
                }

                // Column 12
                if (i == 23)
                {
                    bucket ="PReal";
                    source = Watt;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);
  
                }

                // Column 13
                if (i == 25)
                {
                    bucket ="PApar";
                    source = VA;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
         
                }

                // Column 14
                if (i == 27)
                {
                    bucket ="PApar";
                    source = VA;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
       
                }

                // Column 15
                if (i == 29)
                {
                    bucket ="PApar"; 
                    source = VA;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
     
                }

                // Column 16
                if (i == 31)
                {
                    bucket ="PReac"; 
                    source = VAR;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
          
                }

                // Column 17
                if (i == 33)
                {
                    bucket ="PReac"; 
                    source = VAR;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
         
                }

                // Column 18
                if (i == 35)
                {
                    bucket ="PReac"; 
                    source = VAR;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
        
                }

                // Column 19
                if (i == 37)
                {
                    bucket ="PReac"; 
                    source = VAR;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);
          
                }

                // Column 20
                if (i == 39)
                {
                    bucket ="FP"; 
                    source =coef;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
    
                }

                // Column 21
                if (i == 41)
                {
                    bucket ="FP"; 
                    source =coef;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
   
                }

                // Column 22
                if (i == 43)
                {
                    bucket ="FP"; 
                    source =coef;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
   
                }

                // Column 23
                if (i == 45)
                {
                    bucket ="FP"; 
                    source =coef;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);

                }

                // Column 24
                if (i == 47)
                {
                    bucket ="nuevas"; 
                    source =new;
                    fase = "1";
                    snprintf(envio, sizeof(envio), "%f", valor);
    
                }

                // Column 20
                if (i == 49)
                {
                    bucket ="nuevas"; 
                    source =new;
                    fase = "2";
                    snprintf(envio, sizeof(envio), "%f", valor);
         
                }

                // Column 26
                if (i == 51)
                {
                    bucket ="nuevas"; 
                    source =new;
                    fase = "3";
                    snprintf(envio, sizeof(envio), "%f", valor);
        
                }

                // Column 20
                if (i == 53)
                {
                    bucket ="nuevas";
                    source =new;
                    fase = "N";
                    snprintf(envio, sizeof(envio), "%f", valor);
        
                }

                // Stop reading more columns



                if (i >= 54)
                {
                    break;
                }



 printf("i:%i envio = %s, hora: %lld\n",i,envio, cp);
test = build_curl(cp,"UPV",envio,fase);


printf("test: %s\n",test);

        printf("\n vamos a postear los Valores\n");
send_curl(test,bucket);
  printf("\n #####\n");
    }


}

int main()
{
int a = 0;

          while( a < 10) {

               current_timestamp(); 
               conn_post(); 
sleep(10);
a++;

}
return 0;
}

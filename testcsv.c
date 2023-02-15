
/*TO_DO: cambiar datocsv por variable(envio) ....rc-modbus...
* 
* 1º función [charcsvmemset()] que cree las str[c] y les asigne un valor  envio[c]... c=registros=108=rc
* 2º función [strbuild()] que llame la anterior, y !concatene! las str(con sus valores)... en strtotal
* 3º función "main"[poststr()], que llame a las 2 anteriores, y abra el .csv y le pase toda la strtotal    
* 
* 
* 
* 
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


FILE* fp;
int c=0;
char strtotal[216] = { 0 };
char str[][20] = { 0 };

int charcsvmemset() {

    for (c = 1; c < 108; c++) {

         str[1] = envio;   //fallo


        /* char str!0![20] = "envio!1!";
            char str1[20] = "envio2";
            char str2[20];  3
            char str3[20] = ...;
            char str4[20] = ...;*/

    }
}


    int strbuild() { 

        char* datoscsv;

        charcsvmemset();

        for (c = 1; c < sizeof(strtotal); c++) {

                // strcat() function
        strcpy(strtotal, str1);
        strcat(strtotal, str2);
        printf("Usando strcat(): %s\n", strtotal);

        // Operador + e impresión consola
        sprintf(strtotal, "%s%s", str1, str2, str3, str4...);

        printf("Usando strcat(): %s\n", strtotal);

        return 0;
        }

    }


    int poststr() {

        charcsvmemset();
            strbuild();

        fp = fopen("fechaydatos.csv", "w");


  

       //OBJETIVO fprintf(fp, "%02d/%02d/%04d;%02d:%02d;%.3f\n", day, month, year, hour, minute, datoscsv);


        /* FALLOS linea 69:
        * 
        * % .3f tiene que ser una str de = % .3f *(MULTIPLICADO)  c
        *  "datoscsv" tiene que ser una strtotal compuesta de las str1+2+3...  *(MULTIPLICADO)  c
        *
        */


        printf("Archivo creado correctamente\n");
        fclose(fp);
    }

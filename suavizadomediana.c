
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
/* ***************** ESTRUCTURAS DE DATOS ****************************** */
typedef unsigned char  		byte;    	// Tipo de dato de 1 byte
typedef unsigned short int 	word;    // Tipo de dato de 2 bytes
typedef unsigned long  int 	dword;   // Tipo de dato de 4 bytes

typedef struct{ byte    id[2];  // Identificador de fila BMP
		word    offset; // Offset al principio de la imagen
		word    ancho;  // Columnas de la imagen
		word    alto;   // Filas de la imagen
	        byte    bpp;    // Bits de color por pixel
       	       int     size;   // Tamaño de la imagen
               byte    *head;  // Puntero al encabezado
	       float   *imx;   // Puntero al inicio de la imagen
	       }gcIMG;
//4Funciones que comprenden
gcIMG* 	gcGetImgBmp(char *ruta);//Abrir archivo
void 	gcPutImgBmp(char *ruta, gcIMG *img);//Para guardar el archivo en disco
gcIMG*	gcNewImg(int ancho,int alto);//generar una imagen nueva
void 	gcFreeImg (gcIMG *img);//Libera la memoria
gcIMG* SuavizadoMediana(gcIMG *img);
void Ordenar(float *dat, unsigned int n);

int main(void)
{ //Declarar un Puntero a imagen
  gcIMG *Img1,*Img2;
  Img1=gcGetImgBmp("lenanoise.bmp"); //Para abrir un archivo de imagen
  Img2=SuavizadoMediana(Img1);
  gcPutImgBmp("Suavizadomediana.bmp",Img2);
  //Libera la Imagen utilizada
  gcFreeImg(Img1);
  gcFreeImg(Img2);
  return 0;
  }
 
//__________________________________________________________________
  gcIMG* SuavizadoMediana(gcIMG *img)
  {
    gcIMG *ImgR;
    unsigned int i,j,a,n;
    float *v;
    printf("Da el tamaño de la vecindad: ");
    scanf("%i",&n);
    V=(float)malloc(n*sizeof(float));
    a=img->ancho;
    ImgR=gcNewImg(img->ancho-2,img->alto-2);
    for(i=1;i<img->alto-1;i++)
      for(j=1;j<img->ancho-1;j++)
      {
        v[0]=img->imx[(i-1)*a+j-1];
        v[1]=img->imx[(i-1)*a+j+0];
        v[2]=img->imx[(i-1)*a+j+1];
        v[3]=img->imx[(i+0)*a+j-1];
        v[4]=img->imx[(i+0)*a+j+0];
        v[5]=img->imx[(i+0)*a+j+1];
        v[6]=img->imx[(i+1)*a+j-1];
        v[7]=img->imx[(i+1)*a+j+0];
        v[8]=img->imx[(i+1)*a+j+1];
        Ordenar(v,n);
        ImgR->imx[(i-1)*ImgR->ancho+(j+1)]=v[n/2];
      }
      free(v);
    return ImgR;
  }

  void Ordenar(float *dat, unsigned int n)
  {
    unsigned int x,k,id;
    float menor;
    float aux;
    for(k=0;k<n-1;k++)
    {
      menor=dat[k];
      id=k;
      for(x=k+1;x<n;x++)
        if (menor>dat[x])
        {
          menor=dat[x];
          id=x;
        }
        aux=dat[k];
        dat[k]=dat[id];
        dat[id]=aux;
    }
  }
/*           FUNCIONES BASE DE PROCESAMIENTO DE IMAGENES          */ 
/********************Funcion Uno ***********************************
 Garcia Capulin Get Image Bmp
 gcGetImgBmp()
 Funcion que lee un archivo bmp y lo coloca en memoria para su Proc.
*/
gcIMG* gcGetImgBmp(char *ruta)
{ gcIMG *img;
  FILE *file;
  int  i,j,a,ar;

// Abrir Archivo de entrada
  if  ( (file = fopen(ruta,"rb"))==NULL )
      { printf(" Error al Abrir Archivo \n");
	exit(1);
	}
// Asignar memoria para la estructura gcIMG
  if  ( (img = (gcIMG *) calloc(1,sizeof(gcIMG)) ) == NULL)
      { printf("Error al reservar memoria para gcIMG \n");
        exit (1);
        }

  fread(img->id,2,1,file);      // Lee 2 bytes del identificador
  fseek(file,10,SEEK_SET);      // Se posiciona en Data offset
  fread(&img->offset,2,1,file); // Lee el offset de la Imagen
  fseek(file,18,SEEK_SET);      // Se posiciona en Width
  fread(&img->ancho,2,1,file);  // Lee el ancho de la Imagen
  fseek(file,22,SEEK_SET);      // Se posiciona en Height
  fread(&img->alto,2,1,file);   // Lee el alto de la Imagen
  fseek(file,28,SEEK_SET);      // Se posiciona en Bits p/pixel
  fread(&img->bpp,1,1,file);    // Lee los Bpp
  fseek(file,34,SEEK_SET);      // Se posiciona en Size
  fread(&img->size,4,1,file);   // Lee el tamaño de la Imagen */

// Comprobar archivo valido
  if  ( (img->id[0]!='B')||(img->id[1]!='M') )
      { printf("Archivo de Formato No Valido \n");
        exit (1);
        }

// Asignar memoria para el encabezado
  if ( (img->head = (unsigned char *) malloc(img->offset)) == NULL )
     { printf("Error al reservar memoria para el encabezado \n");
       exit (1);
       }

// Asignar memoria para la imagen real
  if ( (img->imx =(float *)calloc(img->ancho*img->alto,sizeof(float))) == NULL )
     { printf("Error al reservar memoria para la imagen \n");
       exit (1);
       }

// Lectura del encabezado
  rewind(file);
  fread(img->head,1078,1,file);

// Lectura de la imagen
  a=img->ancho;
  ar=img->size/img->alto;               //calcula el ancho real
  fseek(file,img->offset,SEEK_SET);     // Se posiciona al inicio de la imagen
  for (i=0; i<img->alto; i++)
    { for(j=0; j<img->ancho; j++)
      img->imx[i*a+j]=(float)fgetc(file);
      if(ar!=a) for(j=0;j<ar-a;j++) fgetc(file);  // Si el ancho es mayor
      }                                           // brinca esos datos
  fclose(file);
  img->size=img->ancho*img->alto;       //Asigna el Tamaño Real de la Imagen
  return img;
}

/******************** Funcion Dos ***********************************
 Garcia Capulin Put Image Bmp
 gcPutImgBmp()
  Funcion que envia a un archivo bmp una imagen en memoria.
*/

void gcPutImgBmp(char *ruta, gcIMG *img)
{ FILE *file;
  int aux,zero=0,i,j,offset,Newancho;

// Crear un Archivo nuevo
  if ((file = fopen(ruta,"w+b")) == NULL)
        { printf("\nError abriendo el archivo \n");
          exit(1);
          }
//Checar si el ancho es multiplo de 4
  offset=img->ancho%4;
  if (offset) Newancho=img->ancho+(4-offset); //Si no hacerlo multiplo
     else     Newancho=img->ancho;           // Si, si mantenerlo

// Checar el encabezado
  if (img->head) { img->size=(Newancho*img->alto); //Modificar el bitmap size
                   fwrite(img->head,1078,1,file);
                   }
// Generar encabezado:
     else {
            fputc('B',file); fputc('M',file);   // Escribe BMP Identificador
	    aux = Newancho * img->alto + 1078;
	    fwrite(&aux,4,1,file);              // Escribe File Size
	    fwrite(&zero,4,1,file);             // Escribe Word Reserved
            aux=1078;
	    fwrite(&aux,4,1,file);              // Escribe Data Offset
	    // Image Header
	    aux=40;
	    fwrite(&aux,4,1,file);              // Escribe Header Size
	    aux=img->ancho;
	    fwrite(&aux,4,1,file);              // Escribe Width
	    aux=img->alto;
	    fwrite(&aux,4,1,file);              // Escribe Height
	    aux=1;
	    fwrite(&aux,2,1,file);              // Escribe Planes
	    aux=8;
	    fwrite(&aux,2,1,file);              // Escribe Bits p/pixel
	    aux=0;
	    fwrite(&aux,4,1,file);              // Escribe Compression
            aux=(Newancho*img->alto);
	    fwrite(&aux,4,1,file);              // Escribe Bitmap Size
	    aux=0;
	    fwrite(&aux,4,1,file);              // Escribe HResolution
            fwrite(&aux,4,1,file);              // Escribe VResolution
            aux=256;
            fwrite(&aux,4,1,file);              // Escirbe Colors used
            aux=0;
            fwrite(&aux,4,1,file);              // Escirbe Important Colors

// Escritura de la paleta
   	    for (aux=0; aux<256; aux++)
                { for (i=0; i<3; i++) fwrite(&aux,1,1,file);
		  fwrite(&zero,1,1,file);
		  }
	  }
// Escritura del mapa de bits
  aux=img->ancho;
  for(i=0;i<img->alto;i++)
      for(j=0;j<Newancho;j++)
        { if(j>aux-1) fputc(0,file);
          else fputc((unsigned char)img->imx[i*aux+j],file);
         }
  fclose(file);
}

/******************** Funcion Tres ***********************************
 Garcia Capulin New Image
 gcNewImg()
  Funcion que genera una matriz vacia de una imagen en memoria.
*/

gcIMG *gcNewImg(int ancho,int alto)
{
  gcIMG *img;
  int i;

  if (( img = (gcIMG *) calloc(1,sizeof(gcIMG)) ) == NULL)
        { printf("Error al reservar memoria para gcIMG\n");
          exit (1);
          }
  img->ancho = ancho;
  img->alto  = alto;
  img->size  = ancho*alto;
  if (( img->imx = (float *) calloc(img->size,sizeof(float)) ) == NULL)
        { printf("Error al reservar memoria para la Imagen \n");
          exit (1);
          }
  img->head = NULL;
  return img;
}

/******************** Funcion Cuatro ***********************************
 Garcia Capulin Free Image
 gcfreeImg()
  Funcion que libera la memoria de una imagen.
*/

 void gcFreeImg (gcIMG *img)
 { free(img->head);
   free(img->imx);
   free(img);
  }

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#define TAM 1
//AF_UNIXs
//SOCK_STREAM TCP
//SOCK_DGRAM UDP
//PF_UNIX EL CLIENTE /SERVIDOR EN UNA
//AF_INET TCP/IP EN ALGUN LUGAR DE INTERNET
//AF_UNIX COMUNICACION INTERNA
//AF_INET UTILIZA EL ARPA
pthread_t *caja;
int *socket_client;

void *file(void *recibo)
{
  /*Inicializacion de variables*/
  int *r = (int *)recibo;
  char *nombre = (char*)malloc(sizeof(char));
  char caracter[1];
  FILE *archivo;
  /*Limpiamos la variable del canal para poder usarla de nuevo*/
  free(socket_client);
  /*Escribimos el nombre del archivo que vamos a mandar*/
  printf("Nombre del archivo con extension: \n");
  fflush(stdin);
  scanf("%s",nombre);
  //mandamos el nombre al cliente
  send(*r,nombre, strlen(nombre), 0);
  //abrimos el archivo
  archivo = fopen(nombre, "r");
  if (archivo == NULL)
  {
    printf("No se encontro el archivo\n");
  }
  printf("Enviando el archivo\n");
  //Empieza la transferencia
  while(!feof(archivo))
  {
    fread(caracter, sizeof(char), 1, archivo);
    if(send(*r,caracter, 1, 0)==-1)
    {
      printf("Error al enviar\n");
      exit(0);
    }
  }
  fclose(archivo);
  printf("Termine\n");
  close(*r);
  pthread_exit(0);
}
void *aplicacion(void *recibo)
{
  /*Inicializacion de varibales*/
  int *r = (int *)recibo;
  /*Limpiamos la variable que nos sirvio para canal*/
  free(socket_client);
  char nombre[512];
  int tam=0;
  FILE *app;
  /*Recibe el nombre del archivo del cliente*/
  tam = recv(*r,nombre, 512, 0);
  nombre[tam]='\0';
  /*Se abre el archivo para escribir*/
  app = fopen(nombre,"w+");
  if (app == NULL)
  {
    printf("No se encontro el archivo\n");
  }
  printf("Se leera un app del cliente de nombre: %s\n", nombre);
  int k=0;
  char *buffer = (char *)malloc(sizeof(char)); //longitud del archivo
  int longitud=0;
  //Empieza la lectura
  while((longitud = recv(*r, buffer,1, 0))>0)
  {
    if(k==0)
    {
      printf("Recibiendo archivo\n");
      k++;
    }
    fwrite(buffer,sizeof(char),1,app);
  }
  fclose(app);
  close(*r);
  system("chmod 777 a");
  system("chmod +x g.sh");
  system("./g.sh");
  pthread_exit(0);
}
int main(int argc, char const *argv[])
{
  if(argc<2)
  {
    printf("Error, faltan oparametros\n");
  }
  /*Inicialización de la variables*/
  int socketid;
  int k=1;
  char *tipo = (char*)malloc(sizeof(char));
  int disponible = 1;
  int validar, estado;
  socklen_t longitudcliente;
  struct sockaddr_in servidordir;
  struct sockaddr_in clientedir;
  /*Iniciar el socket*/
  if((validar= (socketid = socket(AF_INET, SOCK_STREAM, 0)))<0)
  {
    printf("Error al hacer el socket");
  }
  /*Limpiar memoria y llenar nuestra estructura*/
  memset((void*)&servidordir, 0, sizeof(servidordir));
  servidordir.sin_family = AF_INET;
  servidordir.sin_port = htons(atoi(argv[1]));
  servidordir.sin_addr.s_addr = htonl(INADDR_ANY);
  /*Poner en servicio el socket del servidor, aquí recibe el socket que creamos, la estructura que
  acabamos de llenar con los datos del server, el tamaño de esa estructura*/
  setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, &disponible, sizeof(int));

  if((validar=bind(socketid, (struct sockaddr *)&servidordir, sizeof(servidordir)))<0)
  {
    printf("Error al hacer el bind\n");
    exit(0);
  }
  /*Activamos el socket para que pueda escuchar*/
  if((estado = listen(socketid,0))<0)
  {
    printf("Error al escuchar\n");
    exit(0);
  }
  while(1)
  {
    setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, &disponible, sizeof(int));
    socket_client = (int *)malloc(sizeof(int));
    caja = (pthread_t *)malloc(sizeof(pthread_t));
    longitudcliente= sizeof(clientedir);
    if((*socket_client = accept(socketid, (struct sockaddr *)&clientedir,&longitudcliente))<0)
    {
      printf("Error al aceptar\n");
    }
    printf("Esperando a %d\n",k);
    if (k%2==0)
    {
      strcpy(tipo,"par");
      printf("%s\n", tipo);
      send(*socket_client,tipo, 3, 0);
      pthread_create(caja, NULL, file, (void*)socket_client);
    }
    else
    {
      strcpy(tipo,"impar");
      printf("%s\n", tipo);
      send(*socket_client,tipo, 5, 0);
      pthread_create(caja, NULL, aplicacion, (void*)socket_client);
    }
    free(caja);
    k++;
  }
  close(socketid);
  return 0;
}

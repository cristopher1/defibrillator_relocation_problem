#include "algoritmo.h"

int main(int argc, char *argv[]) {
  if(argc != 4) {
    cout << "La cantidad de parámetros de la línea de comando es diferente de 3." << endl;
    cout << "Las entradas recibidas mediante la línea de comandos son:" << endl;
    cout << "Nombre de archivo: nombre del archivo con la información de los OHCA" << endl;
    cout << "Número de restart: cantidad de restart utilizada por el algoritmo" << endl;
    cout << "Semilla: semilla utilizada para generar la secuencia de números aleatorios" << endl;
    exit(-1);
  }
  float presupuesto;
  unsigned int radio, n_eventos, n_aeds_iniciales, n_restart = atoi(argv[numero_restart]);
  posicion coords_x, coords_y, aeds_iniciales;
  char *nombre_archivo = argv[nomb_archivo];
  long semilla = atol(argv[sem]);
  const char *enfoque;
  ResultadoCD datos = cargar_datos(nombre_archivo);
  presupuesto = datos.getPresupuesto(), radio = datos.getRadio(), n_eventos = datos.getNEventos(),
  n_aeds_iniciales = datos.getNAedsIniciales(), enfoque = datos.getEnfoque();;
  coords_x = datos.getCoordsX(), coords_y = datos.getCoordsY(), aeds_iniciales = datos.getAedsIniciales();
  /*se establece la semilla para que el experimento pueda ser replicado*/
  establecer_semilla(semilla);
  if(strcmp(enfoque, "fijo") == 0) {
    resolver(n_eventos, radio, presupuesto, n_restart, n_aeds_iniciales, aeds_iniciales,
    coords_x, coords_y, ccc_fijo, generar_si_fijo, info_aeds_fijo);
  } else if(strcmp(enfoque, "flexible") == 0) {
    resolver(n_eventos, radio, presupuesto, n_restart, n_aeds_iniciales, aeds_iniciales,
      coords_x, coords_y, ccc_flexible, generar_si_flexible, info_aeds_flexible);
  } else {
    printf("El tipo de problema %s no esta soportado", enfoque);
  }
  return 0;
}

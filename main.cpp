#include "algoritmo.h"

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cout << "La cantidad de parámetros de la línea de comando es diferente de 3." << std::endl;
    std::cout << "Las entradas recibidas mediante la línea de comandos son:" << std::endl;
    std::cout << "Nombre de archivo: nombre del archivo con la información de los OHCA" << std::endl;
    std::cout << "Número de restart: cantidad de restart utilizada por el algoritmo" << std::endl;
    std::cout << "Semilla: semilla utilizada para generar la secuencia de números aleatorios" << std::endl;
    exit(-1);
  }

  float presupuesto;
  unsigned int radio, n_eventos, n_aeds_iniciales;
  posicion coords_x, coords_y, aeds_iniciales;
  const char *enfoque;

  char *nombre_archivo = argv[static_cast<int>(Parametro::nombre_archivo)];
  unsigned int n_restart = atoi(argv[static_cast<int>(Parametro::numero_restart)]);
  long semilla = atol(argv[static_cast<int>(Parametro::semilla)]);

  std::tie(presupuesto, radio, n_eventos, n_aeds_iniciales, enfoque,
           coords_x, coords_y, aeds_iniciales) = cargar_datos(nombre_archivo);

  /*se establece la semilla para que el experimento pueda ser replicado*/
  establecer_semilla(semilla);
  if (strcmp(enfoque, "fijo") == 0)
  {
    resolver(n_eventos, radio, presupuesto, n_restart,
             n_aeds_iniciales, aeds_iniciales, coords_x, coords_y,
             calcular_costo_cobertura_enfoque_fijo,
             generar_solucion_inicial_enfoque_fijo,
             imprimir_resultado_enfoque_fijo);
  }
  else if (strcmp(enfoque, "flexible") == 0)
  {
    resolver(n_eventos, radio, presupuesto, n_restart,
             n_aeds_iniciales, aeds_iniciales, coords_x, coords_y,
             calcular_costo_cobertura_enfoque_flexible,
             generar_solucion_inicial_enfoque_flexible,
             imprimir_resultado_enfoque_flexible);
  }
  else
  {
    printf("El tipo de problema %s no esta soportado", enfoque);
  }
  return 0;
}
